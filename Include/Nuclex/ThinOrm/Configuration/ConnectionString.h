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

#ifndef NUCLEX_THINORM_CONFIGURATION_CONNECTIONSTRING_H
#define NUCLEX_THINORM_CONFIGURATION_CONNECTIONSTRING_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Configuration/WritableConnectionProperties.h"
#include "Nuclex/Support/Text/StringMatcher.h" // for CaseInsensitiveUtf8Less

#include <map> // for std::map

// DEDUP: This code is now an almost exact copy of the ConnectionUrl class.
//   Should we introduce another, intermediate base class?

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Stores connection settings as semicolon-delimited key/value pairs</summary>
  /// <remarks>
  ///   <para>
  ///     This resembles the connection strings from ADO.NET. Similar to ADO.NET, the actual
  ///     properties a connection string is allowed to habe depend on the database connector,
  ///     but unlike ADO.NET, the connection string can specify the connector that should be
  ///     used as well, via a fixed property.
  ///   </para>
  ///   <para>
  ///     You can use this, or JDBC-style connection URIs, to convert database connection
  ///     settings into simple strings that can easily be stored in confguration files or
  ///     be updated by the user.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE ConnectionString : public WritableConnectionProperties {

    /// <summary>Name of the property through which the driver can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string DriverPropertyName;
    /// <summary>Name of the property through which the database host can be specified</summary>
    /// <remarks>
    ///   For weirdo databases that let you run different database engines on the same computer
    ///   answering on the same port whilst giving them different names - SQL Server namely,
    ///   the instance name should be appended here with a slash,
    ///   i.e. <code>127.0.0.1/InstanceName</code>.
    /// </remarks>
    public: NUCLEX_THINORM_API static const std::u8string HostPropertyName;
    /// <summary>Path in which the database is stored for file-based databases</summary>
    /// <remarks>
    ///   This is interchangeable with the <see cref="HostPropertyName" /> and can be more
    ///   expressive when you're dealing with file-based database engines.
    /// </remarks>
    public: NUCLEX_THINORM_API static const std::u8string PathPropertyName;
    /// <summary>Name of the property through which the user name can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string UserPropertName;
    /// <summary>Name of the property through which the password can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string PasswordPropertName;
    /// <summary>Name of the property through which the port can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string PortPropertyName;
    /// <summary>Name of the property through which the database can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string DatabasePropertName;

    /// <summary>Initializes a new connection string with default settings</summary>
    public: NUCLEX_THINORM_API ConnectionString();
    /// <summary>Frees all resources owned by the connection string</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionString() override = default;

    /// <summary>Parses a connection string from the specified set of properties</summary>
    /// <param name="properties">Connection string that will be parsed</param>
    /// <returns>
    ///   The parsed properties as an instance of this connection string wrapper class
    /// </returns>
    public: NUCLEX_THINORM_API static ConnectionString Parse(const std::u8string &properties);

    /// <summary>Converts the connection string back into a plain string</summary>
    /// <returns>A string containing the connection properties</returns>
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
    /// <param name="driver">The driver that should be used to access the database</param>
    public: NUCLEX_THINORM_API void SetDriver(const std::u8string &driver) override {
      this->driver = driver;
    }

    /// <summary>Retrieves the hostname of the database server or database path</summary>
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
    /// <param name="hostnameOrPath">
    ///   IP, hostname or single-file database path to use
    /// </param>
    public: NUCLEX_THINORM_API void SetHostnameOrPath(
      const std::u8string &hostnameOrPath
    ) override {
      this->hostnameOrPath = hostnameOrPath;
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
    /// <param name="port">TCP port number that should be used when connecting</param>
    public: NUCLEX_THINORM_API void SetPort(
      const std::optional<std::uint16_t> &port = std::optional<std::uint16_t>()
    ) override {
      this->port = port;
    }

    /// <summary>Retrieves the name of the user to identify as when connecting</summary>
    /// <returns>The user name by which to connect to the database server</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetUser() const override {
      return this->user;
    }

    /// <summary>Sets the name of the user to identify as when connecting</summary>
    /// <param name="user">User to identify as when connecting to the database server</param>
    public: NUCLEX_THINORM_API void SetUser(
      const std::optional<std::u8string> &user = std::optional<std::u8string>()
    ) override {
      this->user = user;
    }

    /// <summary>Retrieves the password to use when connecting to the database</summary>
    /// <returns>The password to use when connecting to the database</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetPassword() const override {
      return this->password;
    }

    /// <summary>Sets the password to use when connecting to the database</summary>
    /// <param name="password">Password to use when connecting to the database</param>
    public: NUCLEX_THINORM_API void SetPassword(
      const std::optional<std::u8string> &password = std::optional<std::u8string>()
    ) override {
      this->password = password;
    }

    /// <summary>
    ///   Retrieves the name of the database that should be opened upon connecting
    /// </summary>
    /// <returns>The name of the database to open after connecting</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetDatabaseName() const override {
      return this->databaseName;
    }

    /// <summary>Sets the name of the database to open upon connecting</summary>
    /// <param name="databaseName">Database to open when connecting</param>
    public: NUCLEX_THINORM_API void SetDatabaseName(
      const std::optional<std::u8string> &databaseName = std::optional<std::u8string>()
    ) override {
      this->databaseName = databaseName;
    }

    /// <summary>Retrieves the value of an arbitrary option</summary>
    /// <param name="name">Name of the option whose value will be retrieved</param>
    /// <returns>The value of the option or nothing if the option wasn't set</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> GetOption(
      const std::u8string &name
    ) const override;

    /// <summary>Sets the value of an arbitrary option or unsets the option</summary>
    /// <parma name="name">Name of the option whose value will be changed</param>
    /// <param name=:value">
    ///   Value that will be assigned to the option, or nothing to unset the option
    /// </param>
    public: NUCLEX_THINORM_API void SetOption(
      const std::u8string &name,
      const std::optional<std::u8string> &value = std::optional<std::u8string>()
    ) override;

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

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONSTRING_H
