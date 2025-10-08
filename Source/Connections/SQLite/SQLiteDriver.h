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

#ifndef NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITEDRIVER_H
#define NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITEDRIVER_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include "Nuclex/ThinOrm/Connections/Driver.h"

namespace Nuclex::ThinOrm::Connections::SQLite {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Opens SQLite databases on the local file system using the native SQLite driver
  /// </summary>
  class SQLiteDriver : public Driver {

    /// <summary>Initializes a new SQLite driver</summary>
    public: SQLiteDriver();
    /// <summary>Frees all resources owned by the database driver</summary>
    public: ~SQLiteDriver() override;

    /// <summary>Retrieves a human-readable name for the driver</summary>
    /// <returns>A string that describes the driver</returns>
    public: const std::u8string &GetName() const override;

    /// <summary>Establishes a connection to the specified database</summary>
    /// <param name="connectionProperties">
    ///   Settings that specify the hostname or path, protocol, login data and extra options.
    ///   How these settings are used is up to the implementation of the driver.
    /// </param>
    /// <returns>A new database connection</returns>
    public: std::shared_ptr<Connection> Connect(
      const Configuration::ConnectionProperties &connectionProperties
    ) override;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)

#endif // NUCLEX_THINORM_CONNECTIONS_SQLITE_SQLITEDRIVER_H
