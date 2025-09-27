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

#ifndef NUCLEX_THINORM_MIGRATIONS_CONTEXTUALMIGRATION_H
#define NUCLEX_THINORM_MIGRATIONS_CONTEXTUALMIGRATION_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Migrations/Migration.h"

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Migration that specifies which database it is intended for</summary>
  /// <typeparam name="TDataContext">Data context class of the target database</typeparam>
  template<typename TDataContext = void>
  class NUCLEX_THINORM_TYPE ContextualMigration : public Migration {

    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API inline virtual ~ContextualMigration() override = default;

    /// <summary>
    ///   RTTI type id of the data context for the database the migration applies to
    /// </summary>
    /// <returns>The RTTI type id of the data context for the target database</returns>
    public: NUCLEX_THINORM_API inline const std::type_info *GetDataContextType() const override;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline const std::type_info *ContextualMigration<TDataContext>::GetDataContextType() const {
    static const std::type_info &typeInfo = typeid(TDataContext);
    return &typeInfo;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_CONTEXTUALMIGRATION_H
