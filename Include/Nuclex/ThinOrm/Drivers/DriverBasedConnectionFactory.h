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

#ifndef NUCLEX_THINORM_DRIVERS_DRIVERBASEDCONNECTIONFACTORY_H
#define NUCLEX_THINORM_DRIVERS_DRIVERBASEDCONNECTIONFACTORY_H

#include "Nuclex/ThinOrm/Config.h"

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Establishes database connections according to the settings specified in
  ///   a <see cref="ConnectionProperties" /> instance, picking the matching
  ///   <see cref="Driver" /> from its own repository of registered drivers.
  /// </summary>
  class NUCLEX_THINORM_TYPE DriverBasedConnectionFactory {

    /// <summary>Frees all resources owned by the connection factory</summary>
    public: NUCLEX_THINORM_API virtual ~DriverBasedConnectionFactory() = default;


  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Drivers

#endif // NUCLEX_THINORM_DRIVERS_DRIVERBASEDCONNECTIONFACTORY_H
