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

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()

#include "../Utilities/QStringConverter.h" // for QStringConverter, U8Chars()

#include <algorithm> // for std::sort()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Default name for the table that records applied migrations</summary>
  /// <remarks>
  ///   Obviously, do not ever change this. Users can override the name by calling
  ///   the <see cref="Nuclex.ThinOrm.Migrations.MigrationRunner.SetMigrationTableName" />
  ///   method. Changing it after a database has been initialized would cause us to assume
  ///   that no migrations have been applied yet and run them all again, wreaking havoc.
  /// </remarks>
  const std::u8string DefaultMigrationTableName(u8"migrations", 10);

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Comparison functor that checks if one migration's schema version is earlier (less)
  ///   than another's
  /// </summary>
  class MigrationSchemaVersionLess {
    //std::less<std::unique_ptr<Nuclex::ThinOrm::Migrations::Migration>>

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
    if(count == 0) {
      return;
    }

    std::size_t schemaVersion = this->migrations[0]->GetTargetSchemaVersion();
    for(std::size_t index = 1; index < count; ++index) {

      // Assuming migrations are sorted by increasing schema versions, if the same
      // schema version is repeated, it means this verification step has failed.
      std::size_t nextSchemaVersion = this->migrations[index]->GetTargetSchemaVersion();
      if(schemaVersion == nextSchemaVersion) {
        std::u8string message(u8"Schema version '", 16);
        Nuclex::Support::Text::lexical_append(message, schemaVersion);
        message.append(u8"' was declared by more than one migration and is ambiguous", 58);
        throw Errors::AmbiguousSchemaVersionError(message);
      }

      // If the schema versions are not sorted, our technique of linearly checking for
      // repeated schema version is useless, so we need to check for that, too.
      if(nextSchemaVersion < schemaVersion) {
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

    // TODO: Apply schema migration towards the target schema version
    throw std::runtime_error(
      reinterpret_cast<const char *>(u8"Not implemented yet")
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
