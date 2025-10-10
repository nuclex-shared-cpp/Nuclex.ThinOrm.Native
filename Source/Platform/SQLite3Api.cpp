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

#include <Nuclex/Support/Text/LexicalAppend.h> // for std::lexical_append<>

#include <stdexcept> // for std::runtime_error
#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Converts an SQLite result code into a human-readable error message</summary>
  /// <param name="resultCode">Result code that will be converted</param>
  /// <returns>A string that describes the error indicated by the result code</returns>
  std::u8string errorMessageFromResultCode(int resultCode) {
    switch(resultCode) {
      case SQLITE_ERROR: {
        return std::u8string(u8"Generic error", 13);
      }
      case SQLITE_INTERNAL: {
        return std::u8string(u8"Internal logic error in SQLite", 30);
      }
      case SQLITE_PERM: {
        return std::u8string(u8"Access permission denied", 24);
      }
      case SQLITE_ABORT: {
        return std::u8string(u8"Callback routine requested an abort", 35);
      }
      case SQLITE_BUSY: {
        return std::u8string(u8"The database file is locked", 27);
      }
      case SQLITE_LOCKED: {
        return std::u8string(u8"A table in the database is locked", 33);
      }
      case SQLITE_NOMEM: {
        return std::u8string(u8"A malloc() failed", 17);
      }
      case SQLITE_READONLY: {
        return std::u8string(u8"Attempt to write a readonly database", 36);
      }
      case SQLITE_INTERRUPT: {
        return std::u8string(u8"Operation terminated by sqlite3_interrupt()", 43);
      }
      case SQLITE_IOERR: {
        return std::u8string(u8"Some kind of disk I/O error occurred", 36);
      }
      case SQLITE_CORRUPT: {
        return std::u8string(u8"The database disk image is malformed", 36);
      }
      case SQLITE_NOTFOUND: {
        return std::u8string(u8"Unknown opcode in sqlite3_file_control()", 40);
      }
      case SQLITE_FULL: {
        return std::u8string(u8"Insertion failed because database is full", 41);
      }
      case SQLITE_CANTOPEN: {
        return std::u8string(u8"Unable to open the database file", 32);
      }
      case SQLITE_PROTOCOL: {
        return std::u8string(u8"Database lock protocol error", 28);
      }
      case SQLITE_EMPTY: {
        return std::u8string(u8"Internal use only", 17);
      }
      case SQLITE_SCHEMA: {
        return std::u8string(u8"The database schema changed", 27);
      }
      case SQLITE_TOOBIG: {
        return std::u8string(u8"String or BLOB exceeds size limit", 33);
      }
      case SQLITE_CONSTRAINT: {
        return std::u8string(u8"Abort due to constraint violation", 33);
      }
      case SQLITE_MISMATCH: {
        return std::u8string(u8"Data type mismatch", 18);
      }
      case SQLITE_MISUSE: {
        return std::u8string(u8"Library used incorrectly", 24);
      }
      case SQLITE_NOLFS: {
        return std::u8string(u8"Uses OS features not supported on host", 38);
      }
      case SQLITE_AUTH: {
        return std::u8string(u8"Authorization denied", 20);
      }
      case SQLITE_FORMAT: {
        return std::u8string(u8"Not used", 8);
      }
      case SQLITE_RANGE: {
        return std::u8string(u8"2nd parameter to sqlite3_bind out of range", 42);
      }
      case SQLITE_NOTADB: {
        return std::u8string(u8"File opened that is not a database file", 39);
      }
      case SQLITE_NOTICE: {
        return std::u8string(u8"Notifications from sqlite3_log()", 32);
      }
      case SQLITE_WARNING: {
        return std::u8string(u8"Warnings from sqlite3_log()", 27);
      }
      case SQLITE_ROW: {
        return std::u8string(u8"sqlite3_step() has another row ready", 36);
      }
      case SQLITE_DONE: {
        return std::u8string(u8"sqlite3_step() has finished executing", 37);
      }
      default: {
        std::u8string message(u8"SQLite returned an unknown result code:", 39);
        Nuclex::Support::Text::lexical_append(message, resultCode);
        return message;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Converts an SQLite extended result code into a human-readable error message
  /// </summary>
  /// <param name="extendedResultCode">Result code that will be converted</param>
  /// <returns>A string that describes the error indicated by the result code</returns>
  std::u8string errorMessageFromExtendedResultCode(int extendedResultCode) {
    // TODO: Read the SQLite documentation and write short descriptive error messages
    //   https://sqlite.org/rescode.html
    switch(extendedResultCode) {
      case SQLITE_ERROR_MISSING_COLLSEQ: {
        return std::u8string(u8"SQLITE_ERROR_MISSING_COLLSEQ");
      }
      case SQLITE_ERROR_RETRY: {
        return std::u8string(u8"SQLITE_ERROR_RETRY");
      }
      case SQLITE_ERROR_SNAPSHOT: {
        return std::u8string(u8"SQLITE_ERROR_SNAPSHOT");
      }
      case SQLITE_IOERR_READ: {
        return std::u8string(u8"SQLITE_IOERR_READ");
      }
      case SQLITE_IOERR_SHORT_READ: {
        return std::u8string(u8"SQLITE_IOERR_SHORT_READ");
      }
      case SQLITE_IOERR_WRITE: {
        return std::u8string(u8"SQLITE_IOERR_WRITE");
      }
      case SQLITE_IOERR_FSYNC: {
        return std::u8string(u8"SQLITE_IOERR_FSYNC");
      }
      case SQLITE_IOERR_DIR_FSYNC: {
        return std::u8string(u8"SQLITE_IOERR_DIR_FSYNC");
      }
      case SQLITE_IOERR_TRUNCATE: {
        return std::u8string(u8"SQLITE_IOERR_TRUNCATE");
      }
      case SQLITE_IOERR_FSTAT: {
        return std::u8string(u8"SQLITE_IOERR_FSTAT");
      }
      case SQLITE_IOERR_UNLOCK: {
        return std::u8string(u8"SQLITE_IOERR_UNLOCK");
      }
      case SQLITE_IOERR_RDLOCK: {
        return std::u8string(u8"SQLITE_IOERR_RDLOCK");
      }
      case SQLITE_IOERR_DELETE: {
        return std::u8string(u8"SQLITE_IOERR_DELETE");
      }
      case SQLITE_IOERR_BLOCKED: {
        return std::u8string(u8"SQLITE_IOERR_BLOCKED");
      }
      case SQLITE_IOERR_NOMEM: {
        return std::u8string(u8"SQLITE_IOERR_NOMEM");
      }
      case SQLITE_IOERR_ACCESS: {
        return std::u8string(u8"SQLITE_IOERR_ACCESS");
      }
      case SQLITE_IOERR_CHECKRESERVEDLOCK: {
        return std::u8string(u8"SQLITE_IOERR_CHECKRESERVEDLOCK");
      }
      case SQLITE_IOERR_LOCK: {
        return std::u8string(u8"SQLITE_IOERR_LOCK");
      }
      case SQLITE_IOERR_CLOSE: {
        return std::u8string(u8"SQLITE_IOERR_CLOSE");
      }
      case SQLITE_IOERR_DIR_CLOSE: {
        return std::u8string(u8"SQLITE_IOERR_DIR_CLOSE");
      }
      case SQLITE_IOERR_SHMOPEN: {
        return std::u8string(u8"SQLITE_IOERR_SHMOPEN");
      }
      case SQLITE_IOERR_SHMSIZE: {
        return std::u8string(u8"SQLITE_IOERR_SHMSIZE");
      }
      case SQLITE_IOERR_SHMLOCK: {
        return std::u8string(u8"SQLITE_IOERR_SHMLOCK");
      }
      case SQLITE_IOERR_SHMMAP: {
        return std::u8string(u8"SQLITE_IOERR_SHMMAP");
      }
      case SQLITE_IOERR_SEEK: {
        return std::u8string(u8"SQLITE_IOERR_SEEK");
      }
      case SQLITE_IOERR_DELETE_NOENT: {
        return std::u8string(u8"SQLITE_IOERR_DELETE_NOENT");
      }
      case SQLITE_IOERR_MMAP: {
        return std::u8string(u8"SQLITE_IOERR_MMAP");
      }
      case SQLITE_IOERR_GETTEMPPATH: {
        return std::u8string(u8"SQLITE_IOERR_GETTEMPPATH");
      }
      case SQLITE_IOERR_CONVPATH: {
        return std::u8string(u8"SQLITE_IOERR_CONVPATH");
      }
      case SQLITE_IOERR_VNODE: {
        return std::u8string(u8"SQLITE_IOERR_VNODE");
      }
      case SQLITE_IOERR_AUTH: {
        return std::u8string(u8"SQLITE_IOERR_AUTH");
      }
      case SQLITE_IOERR_BEGIN_ATOMIC: {
        return std::u8string(u8"SQLITE_IOERR_BEGIN_ATOMIC");
      }
      case SQLITE_IOERR_COMMIT_ATOMIC: {
        return std::u8string(u8"SQLITE_IOERR_COMMIT_ATOMIC");
      }
      case SQLITE_IOERR_ROLLBACK_ATOMIC: {
        return std::u8string(u8"SQLITE_IOERR_ROLLBACK_ATOMIC");
      }
      case SQLITE_IOERR_DATA: {
        return std::u8string(u8"SQLITE_IOERR_DATA");
      }
      case SQLITE_IOERR_CORRUPTFS: {
        return std::u8string(u8"SQLITE_IOERR_CORRUPTFS");
      }
      case SQLITE_IOERR_IN_PAGE: {
        return std::u8string(u8"SQLITE_IOERR_IN_PAGE");
      }
      case SQLITE_LOCKED_SHAREDCACHE: {
        return std::u8string(u8"SQLITE_LOCKED_SHAREDCACHE");
      }
      case SQLITE_LOCKED_VTAB: {
        return std::u8string(u8"SQLITE_LOCKED_VTAB");
      }
      case SQLITE_BUSY_RECOVERY: {
        return std::u8string(u8"SQLITE_BUSY_RECOVERY");
      }
      case SQLITE_BUSY_SNAPSHOT: {
        return std::u8string(u8"SQLITE_BUSY_SNAPSHOT");
      }
      case SQLITE_BUSY_TIMEOUT: {
        return std::u8string(u8"SQLITE_BUSY_TIMEOUT");
      }
      case SQLITE_CANTOPEN_NOTEMPDIR: {
        return std::u8string(u8"SQLITE_CANTOPEN_NOTEMPDIR");
      }
      case SQLITE_CANTOPEN_ISDIR: {
        return std::u8string(u8"SQLITE_CANTOPEN_ISDIR");
      }
      case SQLITE_CANTOPEN_FULLPATH: {
        return std::u8string(u8"SQLITE_CANTOPEN_FULLPATH");
      }
      case SQLITE_CANTOPEN_CONVPATH: {
        return std::u8string(u8"SQLITE_CANTOPEN_CONVPATH");
      }
      case SQLITE_CANTOPEN_DIRTYWAL: {
        return std::u8string(u8"SQLITE_CANTOPEN_DIRTYWAL");
      }
      case SQLITE_CANTOPEN_SYMLINK: {
        return std::u8string(u8"SQLITE_CANTOPEN_SYMLINK");
      }
      case SQLITE_CORRUPT_VTAB: {
        return std::u8string(u8"SQLITE_CORRUPT_VTAB");
      }
      case SQLITE_CORRUPT_SEQUENCE: {
        return std::u8string(u8"SQLITE_CORRUPT_SEQUENCE");
      }
      case SQLITE_CORRUPT_INDEX: {
        return std::u8string(u8"SQLITE_CORRUPT_INDEX");
      }
      case SQLITE_READONLY_RECOVERY: {
        return std::u8string(u8"SQLITE_READONLY_RECOVERY");
      }
      case SQLITE_READONLY_CANTLOCK: {
        return std::u8string(u8"SQLITE_READONLY_CANTLOCK");
      }
      case SQLITE_READONLY_ROLLBACK: {
        return std::u8string(u8"SQLITE_READONLY_ROLLBACK");
      }
      case SQLITE_READONLY_DBMOVED: {
        return std::u8string(u8"SQLITE_READONLY_DBMOVED");
      }
      case SQLITE_READONLY_CANTINIT: {
        return std::u8string(u8"SQLITE_READONLY_CANTINIT");
      }
      case SQLITE_READONLY_DIRECTORY: {
        return std::u8string(u8"SQLITE_READONLY_DIRECTORY");
      }
      case SQLITE_ABORT_ROLLBACK: {
        return std::u8string(u8"SQLITE_ABORT_ROLLBACK");
      }
      case SQLITE_CONSTRAINT_CHECK: {
        return std::u8string(u8"SQLITE_CONSTRAINT_CHECK");
      }
      case SQLITE_CONSTRAINT_COMMITHOOK: {
        return std::u8string(u8"SQLITE_CONSTRAINT_COMMITHOOK");
      }
      case SQLITE_CONSTRAINT_FOREIGNKEY: {
        return std::u8string(u8"SQLITE_CONSTRAINT_FOREIGNKEY");
      }
      case SQLITE_CONSTRAINT_FUNCTION: {
        return std::u8string(u8"SQLITE_CONSTRAINT_FUNCTION");
      }
      case SQLITE_CONSTRAINT_NOTNULL: {
        return std::u8string(u8"SQLITE_CONSTRAINT_NOTNULL");
      }
      case SQLITE_CONSTRAINT_PRIMARYKEY: {
        return std::u8string(u8"SQLITE_CONSTRAINT_PRIMARYKEY");
      }
      case SQLITE_CONSTRAINT_TRIGGER: {
        return std::u8string(u8"SQLITE_CONSTRAINT_TRIGGER");
      }
      case SQLITE_CONSTRAINT_UNIQUE: {
        return std::u8string(u8"SQLITE_CONSTRAINT_UNIQUE");
      }
      case SQLITE_CONSTRAINT_VTAB: {
        return std::u8string(u8"SQLITE_CONSTRAINT_VTAB");
      }
      case SQLITE_CONSTRAINT_ROWID: {
        return std::u8string(u8"SQLITE_CONSTRAINT_ROWID");
      }
      case SQLITE_CONSTRAINT_PINNED: {
        return std::u8string(u8"SQLITE_CONSTRAINT_PINNED");
      }
      case SQLITE_CONSTRAINT_DATATYPE: {
        return std::u8string(u8"SQLITE_CONSTRAINT_DATATYPE");
      }
      case SQLITE_NOTICE_RECOVER_WAL: {
        return std::u8string(u8"SQLITE_NOTICE_RECOVER_WAL");
      }
      case SQLITE_NOTICE_RECOVER_ROLLBACK: {
        return std::u8string(u8"SQLITE_NOTICE_RECOVER_ROLLBACK");
      }
      case SQLITE_NOTICE_RBU: {
        return std::u8string(u8"SQLITE_NOTICE_RBU");
      }
      case SQLITE_WARNING_AUTOINDEX: {
        return std::u8string(u8"SQLITE_WARNING_AUTOINDEX");
      }
      case SQLITE_AUTH_USER: {
        return std::u8string(u8"SQLITE_AUTH_USER");
      }
      case SQLITE_OK_LOAD_PERMANENTLY: {
        return std::u8string(u8"SQLITE_OK_LOAD_PERMANENTLY");
      }
      case SQLITE_OK_SYMLINK: {
        return std::u8string(u8"SQLITE_OK_SYMLINK");
      }
      default: {
        if(extendedResultCode == (extendedResultCode & 0xff)) {
          return errorMessageFromResultCode(extendedResultCode);
        } else {
          std::u8string message(u8"SQLite returned an unknown extended result code:", 48);
          Nuclex::Support::Text::lexical_append(message, extendedResultCode);
          return message;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Closes an SQLite3 database while ignoring any errors</summary>
  /// <param name="database">SQLite database that will be closed</param>
  void closeSQLite3Database(::sqlite3 *database) noexcept {
    if(database != nullptr) [[likely]] {
      //int resultCode = ::sqlite3_close_v2(database);
      int resultCode = ::sqlite3_close(database);
      NUCLEX_THINORM_NDEBUG_UNUSED(resultCode);
      assert(
        (resultCode == SQLITE_OK) &&
        u8"SQLite3 database was properly finalized and the library was able to close it"
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //
  
} // anonymous namespace

namespace Nuclex::ThinOrm::Platform {

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<::sqlite3> SQLite3Api::Open(
    const std::filesystem::path &path,
    int flags /* = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE */
   ) {
    ::sqlite3 *database = nullptr;

    assert(
      ((flags & SQLITE_OPEN_URI) == 0) &&
      u8"Path-based SQLite3Api::Open() must exclude the SQLITE_OPEN_URI flag"
    );

    flags |= SQLITE_OPEN_EXRESCODE;

    std::u8string utf8Path = path.u8string();
    int extendedResultCode = ::sqlite3_open_v2(
      reinterpret_cast<const char *>(utf8Path.c_str()),
      &database,
      flags,
      nullptr
    );
    if(extendedResultCode != SQLITE_OK) [[unlikely]] {
      ThrowExceptionForExtendedResultCode(extendedResultCode);
    }

    return std::shared_ptr<::sqlite3>(database, &closeSQLite3Database);
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<::sqlite3> SQLite3Api::Open(
    const std::filesystem::path &path,
    int flags /* = SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE */
   ) {
    ::sqlite3 *database = nullptr;

    assert(
      ((flags & SQLITE_OPEN_URI) != 0) &&
      u8"URI-based SQLite3Api::Open() must include the SQLITE_OPEN_URI flag"
    );

    flags |= SQLITE_OPEN_EXRESCODE;

    int extendedResultCode = ::sqlite3_open_v2(
      reinterpret_cast<const char *>(path.c_str()),
      &database,
      flags,
      nullptr
    );
    if(extendedResultCode != SQLITE_OK) [[unlikely]] {
      ThrowExceptionForExtendedResultCode(extendedResultCode);
    }

    return std::shared_ptr<::sqlite3>(database, &closeSQLite3Database);
  }

  // ------------------------------------------------------------------------------------------- //

  void SQLite3Api::ThrowExceptionForExtendedResultCode(int extendedResultCode) {
    std::u8string message = errorMessageFromExtendedResultCode(extendedResultCode);
    throw std::runtime_error(reinterpret_cast<const char *>(message.c_str()));
  }

} // namespace Nuclex::ThinOrm::Platform

#endif // defined(NUCLEX_THINORM_ENABLE_SQLITE)
