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

#ifndef NUCLEX_THINORM_CONFIGURATION_CONNECTIONURL_H
#define NUCLEX_THINORM_CONFIGURATION_CONNECTIONURL_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Configuration/WritableConnectionProperties.h"
#include "Nuclex/Support/Text/StringMatcher.h" // for CaseInsensitiveUtf8Less

#include <map> // for std::map

// NOTE: Test parser with IPv6!

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Stores connection settings as an URL string with key/value options</summary>
  /// <remarks>
  ///   <para>
  ///     This resembles the connection strings from JDBC, the Java database connection
  ///     layer. The driver is specified as the protocol while the server and initial
  ///     database are represented in the server and appended URL path.
  ///   </para>
  ///   <para>
  ///     You can use this, or ADO.NET-style connection strings, to convert database
  ///     connection settings into simple strings that can easily be stored in confguration
  ///     files or be updated by the user.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE ConnectionUrl : public WritableConnectionProperties {

    /// <summary>Initializes a new connection URL with default parameters</summary>
    public: NUCLEX_THINORM_API ConnectionUrl();
    /// <summary>Initializes a new connection URL copying existing settings</summary>
    /// <param name="other">Existing settings that will be copied</param>
    public: NUCLEX_THINORM_API ConnectionUrl(const ConnectionProperties &other);
    /// <summary>Frees all resources owned by the connection URL</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionUrl() override = default;

    /// <summary>Parses a connection string from the specified set of properties</summary>
    /// <param name="properties">Connection string that will be parsed</param>
    /// <returns>
    ///   The parsed properties as an instance of this connection string wrapper class
    /// </returns>
    public: NUCLEX_THINORM_API static ConnectionUrl Parse(const std::u8string &url);

    /// <summary>Converts the connection URL back into a plain (url) string</summary>
    /// <returns>A string containing the connection properties in URL form</returns>
    public: NUCLEX_THINORM_API std::u8string ToString() const;

    //
    // ConnectionProperties interface
    //

    /// <summary>Retrieves the driver to use to access the database</summary>
    /// <returns>The driver by which the database should be accessed</returns>
    public: NUCLEX_THINORM_API std::u8string GetDriver() const override {
      return this->driver;
    }

    /// <summary>Sets the driver to use to access the database</summary>
    /// <param name="newDriver">The driver that should be used to access the database</param>
    public: NUCLEX_THINORM_API void SetDriver(const std::u8string &newDriver) override {
      this->driver = newDriver;
    }

    /// <summary>Retrieves the hostname of the database server</summary>
    /// <returns>The database server's hostname or the filename for the database</returns>
    /// <remarks>
    ///   This should simply contain the IP address or host name of the database server,
    ///   or, for embedded databases such as SQLite, the directory of the database file
    ///   (in which case, the database name contains the filename for consistency reasons,
    ///   as in single-file databases, the file is identical to the database and the final
    ///   URL slug element is always the database name for consistency)
    /// </remarks>
    public: NUCLEX_THINORM_API std::u8string GetHostnameOrPath() const override {
      return this->hostnameOrPath;
    }

    /// <summary>Sets the hostname of the database server to connect to</summary>
    /// <param name="newHostnameOrPath">
    ///   IP, hostname or single-file database path to use
    /// </param>
    public: NUCLEX_THINORM_API void SetHostnameOrPath(
      const std::u8string &newHostnameOrPath
    ) override {
      this->hostnameOrPath = newHostnameOrPath;
    }

    /// <summary>Retrieves the TCP port to connect to the database server on</summary>
    /// <returns>The TCP port number to which connection attempts should be directed</returns>
    /// <remarks>
    ///   If the port is not specified, the database driver decides. For standalone database
    ///   servers, this would mean the standard port used by the database product. Single-file
    ///   embedded databases will not use a port at all.
    /// </remarks>
    public: NUCLEX_THINORM_API std::optional<std::uint16_t> GetPort() const override {
      return this->port;
    }

    /// <summary>Sets the TCP port on which the database server should be addressed</summary>
    /// <param name="newPort">TCP port number that should be used when connecting</param>
    public: NUCLEX_THINORM_API void SetPort(
      const std::optional<std::uint16_t> &newPort = std::optional<std::uint16_t>()
    ) override {
      this->port = newPort;
    }

    /// <summary>Retrieves the name of the user to identify as when connecting</summary>
    /// <returns>The user name by which to connect to the database server</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetUser() const override {
      return this->user;
    }

    /// <summary>Sets the name of the user to identify as when connecting</summary>
    /// <param name="newUser">User to identify as when connecting to the database server</param>
    public: NUCLEX_THINORM_API void SetUser(
      const std::optional<std::u8string> &newUser = std::optional<std::u8string>()
    ) override {
      this->user = newUser;
    }

    /// <summary>Retrieves the password to use when connecting to the database</summary>
    /// <returns>The password to use when connecting to the database</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetPassword() const override {
      return this->password;
    }

    /// <summary>Sets the password to use when connecting to the database</summary>
    /// <param name="newPassword">Password to use when connecting to the database</param>
    public: NUCLEX_THINORM_API void SetPassword(
      const std::optional<std::u8string> &newPassword = std::optional<std::u8string>()
    ) override {
      this->password = newPassword;
    }

    /// <summary>
    ///   Retrieves the name of the database that should be opened upon connecting
    /// </summary>
    /// <returns>The name of the database to open after connecting</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetDatabaseName() const override {
      return this->databaseName;
    }

    /// <summary>Sets the name of the database to open upon connecting</summary>
    /// <param name="newDatabaseName">Database to open when connecting</param>
    public: NUCLEX_THINORM_API void SetDatabaseName(
      const std::optional<std::u8string> &newDatabaseName = std::optional<std::u8string>()
    ) override {
      this->databaseName = newDatabaseName;
    }

    /// <summary>Retrieves the value of an arbitrary option</summary>
    /// <param name="name">Name of the option whose value will be retrieved</param>
    /// <returns>The value of the option or nothing if the option wasn't set</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetOption(
      const std::u8string &name
    ) const override;

    /// <summary>Sets the value of an arbitrary option or unsets the option</summary>
    /// <parma name="name">Name of the option whose value will be changed</param>
    /// <param name="newValue">
    ///   Value that will be assigned to the option, or nothing to unset the option
    /// </param>
    public: NUCLEX_THINORM_API void SetOption(
      const std::u8string &name,
      const std::optional<std::u8string> &newValue = std::optional<std::u8string>()
    ) override;

    /// <summary>Lists all option names that have been set</summary>
    /// <returns>A vector containing the names of all options that have been set</returns>
    public: NUCLEX_THINORM_API std::vector<std::u8string> ListOptions() const override {
      std::vector<std::u8string> names;
      names.reserve(this->options.size());
      for(const std::pair<const std::u8string, std::u8string> &entry : this->options) {
        names.push_back(entry.first);
      }
      return names;
    }

    // The instance name (an unusual feature only present in SQL Server) is appended to
    // to the host name or ip with a slash in ADO.NET. This means it poorly interacts with
    // URL-formatted connection settings because it looks like a plain URL path. However,
    // if we accept that the first slug in an URL path with two slugs must be the instance
    // name, we can arrive at consistency:
    //
    // - For SQLite and other embedded databases, that will mean the hostname or filename
    // property will contain the directory of the database file
    //
    // - For SQL Server with optional instance names, that means the first URL slug becomes
    // the instance name, the second URL slug becomes the database (catalog) name.
    //
    // - For MariaDB, PostgreSQL and others, there are no instances, only the hostname and 
    // the database name, which also would be correctly split by this approach.
    //
    // - Firebird with local paths passed to the server should be able to "just work," too.
    //
    //public: NUCLEX_THINORM_API virtual std::optional<std::u8string> GetInstanceName() const;

    /// <summary>Maps key names to key values for the arbitrary options</summary>
    private: typedef std::map<
      std::u8string, std::u8string,
      Nuclex::Support::Text::CaseInsensitiveUtf8Less
    > OptionsMapType;

    /// <summary>Name of the driver by which connections will be made</summary>
    private: std::u8string driver;
    /// <summary>IP or hostname of the database server of single-file database path</summary>
    private: std::u8string hostnameOrPath;
    /// <summary>Port number to which connections sould be directed</summary>
    private: std::optional<std::uint16_t> port;
    /// <summary>Name of the user to identify as to the database server</summary>
    private: std::optional<std::u8string> user;
    /// <summary>Password for the user account on the database server</summary>
    private: std::optional<std::u8string> password;
    /// <summary>Name of the database that should initially be opened</summary>
    private: std::optional<std::u8string> databaseName;
    /// <summary>Additional, driver-specific options for the database connection</summary>
    private: OptionsMapType options;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONURL_H
