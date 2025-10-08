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

#ifndef NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITECONNECTION_H
#define NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITECONNECTION_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include "Nuclex/ThinOrm/Connections/Connection.h"

#include <filesystem> // for std::filesystem::path

namespace Nuclex::ThinOrm::Connections::SQLite {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Database connection that opens a local file using the native SQLite library
  /// </summary>
  class SQLiteConnection : public Connection {

    /// <summary>Initializes a new native SQLite database connection</summary>
    /// <param name="databasePath">
    ///   Path of the database file on the local file system
    /// </param>
    public: SQLiteConnection(const std::filesystem::path &databasePath);
    /// <summary>Frees all resources owned by the database connection</summary>
    public: ~SQLiteConnection() override;

    /// <summary>Establishes a new connection to a database via QtSql</summary>
    /// <param name="databasePath">
    ///   Path of the database file on the local file system
    /// </param>
    /// <returns>A new database connection estblashed with the provided parameters</returns>
    public: static std::shared_ptr<SQLiteConnection> Connect(
      const std::filesystem::path &databasePath
    );

    /// <summary>Prepares the specified query for execution</summary>
    /// <param name="query">Query that will be prepared for execution</param>
    public: void Prepare(const Query &query) override;

    /// <summary>Executes an SQL query that has no results on the database</summary>
    /// <param name="statement">Statement that will be executed</param>
    public: void RunStatement(const Query &statement) override;

    /// <summary>Executes an SQL query that has a single result on the database</summary>
    /// <param name="scalarQuery">Query that will be executed</param>
    /// <returns>The result of the query</returns>
    public: Value RunScalarQuery(const Query &scalarQuery) override;

    /// <summary>Executes an SQL query that updates (or deletes) rows in the database</summary>
    /// <param name="updateQuery">Query that will be executed</param>
    /// <returns>The number of affected rows</returns>
    public: std::size_t RunUpdateQuery(const Query &updateQuery) override;

    /// <summary>Executes an SQL query that has result rows on the database</summary>
    /// <param name="rowQuery">Query that will be executed</param>
    /// <returns>A reader that can be used to fetch individual rows</returns>
    public: std::unique_ptr<RowReader> RunRowQuery(const Query &rowQuery) override;

    /// <summary>Checks if the specified table exists</summary>
    /// <param name="tableName">Table or view whose existence will be checked</param>
    /// <returns>True if a table or view with the given exists</returns>
    public: bool DoesTableOrViewExist(const std::u8string &tableName) override;

    /// <summary>Path of the opened database on the local file system</summary>
    private: std::filesystem::path databasePath;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)

#endif // NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITECONNECTION_H
