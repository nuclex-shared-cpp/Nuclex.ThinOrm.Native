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

#ifndef NUCLEX_THINORM_CONNECTIONS_QTSQL_UNIQUENAMEGENERATOR_H
#define NUCLEX_THINORM_CONNECTIONS_QTSQL_UNIQUENAMEGENERATOR_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include <string> // for std::u8string
#include <unordered_map> // for std::unordered_map<>
#include <cstdint> // for std::uint64_t
#include <mutex> // for std::mutex

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Generates unique names per database</summary>
  class UniqueNameGenerator {

    /// <summary>Provides a unique name for the specified database</summary>
    /// <param name="databaseName">Database a unique name will be provided for</param>
    /// <returns>
    ///   A number that will be unique for the caller (when combined with the database name
    /// </return>
    public: std::uint64_t BorrowUniqueId(const std::u8string &databaseName);

    /// <summary>Returns the a unique number so it can be recycled</summary>
    /// <param name="databaseName">
    ///  Database for which the unique number will be returned
    /// </param>
    public: void ReturnUniqueId(const std::u8string &databaseName);

    /// <summary>Keeps track of the number of active connections and the next id</summary>
    private: struct DatabaseIdGenerator {
      /// <summary>Number of connections that are currently accessing the database</summary>
      public: std::size_t ConnectionCount;
      /// <summary>Next unique id that will be returned</summary>
      public: std::size_t NextUniqueId;
    };

    /// <summary>Map of strings to database unique number generators</summary>
    private: typedef std::unordered_map<std::u8string, DatabaseIdGenerator> StringIdMap;

    /// <summary>Must be held when modifying the state</summary>
    private: std::mutex stateMutex;
    /// <summary>Stores the next generated database id for each database</summary>
    private: StringIdMap nextDatabaseIds;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // #if defined(NUCLEX_THINORM_ENABLE_QT)

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_UNIQUENAMEGENERATOR_H
