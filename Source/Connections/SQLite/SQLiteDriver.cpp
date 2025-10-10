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

#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h"
#include "Nuclex/ThinOrm/Configuration/KnownOptions.h"
#include "Nuclex/ThinOrm/Value.h"

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

  /// <summary>Controls whether the database will be opened in read-only mode</summary>
  const std::u8string_view readOnlyOptionName = (
    Nuclex::ThinOrm::Configuration::KnownOptions::ReadOnlyOptionName
  );

  /// <summary>Controls whether the database will be opened in read-only mode</summary>
  const std::u8string_view allowCreateNewOptionName = (
    Nuclex::ThinOrm::Configuration::KnownOptions::AllowCreateNewOptionName
  );

  /// <summary>Whether the opened database file can be a symlink</summary>
  const std::u8string_view followSymLinksOptionName(u8"FollowSymLinks", 14);

  // 1: not in memory (false)
  // 2: in memory and private (private) <-- or do this by leaving db name empty?
  // 3: in memory and shared (true)
  const std::u8string_view inMemoryOptionName(u8"InMemory", 8);
  //const std::u8string_view sharedCacheString(u8"SharedCache", 11);

  /// <summary>How SQLite should handle multi-threaded scenarios</summary>
  const std::u8string_view threadingModeOptionName(u8"ThreadingMode", 13);
  /// <summary>Indicates that different connections can run concurrent queries</summary>
  const std::u8string_view perConnectionOptionValue(u8"PerConnection", 13);
  /// <summary>Indicates that all queries should be executed in series</summary>
  const std::u8string_view globalOptionValue(u8"Global", 6);

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Checks whether the specified option, when interpreted as a boolean, indicates
  ///   that the option should be enabled
  /// </summary>
  /// <param name="connectionProperties">Setings that are storing the option</param>
  /// <param name="optionName">Name of the option that will be checked</param>
  /// <returns>True if the option's value indicates that it should be enabled</returns>
  bool isBooleanOptionSet(
    const Nuclex::ThinOrm::Configuration::ConnectionProperties &connectionProperties,
    const std::u8string_view &optionName
  ) {
    std::optional<std::u8string> value = connectionProperties.GetOption(
      std::u8string(optionName)
    );
    if(value.has_value()) {
      return Nuclex::ThinOrm::Value::BooleanFromString(value.value());
    } else {
      return false;
    }
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
    int flags = 0;

    // The read-only options forces read-onyl mode. This is a simple option that default
    // to false when not present, so we only need to check if it is turned on.
    bool shouldOpenReadOnly = isBooleanOptionSet(connectionProperties, readOnlyOptionName);
    if(shouldOpenReadOnly) {
      flags |= SQLITE_OPEN_READONLY;
    }

    // The allow create new option defaults to true (opposite of SQLite,
    // which requires the flag or will not create a new database)
    std::optional<std::u8string> createNewOptionValue = (
      connectionProperties.GetOption(std::u8string(allowCreateNewOptionName))
    );
    if(createNewOptionValue.has_value()) {
      bool allowCreateNew = Value::BooleanFromString(createNewOptionValue.value());
      if(allowCreateNew) {
        flags |= SQLITE_OPEN_CREATE; // Creation of new database file explicitly allowed
      }
    } else {
      flags |= SQLITE_OPEN_CREATE; // Default value allows creation
    }

    // SQLITE_OPEN_READONLY
    // SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
    // SQLITE_OPEN_URI
    // SQLITE_OPEN_MEMORY
    // SQLITE_OPEN_NOMUTEX
    // SQLITE_OPEN_FULLMUTEX
    // SQLITE_OPEN_SHAREDCACHE
    // SQLITE_OPEN_PRIVATECACHE
    // SQLITE_OPEN_EXRESCODE
    // SQLITE_OPEN_NOFOLLOW


    (void)connectionProperties;
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
