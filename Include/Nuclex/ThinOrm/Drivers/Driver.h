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

#ifndef NUCLEX_THINORM_DRIVERS_DRIVER_H
#define NUCLEX_THINORM_DRIVERS_DRIVER_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <optional> // for std::optional
#include <memory> // for std::shared_ptr

namespace Nuclex::ThinOrm::Configuration {
  class ConnectionProperties;
}
namespace Nuclex::ThinOrm {
  class Connection;
}

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Used by the <see cref="ConnectionFactory" /> to delegate actual connection creation
  ///   to the driver responsible for the database engine specified through
  ///   the <see cref='ConnectionProperties" />.
  /// </summary>
  class NUCLEX_THINORM_TYPE Driver {

    /// <summary>Frees all resources owned by the database driver</summary>
    public: NUCLEX_THINORM_API virtual ~Driver() = default;

    /// <summary>Retrieves a human-readable name for the driver</summary>
    /// <returns>A string that describes the driver</returns>
    /// <remarks>
    ///   <para>
    ///     This is not the driver identifier found in connection strings or connection urls,
    ///     but a describing name that describes the actual driver and varies between versions.
    ///   </para>
    ///   <para>
    ///     It could, for example, state "MariaDB/MySQL via MySQL Connector/C++ 9.4.0" or
    ///     "SQLite 3.18 Embedded (built-in)." It is intended to identify the driver being
    ///     used for display and debugging purposes.
    /// </remarks>
    public: virtual const std::u8string &GetName() const = 0;

    /// <summary>Establishes a connection to the specified database</summary>
    /// <param name="connectionProperties">
    ///   Settings that specify the hostname or path, protocol, login data and extra options.
    ///   How these settings are used is up to the implementation of the driver.
    /// </param>
    /// <returns>A new database connection</returns>
    public: virtual std::shared_ptr<Connection> Connect(
      const ConnectionProperties &connectionProperties
    ) = 0;

    //public: virtual std::optional<std::shared_ptr<Connection>> TryConnect(
    //  const ConnectionProperties &connectionProperties
    //) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Drivers

#endif // NUCLEX_THINORM_DRIVERS_DRIVER_H
