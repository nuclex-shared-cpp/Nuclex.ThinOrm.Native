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

#ifndef NUCLEX_THINORM_DRIVERS_QTSQLCONNECTIONFACTORY_H
#define NUCLEX_THINORM_DRIVERS_QTSQLCONNECTIONFACTORY_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Drivers/ConnectionFactory.h"

#if defined(NUCLEX_THINORM_SUPPORT_ASYNCPP)
  #include <stop_token>
  #include <asyncpp/task.h> // for asyncpp::task
#endif

namespace Nuclex::ThinOrm::Drivers {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Establishes database connections through the Qt SQL module from Qt5 or Qt6
  /// </summary>
  class NUCLEX_THINORM_TYPE QtSqlConnectionFactory : public ConnectionFactory {

    /// <summary>Name of the option through which Qt connection name can be set</summary>
    public: NUCLEX_THINORM_API static const std::u8string ConnectionNameOptionName;

    /// <summary>Frees all resources owned by the connection factory</summary>
    public: NUCLEX_THINORM_API virtual ~QtSqlConnectionFactory() = default;

    /// <summary>Establishes a new connection to the specified database</summary>
    /// <param name="connectionProperties">
    ///   Specifies the driver, data source and other parameters to reach the database
    /// </param>
    /// <returns>A new database connection wrapper, if successful</returns>
    /// <remarks>
    ///   This factory passes the 'driver' setting directly to the 'QSqlDatabase' as
    ///   the type of the database, i.e. 'QSQLITE', so pick a valid Qt driver name. The other
    ///   properties are directly applied to the 'QSqlDatabase', whilst the 'connection name'
    ///   (which must be unique if Qt is to actually establish multiple connections) will be
    ///   auto-generated but can be prefixed with an identifier you can control by using
    ///   an options named <see cref="ConnectionNameOptionName" /> in the connection settings.
    /// </remarks>
    public: NUCLEX_THINORM_API std::shared_ptr<Connection> Connect(
      const Configuration::ConnectionProperties &connectionProperties
    ) const override;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Drivers

#endif // NUCLEX_THINORM_DRIVERS_QTSQLCONNECTIONFACTORY_H
