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

#include "./SQLite3Api.h"

#if defined(NUCLEX_THINORM_ENABLE_SQLITE)

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //
  
} // anonymous namespace

namespace Nuclex::ThinOrm::Platform {

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<::sqlite3> SQLite3Api::Open(
    const std::filesystem::path &path,
    int flags /* = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE */
   ) {
    ::sqlite3 *database = nullptr;

    std::u8string utf8Path = path.u8string();
    ::sqlite3_open_v2(
      reinterpret_cast<const char *>(utf8Path.c_str()),
      &database,
      flags,
      nullptr
    );

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
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Platform

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
