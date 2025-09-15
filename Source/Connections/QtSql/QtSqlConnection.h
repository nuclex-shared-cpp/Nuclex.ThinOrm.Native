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
  class ConnecitonProperties;
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
    protected: QtSqlConnection(
      std::u8string databaseName, std::uint64_t uniqueId, QSqlDatabase database
    );
    /// <summary>Frees all resources owned by the database connection</summary>
    public: ~QtSqlConnection() override;

#if 0
    /// <summary>Establishes a new connection to a database via QtSql</summary>
    /// <param name="properties">Settings to use for connection to the database</param>
    /// <returns>A new database connection estblashed with the provided parameters</returns>
    public: static std::shared_ptr<QtSqlConnection> Connect(
      const Configuration::ConnectionProperties &properties
    );
#endif

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
