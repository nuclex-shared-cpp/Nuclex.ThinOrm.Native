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

#ifndef NUCLEX_THINORM_CONNECTIONS_CONTEXTUALCONNECTIONPOOL_H
#define NUCLEX_THINORM_CONNECTIONS_CONTEXTUALCONNECTIONPOOL_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Connections/ConnectionPool.h"

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Connection pool with a template argument to make each database's connection pool
  ///   a unique type for dependency injectors
  /// </summary>
  /// <typeparam name="TDataContext">
  ///   Specialization to distinguish the types in C++ dependency injectors.
  ///   Ignroe this if you do not use a dependency injector or if you only
  ///   access a single database in your application.
  /// </typeparam>
  template<typename TDataContext = void>
  class NUCLEX_THINORM_TYPE ContextualConnectionPool : public ConnectionPool {

    /// <summary>Frees all resources owned by the connection pool</summary>
    public: NUCLEX_THINORM_API inline virtual ~ContextualConnectionPool() override = default;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_CONTEXTUALCONNECTIONPOOL_H
