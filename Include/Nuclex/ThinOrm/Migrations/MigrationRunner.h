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

    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API MigrationRunner();
    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API virtual ~MigrationRunner();

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
    public: NUCLEX_THINORM_API void AddMigration(std::unique_ptr<Migration> migration);

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

    /// <summary>Migrations that have been added to the migration runner</summary>
    private: std::vector<std::unique_ptr<Migration>> migrations;

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

    AddMigration(std::make_unique<TMigration>());
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
