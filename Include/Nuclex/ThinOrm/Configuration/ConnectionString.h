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

#include <string> // for std::u8string
#include <optional> // for std::optional<>

namespace Nuclex { namespace ThinOrm { namespace Configuration {

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
  class NUCLEX_THINORM_TYPE ConnectionString {

    /// <summary>Name of the property through which the driver can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string &DriverPropertyName;
    /// <summary>Name of the property through which the database host can be specified</summary>
    /// <remarks>
    ///   For weirdo databases that let you run different database engines on the same computer
    ///   answering on the same port whilst giving them different names - SQL Server namely,
    ///   the instance name should be appended here with a slash,
    ///   i.e. <code>127.0.0.1/InstanceName</code>.
    /// </remarks>
    public: NUCLEX_THINORM_API static const std::u8string &HostPropertyName;
    /// <summary>Name of the property through which the user name can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string &UserPropertName;
    /// <summary>Name of the property through which the password can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string &PasswordPropertName;
    /// <summary>Name of the property through which the port can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string &PortPropertName;
    /// <summary>Name of the property through which the database can be specified</summary>
    public: NUCLEX_THINORM_API static const std::u8string &DatabasePropertName;

    /// <summary>Initializes a new connection string</summary>
    public: NUCLEX_THINORM_TYPE ConnectionString(const std::u8string &properties);
    /// <summary>Frees all resources owned by the command</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionString() = default;

    /// <summary>Parses a connection string from the specified set of properties</summary>
    /// <param name="properties">Connection string that will be parsed</param>
    /// <returns>
    ///   The parsed properties as an instance of this connection string wrapper class
    /// </returns>
    public: NUCLEX_THINORM_API static ConnectionString Parse(const std::u8string &properties);

    /// <summary>Converts the connection string back into a plain string</summary>
    /// <returns>A string containing the connection properties</returns>
    public: std::u8string ToString() const;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONSTRING_H
