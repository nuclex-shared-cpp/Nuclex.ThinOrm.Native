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

#ifndef NUCLEX_THINORM_FLUENT_ATTRIBUTEACCESSOR_H
#define NUCLEX_THINORM_FLUENT_ATTRIBUTEACCESSOR_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Value.h"

#include <string> // for std::u8string

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Helper to extract the entity and attribute type from an attribute pointer
  /// </summary>
  /// <typeparam name="TAttributePointer">
  ///   The attribute pointer type that should be split
  /// </typeparam>
  template<typename TAttributePointer>
  class AttributePointerTraits;

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Helper to extract the entity and attribute type from an attribute pointer,
  ///   partial specialized version that splits entity and attribute types
  /// </summary>
  /// <typeparam name="TEntity">Type of the entity class the attribute belongs to</typeparam>
  /// <typeparam name="TAttribute">Type of the attribute in the entity class</typeparam>
  template<typename TEntity, typename TAttribute>
  class AttributePointerTraits<TAttribute TEntity::*> {

    /// <summary>The type of the entity class</summary>
    public: typedef TEntity EntityType;

    /// <summary>The type of the attribute in the entity class</summary>
    public: typedef TAttribute AttributeType;

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Provides accessors by which registered attributes of an entity class can be
  ///   read and written
  /// </summary>
  /// <typeparam name="AttributePointer">
  ///   The offset pointer of an attribute in an entity class
  /// </typeparam>
  template<auto AttributePointer>
  class NUCLEX_THINORM_TYPE AttributeAccessor {

    /// <summary>Helper that implements the getter function for an attribute</summary>
    /// <typeparam name="TAttribute">Type of the attribute the getter will read</typeparam>
    /// <param name="entityAsVoid">
    ///   Instance of the entity class (type is TAttribute) from which the attribute
    ///   value will be obtained
    /// </param>
    /// <returns>
    ///   The value of the attribute, wrapped in a <see cref="Value" /> container
    /// </returns>
    public: static inline Value Get(const void *entityAsVoid) noexcept;

    /// <summary>Helper that implements the setter function for an attribute</summary>
    /// <typeparam name="TAttribute">Type of the attribute the setter will update</typeparam>
    /// <param name="entityAsVoid">
    ///   Instance of the entity class (type is TAttribute) into which the attribute
    ///   value will be stored
    /// </param>
    /// <param name="value">
    ///   Value that will be written into the attribute, wrapped in a <see cref="Value" />
    /// </param>
    public: static inline void Set(void *entityAsVoid, const Value &value) noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

  template<auto AttributePointer>
  Value AttributeAccessor<AttributePointer>::Get(const void *entityAsVoid) noexcept {
    using TEntity = typename AttributePointerTraits<decltype(AttributePointer)>::EntityType;
    using TAttribute = typename AttributePointerTraits<decltype(AttributePointer)>::AttributeType;

    const TEntity &entity = *static_cast<const TEntity *>(entityAsVoid);
    const TAttribute &attributeValue = entity.*AttributePointer;

    // Note: if you are binding your own entity class and your compiler shows an error here
    // that says that a 'Value' cannot be constructed from whatever type 'attributeValue' is,
    // that means your entity class uses an attribute of a type that this ORM system does not
    // support. Please check the constructors of the 'Value' class to see supported types.
    return Value(attributeValue);
  }

  // ------------------------------------------------------------------------------------------- //

  template<auto AttributePointer>
  void AttributeAccessor<AttributePointer>::Set(
    void *entityAsVoid, const Value &value
  ) noexcept {
    using TEntity = typename AttributePointerTraits<decltype(AttributePointer)>::EntityType;
    using TAttribute = typename AttributePointerTraits<decltype(AttributePointer)>::AttributeType;

    TEntity &entity = *static_cast<TEntity *>(entityAsVoid);

    // Note: if you are binding your own entity class and your compiler shows an error here
    // that says that a 'Value' cannot be converted to whatever type 'TAttribute' is, that
    // means your entity class uses an attribute of a type that this ORM system does not
    // support. Please check the constructors of the 'Value' class to see supported types.
    entity.*AttributePointer = static_cast<TAttribute>(value); // uses type conversion operator
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_ATTRIBUTEACCESSOR_H
