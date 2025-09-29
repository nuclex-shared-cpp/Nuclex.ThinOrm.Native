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

#ifndef NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_APPLIEDMIGRATIONREPOSITORY_H
#define NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_APPLIEDMIGRATIONREPOSITORY_H

#include "Nuclex/ThinOrm/Config.h"
#include "../Entities/AppliedMigration.h" // for AppliedMigration

#include <cstddef> // for std::size_t
#include <string> // for std::u8string
#include <optional> // for std::optional<>
#include <memory> // for std::shared_ptr<>

namespace Nuclex::ThinOrm::Connections {
  class Connection;
}

namespace Nuclex::ThinOrm::Migrations::Repositories {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Manages reading, inserting and updating informations about applied migrations
  ///   in a database
  /// </summary>
  class AppliedMigrationRepository {

    /// <summary>Creates the table in which applied migrations are recorded</summary>
    /// <param name="connection">Connection through which the table will be created</param>
    /// <param name="tableName">Name to give the migrations table</param>
    public: void CreateMigrationsTable(
      const std::shared_ptr<Connections::Connection> &connection,
      const std::u8string &tableName
    );

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations::Repositories

#endif // NUCLEX_THINORM_MIGRATIONS_REPOSITORIES_APPLIEDMIGRATIONREPOSITORY_H
