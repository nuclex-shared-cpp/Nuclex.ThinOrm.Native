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

#ifndef NUCLEX_THINORM_MIGRATIONS_MIGRATIONRUNNER_H
#define NUCLEX_THINORM_MIGRATIONS_MIGRATIONRUNNER_H

#include "Nuclex/ThinOrm/Config.h"

#include <memory> // for std::unique_ptr<>
#include <vector> // for std::vector<>
#include <cstddef> // for std::size_t
#include <unordered_set> // for std::unordered_set<>

namespace Nuclex::ThinOrm::Connections {
  class Connection;
  class ConnectionPool;
}
namespace Nuclex::ThinOrm::Migrations {
  class Migration;
}

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Handles upgrading (and downgrading) database schema version</summary>
  /// <remarks>
  ///   <para>
  ///     You'd generally use the <see cref="MigrationRunner" /> during your application's
  ///     startup to ensure that the database, if it belongs to a prior version of your
  ///     application, is automatically upgraded to the latest version.
  ///   </para>
  ///   <para>
  ///     To make use of the <see cref="MigrationRunner" />, first, write migrations as
  ///     individual classes, i.e. <code>class M0000_UsersAndRoles : Migration {};</code>,
  ///     then either use the <see cref="GlobalMigrationRepository" /> to register your
  ///     schema migrations globally or manually add them to the <see cref="MigrationRunner" />.
  ///   </para>
  ///   <para>
  ///     Then simply invoke <see cref="UpgradeToLatestSchema" /> and all migrations will
  ///     be applied in order.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE MigrationRunner {

    /// <summary>Initializes a new migration runner on the specified connection</summary>
    /// <param name="connection">Database connection the migration runner will use</param>
    public: NUCLEX_THINORM_API MigrationRunner(
      const std::shared_ptr<Connections::Connection> &connection
    );
    /// <summary>Initializes a new migration runner using a connection pool</summary>
    /// <param name="pool">
    ///   Connection pool from which the migration runner will, at the time it is needed,
    ///   borrow a connection to perform the schema migration
    /// </param>
    public: NUCLEX_THINORM_API MigrationRunner(
      const std::shared_ptr<Connections::ConnectionPool> &pool
    );
    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API ~MigrationRunner();

    /// <summary>Sets a custom name for the table in which migrations are recorded</summary>
    /// <param name="tableName">Name that should be used for the migration table</param>
    /// <remarks>
    ///   As should be obvious, if you use a custom migration table name, do not ever change
    ///   it. Otherwise, you'd either have to manually and diligently rename the table in
    ///   every existing database your application comes into contact with or end up with
    ///   all migrations being applied a second time, potentially wreaking havoc.
    /// </remarks>
    public: NUCLEX_THINORM_API void SetMigrationTableName(const std::u8string &tableName);

    // TODO: Implement logger support
    //public: NUCLEX_THINORM_API void SetLogger();

    /// <summary>Upgrades the database to the highest schema version available</summary>
    /// <remarks>
    ///   This is the method you'd normally call once during your application's startup.
    /// </remarks>
    public: NUCLEX_THINORM_API void UpgradeToLatestSchema();

    /// <summary>Uprages or downgrades the database schema to the specified version</summary>
    /// <param name="schemaVersion">
    ///   Specific version the database schema will be upgraded or downgraded to
    /// </param>
    public: NUCLEX_THINORM_API void MoveToSchemaVersion(std::size_t schemaVersion);

    /// <summary>Adds the specified migration to the runner for execution</summary>
    /// <param name="migration">Migration that will be added to the runner</param>
    public: NUCLEX_THINORM_API void AddMigration(const std::shared_ptr<Migration> &migration);

    /// <summary>Adds the specified migration to the runner for execution</summary>
    /// <typeparam name="TMigration">
    ///   Migration class that will be added to the runner
    /// </typeparam>
    public: template<typename TMigration>
    NUCLEX_THINORM_API inline void AddMigration();

    /// <summary>
    ///   Adds all migrations in the <see cref="GlobalMigrationReposiory" /> to the runner
    /// </summary>
    public: NUCLEX_THINORM_API void AddAllGlobalMigrations();

    /// <summary>
    ///   Adds all migrations in the <see cref="GlobalMigrationReposiory" /> that apply to
    ///   the database with the specified context to the runner
    /// </summary>
    /// <param name="dataContextType">
    ///   Type of the data context for the database whose migrations will be added to the runner
    /// </param>
    public: NUCLEX_THINORM_API void AddAllGlobalMigrations(const std::type_info *dataContextType);

    /// <summary>
    ///   Adds all migrations in the <see cref="GlobalMigrationReposiory" /> that apply to
    ///   the database with the specified context to the runner
    /// </summary>
    /// <typeparam name="TDataContext">
    ///   Context that identifies the database whose migrations will be added to the runner
    /// </typeparam>
    public: template<typename TDataContext>
    NUCLEX_THINORM_API void AddAllGlobalMigrations();

    /// <summary>Sorts the migrations in the list by their database schema version</summary>
    private: void sortMigrationsBySchemaVersion();

    /// <summary>Throws an exception is a schema version appears twice</summary>
    private: void requireDistinctSchemaVersions();

    /// <summary>Performs the actual migration work</summary>
    /// <param name="connection">
    ///   Connection, either the one given to the migration runner or a borrowed one
    /// </param>
    /// <param name="schemaVersion">Schema version to migrate to, nullptr for latest</param>
    private: void migrate(
      const std::shared_ptr<Connections::Connection> &connection, std::size_t *schemaVersion
    );

    /// <summary>List of migration steps</summary>
    private: typedef std::vector<std::shared_ptr<Migration>> MigrationVector;

    /// <summary>Connection that will be used to upgrade the database schema</summary>
    private: std::shared_ptr<Connections::Connection> connection;
    /// <summary>Pool from which a connection will be taken as the upgrade happens</summary>
    private: std::shared_ptr<Connections::ConnectionPool> pool;

    /// <summary>Migrations that have been added to the migration runner</summary>
    private: MigrationVector migrations;
    /// <summary>Name of the table in which applied migrations are recorded</summary>
    private: std::u8string tableName;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TMigration>
  inline void MigrationRunner::AddMigration() {
    static_assert(
      std::is_default_constructible<TMigration>::value,
      u8"The templated AddMigration() method can only be used on migrations that have "
      u8"a parameterless default constructor. If you are forwardig the schema version, "
      u8"don't do that. Instead, pass a hard-coded and schema version to the base class "
      u8"contructor and give your migration itself a parameterless default constructor."
    );
    static_assert(
      std::is_base_of<Migration, TMigration>::value,
      //std::is_convertible<Migration *, TMigration *>::value,
      u8"Migrations have to publicly inherit the 'Migration' base class in order to "
      u8"quality as migrations."
    );

    AddMigration(std::make_shared<TMigration>());
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void MigrationRunner::AddAllGlobalMigrations() {
    static const std::type_info &typeInfo = typeid(TDataContext);
    AddAllGlobalMigrations(&typeInfo);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_MIGRATIONRUNNER_H
