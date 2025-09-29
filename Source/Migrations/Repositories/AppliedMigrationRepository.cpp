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

#include "./AppliedMigrationRepository.h"

#include "Nuclex/ThinOrm/Query.h" // for Query
#include "Nuclex/ThinOrm/Connections/Connection.h" // for Connection

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Opening portion of the CREATE TABLE statement before the table name</summary>
  const std::u8string CreateTableOpener(
    u8"CREATE TABLE ", 13
  );

  /// <summary>Closing portion of the CREATE TABLE statement after the table name</summary>
  const std::u8string CreateTableCloser(
    u8" (\n"
    u8"  SchemaVersion BIGINT NOT NULL,\n"
    u8"  AppliedOn     TIMESTAMP NOT NULL,\n"
    u8"  Name          VARCHAR(255) NULL\n"
    u8");",
    108
  );

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Migrations::Repositories {

  // ------------------------------------------------------------------------------------------- //

  void AppliedMigrationRepository::CreateMigrationsTable(
    const std::shared_ptr<Connections::Connection> &connection,
    const std::u8string &tableName
  ) {
    std::u8string createTableStatement(CreateTableOpener);
    createTableStatement.append(tableName);
    createTableStatement.append(CreateTableCloser);

    Query createTableQuery(createTableStatement);
    connection->RunStatement(createTableQuery);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations::Repositories
