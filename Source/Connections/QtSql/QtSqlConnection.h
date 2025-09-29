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

#ifndef NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLCONNECTION_H
#define NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLCONNECTION_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Connections/Connection.h"

#include "./UniqueNameGenerator.h" // for UniqueNameGenerator

#include <QSqlDatabase> // for QSqlDatabase
#include <memory> // for std::shared_ptr
#include <optional> // for std::optional

namespace Nuclex::ThinOrm::Configuration {
  class ConnectionProperties;
}

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Database connection using Qt's SQL module</summary>
  class QtSqlConnection : public Connection {

    /// <summary>Initializes a new QtSql-based database connection</summary>
    /// <param name="databaseName">
    ///   Name of the database, can be user-defined or use the connection target as name
    /// </param>
    /// <param name="uniqueId">
    ///   Unique connection id provided by the unique name generator
    /// </param>
    /// <param name="database">Database connection holder from Qt SQL</param>
    public: QtSqlConnection(
      std::u8string databaseName, std::uint64_t uniqueId, QSqlDatabase database
    );
    /// <summary>Frees all resources owned by the database connection</summary>
    public: ~QtSqlConnection() override;

    /// <summary>Establishes a new connection to a database via QtSql</summary>
    /// <param name="properties">Settings to use for connection to the database</param>
    /// <param name="connectionBaseName">
    ///   Base name to use for the connection name passed to QSqlDatabase::addDatabase(),
    ///   will be made completely unique by appending a generated unique id
    /// </param>
    /// <param name="databaseName">
    ///   Database name that should be passed to QSqlDatabase::setDatabaseName(),
    ///   contains either the name of the database on the server or a file path,
    ///   an empty value means the setter method will not be called.
    /// </param>
    /// <returns>A new database connection estblashed with the provided parameters</returns>
    public: static std::shared_ptr<QtSqlConnection> Connect(
      const Configuration::ConnectionProperties &properties,
      const std::u8string connectionBaseName,
      const std::optional<std::u8string> databaseName
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

    /// <summary>Executes an SQL query that has result rows on the database</summary>
    /// <param name="rowQuery">Query that will be executed</param>
    /// <returns>A reader that can be used to fetch individual rows</returns>
    public: RowReader RunRowQuery(const Query &rowQuery) override;

    /// <summary>Checks if the specified table exists</summary>
    /// <param name="tableName">Table or view whose existence will be checked</param>
    /// <returns>True if a table or view with the given exists</returns>
    public: bool DoesTableOrViewExist(const std::u8string &tableName) override;

    /// <summary>Applies the connection properties to the Qt database</summary>
    /// <param name="database">Qt database instance that will be configured</param>
    /// <param name="properties">Connection properties that will be applied</param>
    /// <param name="databaseName">
    ///   Connection name to use instead of a purely auto-generated one in order to
    ///   aid in debugging when needed.
    /// </param>
    private: static void configureQSqlDatabase(
      QSqlDatabase &database,
      const Configuration::ConnectionProperties &properties,
      const std::optional<std::u8string> &databaseName
    );

    /// <summary>Generates unique numbers for each database name</summary>
    private: static UniqueNameGenerator uniqueNameGenerator;

    /// <summary>Name of the database being accessed, important for unique name</summary>
    private: std::u8string connectionBaseName;
    /// <summary>Unique id assigned to the instance, important for unique name</summary>
    private: std::uint64_t uniqueId;
    /// <summary>Opened database connection from the QtSql module</summary>
    private: QSqlDatabase database;
    /// <summary>Unique connection name the connection is registered under to Qt</summary>
    private: QString uniqueConnectionName;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLCONNECTION_H

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
