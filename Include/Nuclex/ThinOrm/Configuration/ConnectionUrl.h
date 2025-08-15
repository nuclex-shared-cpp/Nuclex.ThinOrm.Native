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

#include <string> // for std::u8string
#include <optional> // for std::optional<>

namespace Nuclex { namespace ThinOrm { namespace Configuration {

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
  class NUCLEX_THINORM_TYPE ConnectionUrl {

    /// <summary>Initializes a new connection string</summary>
    public: NUCLEX_THINORM_TYPE ConnectionUrl(const std::u8string &uri);
    /// <summary>Frees all resources owned by the command</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionUrl() = default;

    /// <summary>Parses a connection string from the specified set of properties</summary>
    /// <param name="properties">Connection string that will be parsed</param>
    /// <returns>
    ///   The parsed properties as an instance of this connection string wrapper class
    /// </returns>
    public: NUCLEX_THINORM_API static ConnectionUrl Parse(const std::u8string &url);

    /// <summary>Converts the connection string back into a plain string</summary>
    /// <returns>A string containing the connection properties</returns>
    public: std::u8string ToString() const;

  };

  // ------------------------------------------------------------------------------------------- //

}}} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_CONNECTIONURL_H
