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

#include "./SQLiteDriver.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include "../../Platform/SQLite3Api.h" // for SQLite3Api

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Converts a char-based string into a UTF-8 string</summary>
  /// <param name="charString">Char-based string that will be converted</param>
  /// <returns>The equivalent UTF-8 string</returns>
  /// <remarks>
  ///   This simply assumes the char-based string already contains UTF-8 or plain
  ///   ASCII characters that are part of the UTF-8 range. It will not assume any
  ///   code pages or perform any character set conversions.
  /// </remarks>
  std::u8string u8stringFromChars(const std::string_view &charString) {
    return std::u8string(charString.begin(), charString.end());
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::SQLite {

  // ------------------------------------------------------------------------------------------- //

  SQLiteDriver::SQLiteDriver() {}

  // ------------------------------------------------------------------------------------------- //

  SQLiteDriver::~SQLiteDriver() = default;

  // ------------------------------------------------------------------------------------------- //

  const std::u8string &SQLiteDriver::GetName() const {
    static std::u8string driverName = (
      std::u8string(u8"SQLite ") +
      u8stringFromChars(SQLITE_VERSION) +
      std::u8string(u8" via built-in library")
    );

    return driverName;
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<Connection> SQLiteDriver::Connect(
    const Configuration::ConnectionProperties &connectionProperties
  ) {
    (void)connectionProperties;
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
