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

#ifndef NUCLEX_THINORM_FLUENT_TABLEREGISTRATIONSYNTAX_H
#define NUCLEX_THINORM_FLUENT_TABLEREGISTRATIONSYNTAX_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Fluent/ColumnRegistrationSyntax.h" // for ColumnRegistrationSyntax
#include "Nuclex/ThinOrm/Fluent/AttributeAccessor.h" // for AttributeAccessor

#include <string> // for std::u8string

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Syntax used to fluently describe tables to the <see cref="GlobalEntityRegistry" />
  /// </summary>
  template<typename TEntity>
  class NUCLEX_THINORM_TYPE TableRegistrationSyntax {

    /// <summary>Initializes the table registration syntax helper</summary>
    /// <param name="registry">
    ///   Entity registry in which the description of the entity-table mapping will
    ///   ultimately be stored
    /// </param>
    /// <param name="tableName">Name of the table in the database to set up</param>
    public: NUCLEX_THINORM_API inline TableRegistrationSyntax(
      EntityMappingConfigurator &registry, const std::u8string_view &tableName
    );

    /// <summary>Maps a column in the table to an attribute in the entity class</summary>
    /// <typeparam name="TAttribute">Type of the attribute that is being mapped</typeparam>
    /// <param name="columnName">Name of the colum in the database table</param>
    /// <param name="member">Address of the attribute in the entity class</param>
    /// <returns>
    ///   A fluent interface helper that provides the syntactic methods for describing
    ///   the column or starting the registration of another column.
    /// </returns>
    public: template<typename TAttribute, TAttribute TEntity::*attribute>
    NUCLEX_THINORM_API inline ColumnRegistrationSyntax<TEntity, TAttribute> WithColumn(
      const std::u8string_view &columnName //, TAttribute TEntity::*member
    );

    /// <summary>Global registry in which the table will be registered</summary>
    private: EntityMappingConfigurator &registry;
    /// <summary>Name of the table that is being set up</summary>
    private: std::u8string_view tableName;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TEntity>
  inline TableRegistrationSyntax<TEntity>::TableRegistrationSyntax(
    EntityMappingConfigurator &registry, const std::u8string_view &tableName
  ) :
    registry(registry),
    tableName(tableName) {}

  // ------------------------------------------------------------------------------------------- //

  template<typename TEntity>
  template<typename TAttribute, TAttribute TEntity::*attribute>
  inline ColumnRegistrationSyntax<
    TEntity, TAttribute
  > TableRegistrationSyntax<TEntity>::WithColumn(
    const std::u8string_view &columnName//, TAttribute TEntity::*member
  ) {
    /*
    this->registry.AddEntityAttribute(
      typeid(TEntity),
      columnName,
      &AttributeAccessor<TEntity, TAttribute, attribute>::Get,
      &AttributeAccessor<TEntity, TAttribute, attribute>::Set
    );
    */
    return ColumnRegistrationSyntax<TEntity, TAttribute>(
      this->registry, this->tableName, columnName
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_TABLEREGISTRATIONSYNTAX_H
