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

#ifndef NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_H
#define NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Fluent/TableRegistrationSyntax.h"

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Exposes a table in the data contex for fluent queriest</summary>
  class NUCLEX_THINORM_TYPE GlobalEntityRegistry {

    /// <summary>Registers an entity class that maps to a specific table</summary>
    /// <typeparam name="TEntity">Entity class that will be registered</typeparam>
    /// <param name="tableName">Name of the table in the database</param>
    /// <returns>
    ///   A fluent interface helper that provides the syntactic methods for mapping
    ///   columns in the table to attributes in the entity class.
    /// </returns>
    public: template<typename TEntity>
    NUCLEX_THINORM_API inline TableRegistrationSyntax<TEntity> RegisterTable(
      const std::u8string_view &tableName
    );

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TEntity>
  inline TableRegistrationSyntax<TEntity> GlobalEntityRegistry::RegisterTable(
    const std::u8string_view &tableName
  ) {
    // TODO: Register the table in the global entity registry
    return TableRegistrationSyntax<TEntity>(this, tableName);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_GLOBALENTITYREGISTRY_H
