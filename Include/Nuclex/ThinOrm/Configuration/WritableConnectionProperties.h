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

#ifndef NUCLEX_THINORM_CONFIGURATION_WRITABLECONNECTIONPROPERTIES_H
#define NUCLEX_THINORM_CONFIGURATION_WRITABLECONNECTIONPROPERTIES_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h"

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Interface for connection settings containers that can update their values
  /// </summary>
  /// <remarks>
  ///   <para>
  ///     When passing connection settings around in this application, only the getter
  ///     methods are needed, so you would not need to implement this interface for your
  ///     own connection settings container types.
  ///   </para>
  ///   <para>
  ///     However, it is provided due to this library already having two diffeent storage
  ///     formats for its database settings, each providing its own settings container class:
  ///     <see cref="ConnectionUrl" /> and <see cref="ConnectionString" />. Both implement
  ///     this interface, so if you wanted to write a some generic code that updates values
  ///     in either the containers, you could do so via this interface.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE WritableConnectionProperties : public ConnectionProperties {

    /// <summary>Frees all resources owned by the connection property container</summary>
    public: NUCLEX_THINORM_API virtual ~WritableConnectionProperties() = default;

    /// <summary>Sets the driver to use to access the database</summary>
    /// <param name="driver">The driver that should be used to access the database</param>
    public: virtual void SetDriver(const std::u8string &driver) = 0;

    /// <summary>Sets the hostname of the database server to connect to</summary>
    /// <param name="hostnameOrPath">
    ///   IP, hostname or single-file database path to use
    /// </param>
    public: virtual void SetHostnameOrPath(
      const std::u8string &hostnameOrPath
    ) = 0;

    /// <summary>Sets the TCP port on which the database server should be addressed</summary>
    /// <param name="port">TCP port number that should be used when connecting</param>
    public: virtual void SetPort(
      const std::optional<std::uint16_t> &port = std::optional<std::uint16_t>()
    ) = 0;

    /// <summary>Sets the name of the user to identify as when connecting</summary>
    /// <param name="user">User to identify as when connecting to the database server</param>
    public: virtual void SetUser(
      const std::optional<std::u8string> &user = std::optional<std::u8string>()
    ) = 0;

    /// <summary>Sets the password to use when connecting to the database</summary>
    /// <param name="password">Password to use when connecting to the database</param>
    public: virtual void SetPassword(
      const std::optional<std::u8string> &password = std::optional<std::u8string>()
    ) = 0;

    /// <summary>Sets the name of the database to open upon connecting</summary>
    /// <param name="databaseName">Database to open when connecting</param>
    public: virtual void SetDatabaseName(
      const std::optional<std::u8string> &databaseName = std::optional<std::u8string>()
    ) = 0;

    /// <summary>Sets the value of an arbitrary option or unsets the option</summary>
    /// <parma name="name">Name of the option whose value will be changed</param>
    /// <param name=:value">
    ///   Value that will be assigned to the option, or nothing to unset the option
    /// </param>
    public: virtual void SetOption(
      const std::u8string &name,
      const std::optional<std::u8string> &value = std::optional<std::u8string>()
    ) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_WRITABLECONNECTIONPROPERTIES_H
