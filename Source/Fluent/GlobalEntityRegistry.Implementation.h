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

#ifndef NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_IMPLEMENTATION_H
#define NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_IMPLEMENTATION_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Fluent/GlobalEntityRegistry.h"

#include "./TableInfo.h"

#include <typeindex> // for std::type_index
#include <unordered_map> // for std::unordered_map<>

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Private implementation of the global entity registry</summary>
  class GlobalEntityRegistry::Implementation {

    /// <summary>Initializes the private entity registry implementation</summary>
    public: NUCLEX_THINORM_API Implementation();

    /// <summary>Frees all resources owned by the entity registry implementation</summary>
    public: NUCLEX_THINORM_API ~Implementation();

    /// <summary>Map of a RTTI type to database table descriptions/summary>
    private: typedef std::unordered_map<std::type_index, TableInfo> TypeTableInfoMap;

    /// <summary>Tables that have been registered for different entity types</summary>
    private: TypeTableInfoMap tables;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_IMPLEMENTATION_H
