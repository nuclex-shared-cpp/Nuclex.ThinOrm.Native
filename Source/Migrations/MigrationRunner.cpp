#pragma region Apache License 2.0
/*
Nuclex Native Framework
Copyright (C) 2002-2024 Markus Ewald / Nuclex Development Labs

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma endregion // Apache License 2.0

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_THINORM_SOURCE 1

#include "Nuclex/ThinOrm/Migrations/MigrationRunner.h"
#include "Nuclex/ThinOrm/Migrations/Migration.h"
#include "Nuclex/ThinOrm/Migrations/GlobalMigrationRepository.h"

#include "Nuclex/ThinOrm/Connections/ConnectionPool.h"
#include "Nuclex/ThinOrm/Connections/Connection.h"
#include "Nuclex/ThinOrm/Errors/AmbiguousSchemaVersionError.h"

#include "./Repositories/MigrationRecordRepository.h"

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()

#include <algorithm> // for std::sort()
#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Default name for the table that records applied migrations</summary>
  /// <remarks>
  ///   Obviously, do not ever change this. Users can override the name by calling
  ///   the <see cref="Nuclex.ThinOrm.Migrations.MigrationRunner.SetMigrationTableName" />
  ///   method. Changing it after a database has been initialized would cause us to assume
  ///   that no migrations have been applied yet and run them all again, wreaking havoc.
  /// </remarks>
  const std::u8string DefaultMigrationTableName(u8"Migrations", 10);

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Comparison functor that checks if one migration's schema version is earlier (less)
  ///   than that of another migration
  /// </summary>
  class MigrationSchemaVersionLess {
    //std::less<std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration>>

    /// <summary>
    ///   Checks if the first migration's schema version is 'less' than the second's
    /// </summary>
    /// <param name="left">First migration step whose schema version to compare</param>
    /// <param name="right">Second migration step whose schema version to compare</param>
    /// <returns>True if the first migration's schema version is 'less'</returns>
    public: bool operator()(
      const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &left,
      const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &right
    ) const noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

  bool MigrationSchemaVersionLess::operator()(
    const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &left,
    const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &right
  ) const noexcept {
    return left->GetTargetSchemaVersion() < right->GetTargetSchemaVersion();
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Borrows a connection from a connection pool and handels returning it</summary>
  class ConnectionBorrowScope {

    /// <summary>Initializes a new borrow scope and borrows a connection</summary>
    /// <param name="pool">Pool from which the connection will be borrowed</param>
    public: inline ConnectionBorrowScope(
      const std::shared_ptr<Nuclex::ThinOrm::Connections::ConnectionPool> &pool
    );

    /// <summary>Returns the borrowed connection to the pool</summary>
    public: inline ~ConnectionBorrowScope();

    /// <summary>Accesses the borrowed connection that scope is holding on to</summary>
    /// <returns>The borrowed connection</returns>
    public: inline const std::shared_ptr<Nuclex::ThinOrm::Connections::Connection> &Get() const;

    /// <summary>Pool from which the connection has been borrowed</summary>
    private: const std::shared_ptr<Nuclex::ThinOrm::Connections::ConnectionPool> &pool;
    /// <summary>Connection that has been borrowed from the pool</summary>
    private: std::shared_ptr<Nuclex::ThinOrm::Connections::Connection> connection;

  };

  // ------------------------------------------------------------------------------------------- //

  inline ConnectionBorrowScope::ConnectionBorrowScope(
    const std::shared_ptr<Nuclex::ThinOrm::Connections::ConnectionPool> &pool
  ) :
    pool(pool),
    connection(pool->BorrowConnection()) {}

  // ------------------------------------------------------------------------------------------- //

  inline ConnectionBorrowScope::~ConnectionBorrowScope() {
    this->pool->ReturnConnection(this->connection);
  }

  // ------------------------------------------------------------------------------------------- //

  inline const std::shared_ptr<
    Nuclex::ThinOrm::Connections::Connection
  > &ConnectionBorrowScope::Get() const {
    return this->connection;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::MigrationRunner(
    const std::shared_ptr<Connections::Connection> &connection
  ) :
    connection(connection),
    pool(),
    migrations(),
    tableName(DefaultMigrationTableName) {}

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::MigrationRunner(
    const std::shared_ptr<Connections::ConnectionPool> &pool
  ) :
    connection(),
    pool(pool),
    migrations(),
    tableName(DefaultMigrationTableName) {}

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::~MigrationRunner() = default;

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::UpgradeToLatestSchema() {
    sortMigrationsBySchemaVersion();
    requireDistinctSchemaVersions();

    if(static_cast<bool>(this->connection)) {
      migrate(this->connection, nullptr);
    } else {
      ConnectionBorrowScope borrowScope(this->pool);
      migrate(borrowScope.Get(), nullptr);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::MoveToSchemaVersion(std::size_t schemaVersion) {
    sortMigrationsBySchemaVersion();
    requireDistinctSchemaVersions();

    if(static_cast<bool>(this->connection)) {
      migrate(this->connection, &schemaVersion);
    } else {
      ConnectionBorrowScope borrowScope(this->pool);
      migrate(borrowScope.Get(), &schemaVersion);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddMigration(const std::shared_ptr<Migration> &migration) {
    this->migrations.push_back(migration);
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations() {
    AddAllGlobalMigrations(nullptr);
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations(const std::type_info *dataContextType) {
    const MigrationVector &globalMigrations = (
      GlobalMigrationRepository::GetUnsortedMigrations(dataContextType)
    );

    std::size_t count = globalMigrations.size();
    for(std::size_t index = 0; index < count; ++index) {
      this->migrations.push_back(globalMigrations[index]);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::sortMigrationsBySchemaVersion() {
    std::sort(
      this->migrations.begin(),
      this->migrations.end(),
      MigrationSchemaVersionLess()
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::requireDistinctSchemaVersions() {
    std::size_t count = this->migrations.size();
    if(count == 0) [[unlikely]] {
      return;
    }

    std::size_t schemaVersion = this->migrations[0]->GetTargetSchemaVersion();
    for(std::size_t index = 1; index < count; ++index) {

      // Assuming migrations are sorted by increasing schema versions, if the same
      // schema version is repeated, it means this verification step has failed.
      std::size_t nextSchemaVersion = this->migrations[index]->GetTargetSchemaVersion();
      if(schemaVersion == nextSchemaVersion) [[unlikely]] {
        std::u8string message(u8"Schema version '", 16);
        Nuclex::Support::Text::lexical_append(message, schemaVersion);
        message.append(u8"' was declared by more than one migration and is ambiguous", 58);
        throw Errors::AmbiguousSchemaVersionError(message);
      }

      // If the schema versions are not sorted, our technique of linearly checking for
      // repeated schema version is useless, so we need to check for that, too.
      if(nextSchemaVersion < schemaVersion) [[unlikely]] {
        assert(
          (nextSchemaVersion < schemaVersion) &&
          u8"Migration steps must be sorted by schema version"
        );
        throw std::logic_error( // even with assertions disabled, do not let it through!
          reinterpret_cast<const char *>(
            u8"Internal error: migration steps were not sorted by their declared "
            u8"schema vesions by the time 'requireDistinctSchemaVersions() was called."
          )
        );
      }

    } // for each migration index
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::migrate(
    const std::shared_ptr<Connections::Connection> &connection, std::size_t *schemaVersion
  ) {
    bool isDatabaseInitialized = connection->DoesTableOrViewExist(this->tableName);

    Repositories::MigrationRecordRepository repository(connection, this->tableName);

    // Figure out which migrations have already been applied. If the migration records
    // table doesn't exist yet, create it and skip fetching it.
    std::unordered_set<std::size_t> appliedMigrations;
    if(!isDatabaseInitialized) [[unlikely]] {
      repository.CreateTable();
    } else {
      appliedMigrations = repository.FetchAllAppliedSchemaVersions();
    }

    std::size_t registeredMigrationCount = this->migrations.size();

    // First, revert any migrations that have been applied but should no longer be so
    // given the target schema version. If the schema version is a null pointer, we can
    // skip this step because the caller wants all migrations to be applied.
    if(schemaVersion != nullptr) {
      for(std::size_t index = registeredMigrationCount; 0 < index;) {
        --index;

        const std::shared_ptr<Migration> &migration = this->migrations[index];
        std::size_t migrationTargetSchemaVersion = migration->GetTargetSchemaVersion();

        // Figure out if this migration should be reverted given the schema version
        // the database should be reverted to
        bool shouldBeReverted = (*schemaVersion < migrationTargetSchemaVersion);

        // If the migration should be reverted and the migration records table show
        // that it is currently applied on the database, revert the migration now.
        bool isApplied = appliedMigrations.contains(migrationTargetSchemaVersion);
        if(shouldBeReverted && isApplied) {

          // Revert the migration and immediately remove it from the migration records
          // so it won't be reverted a second time if some migration further down the line
          // causes an exception and fails the overall migration.
          migration->Down(*connection.get());
          repository.RemoveMigration(migrationTargetSchemaVersion);
        }
      } // for each migration index in reverse
    } // if explicit schema version specified

    // Now deal with migrations that should be applied. This is the default and will
    // step through all migrations in order of their target schema version. Normally,
    // no new migrations should pop up inbetween already applied migrations, but we
    // allow this in order to support developers who want to have a gallery of
    // independent "baselining" migrations.
    for(std::size_t index = 0; index < registeredMigrationCount; ++index) {
      const std::shared_ptr<Migration> &migration = this->migrations[index];
      std::size_t migrationTargetSchemaVersion = migration->GetTargetSchemaVersion();

      // Figure out if this migration should be applied in order to move to
      // the target database schema version we were given
      bool shouldBeApplied;
      if(schemaVersion == nullptr) {
        shouldBeApplied = true; // apply all if target schema version is a null pointer
      } else {
        shouldBeApplied = (*schemaVersion >= migrationTargetSchemaVersion);
      }

      // If the migration should be applied and the migration records table shows
      // that it has not yet been applied, run the migration now.
      bool isApplied = appliedMigrations.contains(migrationTargetSchemaVersion);
      if(shouldBeApplied && !isApplied) {
        std::u8string name = migration->GetName();

        // Apply the migration and immediately record it. In case another migration will
        // fail, we at least leave a record of the successfully applied migrations,
        // aiding the user in reconstructing what went wrong (and avoiding reapplication
        // of successful migrations once the failing migration is fixed).
        migration->Up(*connection.get());
        repository.AddMigration(
          Entities::MigrationRecord(
            migrationTargetSchemaVersion,
            DateTime::FromSecondsSinceUnixEpoch(0), // TODO: Implement DateTime::GetNow()
            name.empty() ? std::optional<std::u8string>() : name
          )
        );
      } // if migration should be applied but isn't
    } // for each migration index
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
