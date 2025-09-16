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

#ifndef NUCLEX_THINORM_CONNECTIONS_DRIVERBASEDCONNECTIONFACTORY_H
#define NUCLEX_THINORM_CONNECTIONS_DRIVERBASEDCONNECTIONFACTORY_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Connections/ConnectionFactory.h"

#if defined(NUCLEX_THINORM_SUPPORT_ASYNCPP)
  #include <stop_token>
  #include <asyncpp/task.h> // for asyncpp::task
#endif

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Establishes database connections according to the settings specified in
  ///   a <see cref="ConnectionProperties" /> instance, picking the matching
  ///   <see cref="Driver" /> from its own repository of registered drivers.
  /// </summary>
  class NUCLEX_THINORM_TYPE DriverBasedConnectionFactory : public ConnectionFactory {

    /// <summary>Frees all resources owned by the connection factory</summary>
    public: NUCLEX_THINORM_API virtual ~DriverBasedConnectionFactory() = default;

    /// <summary>Establishes a new connection to the specified database</summary>
    /// <param name="connectionProperties">
    ///   Specifies the driver, data source and other parameters to reach the database
    /// </param>
    /// <returns>A new database connection wrapper, if successful</returns>
    /// <remarks>
    ///   Here, the 'driver' part of the connection properties specifies either
    ///   the abbreviated name ('sqlite', 'mariadb') of a database driver,
    ///   or the full name ('SQLite', 'MariaDB C/C++ Connector') of a driver that was
    ///   previously registered to the driver-based connection factory.
    /// </remarks>
    public: NUCLEX_THINORM_API [[nodiscard]] std::shared_ptr<Connection> Connect(
      const Configuration::ConnectionProperties &connectionProperties
    ) const override;

#if 0 && defined(NUCLEX_THINORM_SUPPORT_ASYNCPP)

    public: NUCLEX_THINORM_API asyncpp::task<std::shared_ptr<Connection>> ConnectAsync(
      const Configuration::ConnectionProperties &connectionProperties,
      const std::stop_token &stopToken
    ) const;

#endif

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_DRIVERBASEDCONNECTIONFACTORY_H
