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

#include <vector> // for std::vector<>
#include <string> // for std::u8string
#include <cstdint> // for std::uint16_t
#include <optional> // for std::optional<>

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Accesses the settings of a database connection</summary>
  /// <remarks>
  ///   <para>
  ///     This is an interface through which the settings for a database connection can be
  ///     retrieved. Usually, you'd store your database connection settings as a string using
  ///     either the <see cref="ConnectionUrl" /> or <see cref="ConnectionString" /> container
  ///     for easy storage and passing around.
  ///   </para>
  ///   <para>
  ///     To estbalish a database connection, a reference to this interface is simply passed
  ///     into the connection manager. It will look up the driver (which has to be registered
  ///     beforehand) and the driver will then pick the settings it knows and requires by
  ///     calling the virtual getter methods in this interface.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE ConnectionProperties {

    /// <summary>Frees all resources owned by the connection property container</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionProperties() = default;

    /// <summary>Retrieves the driver to use to access the database</summary>
    /// <returns>The driver by which the database should be accessed</returns>
    /// <remarks>
    ///   <para>
    ///     The driver is the name by which the database connector is registered to this
    ///     library. Drivers have a short name (i.e. 'mysql' or 'postgres') that can be
    ///     used as well as a display name (i.e. 'MySQL Connector' or 'libpqxx') that
    ///     can be used alternatively to unambiguously select a driver.
    ///   </para>
    ///   <para>
    ///     The driver decides over the interpretation of all other connection settings,
    ///     so some drivers may not use certain attributes (i.e. sqlite will not use
    ///     a port number due to being a file-based database) and the valid option names
    ///     can be entirely specific to each database driver.
    ///   </para>
    /// </remarks>
    public: virtual std::u8string GetDriver() const = 0;

    /// <summary>Retrieves the hostname of the database server</summary>
    /// <returns>The database server's hostname or the filename for the database</returns>
    /// <remarks>
    ///   This should simply contain the IP address or host name of the database server,
    ///   or, for embedded databases such as SQLite, the directory of the database file
    ///   (in which case, the database name contains the filename for consistency reasons,
    ///   as in single-file databases, the file is identical to the database and the final
    ///   URL slug element is always the database name for consistency)
    /// </remarks>
    public: virtual std::u8string GetHostnameOrPath() const = 0;

    /// <summary>Retrieves the TCP port to connect to the database server on</summary>
    /// <returns>The TCP port number to which connection attempts should be directed</returns>
    /// <remarks>
    ///   If the port is not specified, the database driver decides. For standalone database
    ///   servers, this would mean the standard port used by the database product. Single-file
    ///   embedded databases will not use a port at all.
    /// </remarks>
    public: virtual std::optional<std::uint16_t> GetPort() const = 0;

    /// <summary>Retrieves the name of the user to identify as when connecting</summary>
    /// <returns>The user name by which to connect to the database server</returns>
    public: virtual std::optional<std::u8string> GetUser() const = 0;

    /// <summary>Retrieves the password to use when connecting to the database</summary>
    /// <returns>The password to use when connecting to the database</returns>
    public: virtual std::optional<std::u8string> GetPassword() const = 0;

    /// <summary>
    ///   Retrieves the name of the database that should be opened upon connecting
    /// </summary>
    /// <returns>The name of the database to open after connecting</returns>
    public: virtual std::optional<std::u8string> GetDatabaseName() const = 0;

    /// <summary>Retrieves the value of an arbitrary option</summary>
    /// <param name="name">Name of the option whose value will be retrieved</param>
    /// <returns>The value of the option or nothing if the option wasn't set</returns>
    public: virtual std::optional<std::u8string> GetOption(
      const std::u8string &name
    ) const = 0;

    /// <summary>Lists all option names that have been set</summary>
    /// <returns>A vector containing the names of all options that have been set</returns>
    public: virtual std::vector<std::u8string> ListOptions() const = 0;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONPROPERTIES_H
