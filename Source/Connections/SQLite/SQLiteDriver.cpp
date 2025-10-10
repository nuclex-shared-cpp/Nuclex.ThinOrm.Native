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

#include <Nuclex/Support/Text/StringMatcher.h> // for StringMatcher

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

  /// <summary>Controls how pages are cached for a database</summary>
  const std::u8string_view cacheOptionName(u8"Cache", 5);
  /// <summary>Indicates that the database should be a private in-memory database</summary>
  const std::u8string_view privateOptionValue(u8"Private", 7);
  /// <summary>Indicates that the database should be a shareable in-memory database</summary>
  const std::u8string_view sharedOptionValue(u8"Shared", 6);

  /// <summary>Whether to open a database in memory only</summary>
  const std::u8string_view inMemoryOptionName(u8"InMemory", 8);

  /// <summary>Whether the opened database file can be a symlink</summary>
  const std::u8string_view followSymLinksOptionName(u8"FollowSymLinks", 14);

  /// <summary>How SQLite should handle multi-threaded scenarios</summary>
  const std::u8string_view mutexOptionName(u8"ConnectionMutex", 15);

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Checks whether the specified value, when interpreted as a boolean, indicates
  ///   that an option should be enabled
  /// </summary>
  /// <param name="optionValue">Value the user has assigned to the option</param>
  /// <param name="valueOnMissing">Value to assume if the option was not set</param>
  /// <param name="valueOnEmpty">Value to assume if the option value is empty</param>
  /// <returns>True if the value indicates that the option should be enabled</returns>
  inline bool interpretBooleanOption(
    const std::optional<std::u8string> &optionValue,
    bool valueOnMissing = false,
    bool valueOnEmpty = true
  ) {
    if(optionValue.has_value()) {
      if(optionValue.value().empty()) {
        return valueOnEmpty;
      } else {
        return Nuclex::ThinOrm::Value::BooleanFromString(optionValue.value());
      }
    } else {
      return valueOnMissing
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
    using Nuclex::Support::Text::StringMatcher;
    constexpr const bool CaseSensitive = false;

    int flags = SQLITE_OPEN_EXRESCODE;

    // The read-only option forces read-only mode by its mere presence
    bool shouldOpenReadOnly = interpretBooleanOption(
      connectionProperties.GetOption(std::u8string(readOnlyOptionName))
    );
    if(shouldOpenReadOnly) {
      flags |= SQLITE_OPEN_READONLY;
    } else {
      flags |= SQLITE_OPEN_READWRITE;
    }

    // The "allow create new" option defaults to true if missing (opposite of SQLite,
    // which requires the flag to create a new database).
    bool shouldCreateNewDatabase = interpretBooleanOption(
      connectionProperties.GetOption(std::u8string(allowCreateNewOptionName)), true
    );
    if(shouldCreateNewDatabase) {
      flags |= SQLITE_OPEN_CREATE;
    }

    // The "in memory" option can be used to create databases that exist only in memory
    // and are not backed by a file on disk.
    bool shouldStoreDatabaseInMemory = interpretBooleanOption(
      connectionProperties.GetOption(std::u8string(inMemoryOptionName))
    );
    if(shouldStoreDatabaseInMemory) {
      flags |= SQLITE_OPEN_MEMORY;
    }

    // The cache option controls the page cache. For an in-memory database that can be
    // opened by multiple connections, a shared page cache must be used.
    std::optional<std::u8string> cacheOptionValue = (
      connectionProperties.GetOption(std::u8string(cacheOptionName))
    );
    if(cacheOptionValue.has_value()) {
      bool isPrivate = StringMatcher::AreEqual<CaseSensitive>(
        cacheOptionValue.value(), privateOptionValue
      );
      if(isPrivate) {
        flags |= SQLITE_OPEN_MEMORY | SQLITE_OPEN_PRIVATECACHE;
      }

      bool isShared = StringMatcher::AreEqual<CaseSensitive>(
        cacheOptionValue.value(), sharedOptionValue
      );
      if(isShared) {
        flags |= SQLITE_OPEN_MEMORY | SQLITE_OPEN_SHAREDCACHE;
      }
    }

    // The allow create new option defaults to true if missing (opposite of SQLite,
    // which requires the flag to create a new database)
    bool shouldFollowSymLinks = interpretBooleanOption(
      connectionProperties.GetOption(std::u8string(followSymLinksOptionName)), true
    );
    if(!shouldFollowSymLinks) {
      flags |= SQLITE_OPEN_NOFOLLOW;
    }

    // The mutex option tells SQLite whether to use a mutex in each connection,
    // allowing connections to be accessed from multple threads. Otherwise, each
    // connection must be exlusively used by one thread only.
    std::optional<std::u8string> mutexOptionValue = (
      connectionProperties.GetOption(std::u8string(mutexOptionName))
    );
    if(mutexOptionValue.has_value()) {
      if(Value::BooleanFromString(mutexOptionValue.value())) {
        flags |= SQLITE_OPEN_FULLMUTEX;
      } else {
        flags |= SQLITE_OPEN_NOMUTEX;
      }
    }

    // Now establish a connection. Due to how connection properties work, especially those
    // that are parsed from JDBC-style connection URLs, a complex path would be split into
    // a directory stored in "hostname or path" and a "database name" (which here is identical
    // to the file name). If both are present we need to concatenate them like a path.
    std::shared_ptr<::sqlite3> connection;
    {
      std::u8string hostnameOrPath = connectionProperties.GetHostnameOrPath();
      std::optional<std::u8string> databaseName = connectionProperties.GetDatabaseName();

      if(databaseName.has_value()) {
        if(hostnameOrPath.empty()) {
          connection = Platform::SQLite3Api::Open(databaseName.value(), flags);
        } else { // ^^ only database name present ^^ // vv path and name present vv
          std::filesystem::path path = hostnameOrPath;
          connection = Platform::SQLite3Api::Open(path / databaseName.value(), flags);
        }
      } else { // ^^ database name present ^^ // vv database name absent vv
        connection = Platform::SQLite3Api::Open(hostnameOrPath, flags);
      }
    }

    (void)connectionProperties;
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::SQLite

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
