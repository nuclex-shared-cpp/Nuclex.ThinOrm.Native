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

#ifndef NUCLEX_THINORM_FLUENT_COLUMNREGISTRATIONSYNTAX_H
#define NUCLEX_THINORM_FLUENT_COLUMNREGISTRATIONSYNTAX_H

#include "Nuclex/ThinOrm/Config.h"
#include <string> // for std::u8string_view

namespace Nuclex::ThinOrm::Fluent {
  class GlobalEntityRegistry;
  //template<typename TEntity> class TableRegistrationSyntax;
}

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Syntax used to fluently describe columns to the <see cref="GlobalEntityRegistry" />
  /// </summary>
  template<typename TEntity, typename TAttribute>
  class NUCLEX_THINORM_TYPE ColumnRegistrationSyntax {

    /// <summary>Initializes the column registration syntax helper</summary>
    /// <param name="registry">
    ///   Entity registry in which the description of the entity-table mapping will
    ///   ultimately be stored
    /// </param>
    /// <param name="tableName">Name of the table for which a mapping is being set up</param>
    /// <param name="columnName">Column whose mapping is being configured</param>
    public: NUCLEX_THINORM_API inline ColumnRegistrationSyntax(
      GlobalEntityRegistry &registry,
      const std::u8string_view &tableName,
      const std::u8string_view &columnName
    );

    public: ColumnRegistrationSyntax &NotNull();
    public: ColumnRegistrationSyntax &PrimaryKey();
    public: ColumnRegistrationSyntax &Identity();
    public: ColumnRegistrationSyntax &Length();
    public: ColumnRegistrationSyntax &WithColumn(const std::u8string_view &nextColumn /* , ?? */);

    /// <summary>Global registry in which the table will be registered</summary>
    private: GlobalEntityRegistry &registry;
    /// <summary>Name of the table that is being set up</summary>
    private: std::u8string_view tableName;
    /// <summary>Name of the column that is being configured</summary>
    private: std::u8string_view columnName;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TEntity, typename TAttribute>
  inline ColumnRegistrationSyntax<TEntity, TAttribute>::ColumnRegistrationSyntax(
    GlobalEntityRegistry &registry,
    const std::u8string_view &tableName,
    const std::u8string_view &columnName
  ) :
    registry(registry),
    tableName(tableName),
    columnName(columnName) {}

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_COLUMNREGISTRATIONSYNTAX_H
