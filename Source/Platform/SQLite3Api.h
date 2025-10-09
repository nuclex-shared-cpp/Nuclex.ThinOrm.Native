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

#ifndef NUCLEX_THINORM_PLATFORM_SQLITE3API_H
#define NUCLEX_THINORM_PLATFORM_SQLITE3API_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include <filesystem> // for std::filesystem::path

#include <sqlite3.h> // for the SQLite3 API methods

namespace Nuclex::ThinOrm::Platform {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Wrapper methods that automate error checking for the SQLite3 API</summary>
  class SQLite3Api {

    /// <summary>Opens an SQLite database from a plain path</summary>
    /// <param name="path">Path of the database file</param>
    /// <parm name="flags">Flags that control how the database will be opened</param>
    public: static std::shared_ptr<::sqlite3> Open(
      const std::filesystem::path &path,
      int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
    );

    /// <summary>Opens an in-memory SQLite database or a from a plain path</summary>
    /// <param name="path">Path of the database file</param>
    /// <parm name="flags">Flags that control how the database will be opened</param>
    public: static std::shared_ptr<::sqlite3> Open(
      const std::u8string &uriOrName,
      int flags = SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE
    );

    public: [[noreturn]] void ThrowExceptionForExtendedResultCode(int errorCode);

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Platform

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)

#endif // NUCLEX_THINORM_PLATFORM_SQLITE3API_H
