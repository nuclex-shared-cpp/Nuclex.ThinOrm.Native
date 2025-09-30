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

#include "./MigrationRecordRepository.h"

#include "Nuclex/ThinOrm/Query.h" // for Query
#include "Nuclex/ThinOrm/RowReader.h" // for RowReader
#include "Nuclex/ThinOrm/Connections/Connection.h" // for Connection

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Opening portion of the CREATE TABLE statement before the table name</summary>
  const std::u8string_view CreateTableOpener(
    u8"CREATE TABLE ",
    13
  );

  /// <summary>Closing portion of the CREATE TABLE statement after the table name</summary>
  const std::u8string_view CreateTableCloser(
    u8" (\n"
    u8"  SchemaVersion  BIGINT NOT NULL,\n"
    u8"  AppliedOn  TIMESTAMP NOT NULL,\n"
    u8"  Name  VARCHAR(255) NULL,\n"
    u8"  PRIMARY KEY (SchemaVersion)\n"
    u8");",
    129
  );

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Opening portion of the version fetch statement before the table name</summary>
  const std::u8string_view FetchMigrationSchemaVersionsOpener(
    u8"SELECT SchemaVersion\n"
    u8"FROM ",
    26
  );

  /// <summary>Closing portion of the version fetch statement after the table name</summary>
  const std::u8string_view FetchMigrationSchemaVersionsCloser(
    u8"",
    0
  );

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Opening portion of the insert statement before the table name</summary>
  const std::u8string_view InsertMigrationRecordOpener(
    u8"INSERT INTO ",
    12
  );

  /// <summary>Closing portion of the insert statement after the table name</summary>
  const std::u8string_view InsertMigrationRecordCloser(
    u8" (SchemaVersion, AppliedOn, Name)\n"
    u8"VALUES ({schemaVersionValue}, {appliedOnValue}, {nameValue})",
    94
  );

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Opening portion of the delete statement before the table name</summary>
  const std::u8string_view DeletehMigrationRecordOpener(
    u8"DELETE FROM ",
    12
  );

  /// <summary>Closing portion of the delete statement after the table name</summary>
  const std::u8string_view DeleteMigrationRecordCloser(
    u8"\n"
    u8"WHERE (SchemaVersion = {schemaVersionValue})",
    45
  );

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Migrations::Repositories {

  // ------------------------------------------------------------------------------------------- //

  MigrationRecordRepository::MigrationRecordRepository(
    const std::shared_ptr<Connections::Connection> &connection,
    const std::u8string &tableName
  ) :
    connection(connection),
    tableName(tableName) {}

  // ------------------------------------------------------------------------------------------- //

  void MigrationRecordRepository::CreateTable() {
    std::u8string createTableStatement(CreateTableOpener);
    createTableStatement.append(this->tableName);
    createTableStatement.append(CreateTableCloser);

    Query createTableQuery(createTableStatement);
    this->connection->RunStatement(createTableQuery);
  }

  // ------------------------------------------------------------------------------------------- //

  std::unordered_set<
    std::size_t
  > MigrationRecordRepository::FetchAllAppliedSchemaVersions() const {
    std::unordered_set<std::size_t> result;

    std::u8string fetchAllVersionsStatement(FetchMigrationSchemaVersionsOpener);
    fetchAllVersionsStatement.append(this->tableName);
    fetchAllVersionsStatement.append(FetchMigrationSchemaVersionsCloser);

    Query fetchAllVersionsQuery(fetchAllVersionsStatement);

    // Retrieve the recorded migrations from the row reader and stash than into
    // our set. This could be done in chunks and batches, but the table itself is
    // quite lean and we expect at most a few hundred rows.
    std::unique_ptr<RowReader> reader = this->connection->RunRowQuery(fetchAllVersionsQuery);
    while(reader->MoveToNext()) {
      result.emplace(
        static_cast<std::size_t>(reader->GetColumnValue(0).AsInt64().value())
      );
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRecordRepository::AddMigration(const Entities::MigrationRecord &newMigration) {
    std::u8string insertMigrationStatement(InsertMigrationRecordOpener);
    insertMigrationStatement.append(this->tableName);
    insertMigrationStatement.append(InsertMigrationRecordCloser);

    // CHECK: The Query instance could be cached.
    //   When rolling out an application to production, typically all migrations will be
    //   applied in a row, so there may be a tiny performance gain if the query remains
    //   pre-parsed with its parameters and maybe even its materialization can be reused.
    Query insertMigrationQuery(insertMigrationStatement);

    insertMigrationQuery.SetParameterValue(
      u8"schemaVersionValue", Value(static_cast<std::int64_t>(newMigration.SchemaVersion))
    );
    insertMigrationQuery.SetParameterValue(
      u8"appliedOnValue", Value(false) // Value(newMigration.AppliedOn))
    );
    insertMigrationQuery.SetParameterValue(
      u8"nameValue", Value(newMigration.Name)
    );

    this->connection->RunStatement(insertMigrationQuery);
  }

  // ------------------------------------------------------------------------------------------- //

  bool MigrationRecordRepository::RemoveMigration(std::size_t schemaVersion) {
    std::u8string deleteMigrationStatement(DeletehMigrationRecordOpener);
    deleteMigrationStatement.append(this->tableName);
    deleteMigrationStatement.append(DeleteMigrationRecordCloser);

    Query deleteMigrationQuery(deleteMigrationStatement);
    deleteMigrationQuery.SetParameterValue(
      u8"schemaVersionValue", Value(static_cast<std::int64_t>(schemaVersion))
    );

    std::size_t affectedRowCount = this->connection->RunUpdateQuery(deleteMigrationQuery);
    return (0 < affectedRowCount);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations::Repositories
