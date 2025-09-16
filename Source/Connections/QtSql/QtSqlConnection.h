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
#include "Nuclex/ThinOrm/Connections/Connection.h"

#include "./UniqueNameGenerator.h" // for UniqueNameGenerator

#include <QSqlDatabase> // for QSqlDatabase
#include <memory> // for std::shared_ptr

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
    /// <returns>A new database connection estblashed with the provided parameters</returns>
    public: static std::shared_ptr<QtSqlConnection> Connect(
      const Configuration::ConnectionProperties &properties
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

    /// <summary>Throws an exception if the specified driver isn't available</summary>
    /// <param name="driver">Driver that will be checked for availability</param>
    private: static void requireQtSqlDriver(const std::u8string &driver);

    /// <summary>Forms a unique name for the database being accessed</summary>
    /// <param name="properties">Connection settings for the database</param>
    /// <returns>A unique name under which the database can be managed</returns>
    /// <remarks>
    ///   This name is not unique per connection but unique per connection settings
    ///   (with a &quot;decent chance&quot; guarantee, not an absolute guarantee),
    ///   so it needs to be combined with the <see cref="UniqueNameGenerator" /> to
    ///   create a really unique database name that can be registered in Qt (which sadly
    ///   manages open database connections globally and demands a unique name for each)
    /// </remarks>
    private: static std::u8string determineUniqueDatabaseName(
      const Configuration::ConnectionProperties &properties
    );

    /// <summary>Generates unique numbers for each database name</summary>
    private: static UniqueNameGenerator uniqueNameGenerator;

    /// <summary>Name of the database being accessed, important for unique name</summary>
    private: std::u8string databaseName;
    /// <summary>Unique id assigned to the instance, important for unique name</summary>
    private: std::uint64_t uniqueId;
    /// <summary>Opened database connection from the QtSql module</summary>
    private: QSqlDatabase database;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLCONNECTION_H
