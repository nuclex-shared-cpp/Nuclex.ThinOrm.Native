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

#include "./SQLiteConnection.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h" // for ConnectionProperties
#include "Nuclex/ThinOrm/Value.h" // for Value
#include "Nuclex/ThinOrm/RowReader.h" // for RowReader

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //
  
} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::SQLite {

  // ------------------------------------------------------------------------------------------- //

  SQLiteConnection::SQLiteConnection(const std::filesystem::path &databasePath) :
    databasePath(databasePath) {}

  // ------------------------------------------------------------------------------------------- //

  SQLiteConnection::~SQLiteConnection() {
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<SQLiteConnection> SQLiteConnection::Connect(
    const std::filesystem::path& databasePath
  ) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void SQLiteConnection::Prepare(const Query &query) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void SQLiteConnection::RunStatement(const Query &statement) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  Value SQLiteConnection::RunScalarQuery(const Query &scalarQuery) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t SQLiteConnection::RunUpdateQuery(const Query &updateQuery) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::unique_ptr<RowReader> SQLiteConnection::RunRowQuery(const Query &rowQuery) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  bool SQLiteConnection::DoesTableOrViewExist(const std::u8string &tableName) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
