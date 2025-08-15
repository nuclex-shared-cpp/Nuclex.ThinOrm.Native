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

#ifndef NUCLEX_THINORM_CONFIGURATION_CONNECTIONPROPERTIES_H
#define NUCLEX_THINORM_CONFIGURATION_CONNECTIONPROPERTIES_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <cstdint> // for std::uint16_t
#include <optional> // for std::optional<>

// NOTE: Test parser with IPv6!

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Stores connection settings as semicolon-delimited key/value pairs</summary>
  class NUCLEX_THINORM_TYPE ConnectionProperties {

    /// <summary>Frees all resources owned by the connection property container</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionProperties() = default;

    /// <summary>Retrieves the hostname of the database server</summary>
    /// <returns>The database server's hostname or the filename for the database</returns>
    /// <remarks>
    ///   This should simply contain the IP address or host name of the database server,
    ///   or, for embedded databases such as SQLite, the directory of the database file
    ///   (in which case, the database name contains the filename for consistency reasons,
    ///   as in single-file databases, the file is identical to the database and the final
    ///   URL slug element is always the database name for consistency)
    /// </remarks>
    public: virtual std::u8string GetHostnameOrFilename() const = 0;

    /// <summary>Sets the hostname of the database server to connect to</summary>
    /// <param name="hostnameOrFilename">
    ///   IP, hostname or single-file database path to use
    /// </param>
    public: virtual void SetHostnameOrFilename(
      const std::u8string &hostnameOrFilename
    ) = 0;

    /// <summary>Retrieves the TCP port to connect to the database server on</summary>
    /// <returns>The TCP port number to which connection attempts should be directed</returns>
    /// <remarks>
    ///   If the port is not specified, the database driver decides. For standalone database
    ///   servers, this would mean the standard port used by the database product. Single-file
    ///   embedded databases will not use a port at all.
    /// </remarks>
    public: virtual std::optional<std::uint16_t> GetPort() const = 0;

    /// <summary>Sets the TCP port on which the database server should be addressed</summary>
    /// <param name="port">TCP port number that should be used when connecting</param>
    public: virtual void SetPort(
      const std::optional<std::uint16_t> &port = std::optional<std::uint16_t>()
    ) = 0;

    /// <summary>Retrieves the name of the user to identify as when connecting</summary>
    /// <returns>The user name by which to connect to the database server</returns>
    public: virtual std::optional<std::u8string> GetUser() const = 0;

    /// <summary>Sets the name of the user to identify as when connecting</summary>
    /// <param name="user">User to identify as when connecting to the database server</param>
    public: virtual void SetUser(
      const std::optional<std::u8string> &user = std::optional<std::u8string>()
    ) = 0;

    /// <summary>Retrieves the password to use when connecting to the database</summary>
    /// <returns>The password to use when connecting to the database</returns>
    public: virtual std::optional<std::u8string> GetPassword() const = 0;

    /// <summary>Sets the password to use when connecting to the database</summary>
    /// <param name="password">Password to use when connecting to the database</param>
    public: virtual void SetPassword(
      const std::optional<std::u8string> &password = std::optional<std::u8string>()
    ) = 0;

    /// <summary>
    ///   Retrieves the name of the database that should be opened upon connecting
    /// </summary>
    /// <returns>The name of the database to open after connecting</returns>
    public: virtual std::optional<std::u8string> GetDatabaseName() const = 0;

    /// <summary>Sets the name of the database to open upon connecting</summary>
    /// <param name="databaseName">Database to open when connecting</param>
    public: virtual void SetDatabaseName(
      const std::optional<std::u8string> &databaseName = std::optional<std::u8string>()
    ) = 0;

    // Couldn't be distinguished from the URL path, so we'll simply treat everything before
    // the last slug in the URL as the server name.
    //
    // For SQLite and other embedded databases, that will mean the hostname or filename
    // property will contain the directory of the database file
    //
    // For SQL Server with optional instance names, that means the first URL slug becomes
    // the instance name, the second URL slug becomes the database (catalog) name.
    //
    // For MariaDB, PostgreSQL and others, there are no instances, only the hostname and 
    // the database name, which also would be correctly split by this approach.
    //
    // Firebird with local paths passed to the server should be able to "just work," too.
    //public: NUCLEX_THINORM_API virtual std::optional<std::u8string> GetInstanceName() const = 0;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONPROPERTIES_H
