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

#ifndef NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_MIGRATIONRECORDREPOSITORY_H
#define NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_MIGRATIONRECORDREPOSITORY_H

#include "Nuclex/ThinOrm/Config.h"
#include "../Entities/MigrationRecord.h" // for MigrationRecord

#include <memory> // for std::shared_ptr<>
#include <unordered_set> // for std::unordered_set<>

namespace Nuclex::ThinOrm::Connections {
  class Connection;
}

namespace Nuclex::ThinOrm::Migrations::Repositories {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Manages reading, inserting and updating informations about applied migrations
  ///   in a database
  /// </summary>
  class MigrationRecordRepository {

    /// <summary>
    ///   Initializes a migration repository working on the specified connection
    /// </summary>
    /// <param name="connection">Connection the repository will work through</param>
    /// <param name="tableName">Name to use for the migrations table</param>
    public: MigrationRecordRepository(
      const std::shared_ptr<Connections::Connection> &connection,
      const std::u8string &tableName
    );

    /// <summary>Creates the table in which applied migrations are recorded</summary>
    /// <param name="connection">Connection through which the table will be created</param>
    /// <param name="tableName">Name to give the migrations table</param>
    public: void CreateTable();

    /// <summary>Fetches all schema migrations that have been recorded as a set</summary>
    /// <returns>A set of all schema versions whose migrations have been applied</returns>
    public: std::unordered_set<std::size_t> FetchAllAppliedSchemaVersions() const;

    /// <summary>Records a new migration into the table</summary>
    /// <param name="newMigration">Migration that will be added itno the table</param>
    public: void AddMigration(const Entities::MigrationRecord &newMigration);

    /// <summary>Removes the record for the migration to the specified version</summary>
    /// <param name="schemaVersion">
    ///   Schema version for which the migration record will be removed if present
    /// </param>
    /// <returns>
    ///   True if a migration record for the schema version existed and was removed
    /// </returns>
    public: bool RemoveMigration(std::size_t schemaVersion);

    /// <summary>Connection the repository will work through</summary>
    private: const std::shared_ptr<Connections::Connection> &connection;
    /// <summary>Name to use for the migrations table</summary>
    private: const std::u8string &tableName;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations::Repositories

#endif // NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_MIGRATIONRECORDREPOSITORY_H
