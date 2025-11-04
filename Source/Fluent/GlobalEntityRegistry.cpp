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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_THINORM_SOURCE 1

#include "Nuclex/ThinOrm/Fluent/GlobalEntityRegistry.h"

#include "./GlobalEntityRegistry.Implementation.h"

#include <stdexcept> // for std::invalid_argument

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Fetches the <see cref="ColumnInfo" /> class from the registry when given
  ///   the RTTI type information for the entity class and the column name
  /// </summary>
  /// <param name="tables">Table dictionary in which the entity will be looked up</param>
  /// <param name="entityTyep">RTTY type information of the entity class to look up</param>
  /// <param name="columnName">Name of the column to look up</param>
  /// <returns>The metadata recorded for the specified column in the specified entity</returns>
  Nuclex::ThinOrm::Fluent::ColumnInfo &getColumnInfoForPropertyUpdateOrThrow(
    std::unordered_map<std::type_index, Nuclex::ThinOrm::Fluent::TableInfo> &tables,
    const std::type_info &entityType,
    const std::u8string &columnName
  ) {
    typedef std::unordered_map<
      std::type_index, Nuclex::ThinOrm::Fluent::TableInfo
    > TypeTableInfoMap;

    TypeTableInfoMap::iterator tableIterator = tables.find(std::type_index(entityType));
    if(tableIterator == tables.end()) {
      throw std::invalid_argument(
        reinterpret_cast<const char *>(
          u8"Tried to change the properties of a column<->attribute mapping for an entity "
          u8"class that had not been registered as an entity class first."
        )
      );
    }

    Nuclex::ThinOrm::Fluent::TableInfo::SizeColumnInfoMap::iterator columnIterator = (
      tableIterator->second.Columns.find(columnName)
    );
    if(columnIterator == tableIterator->second.Columns.end()) {
      throw std::invalid_argument(
        reinterpret_cast<const char *>(
          u8"Tried to change the properties of a column<->attribute mapping for a column "
          u8"that had not been registered first."
        )
      );
    }

    return columnIterator->second;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  GlobalEntityRegistry::GlobalEntityRegistry() :
    implementation(std::make_unique<Implementation>()) {}

  // ------------------------------------------------------------------------------------------- //

  GlobalEntityRegistry::~GlobalEntityRegistry() = default;

  // ------------------------------------------------------------------------------------------- //

  void GlobalEntityRegistry::AddEntity(
    const std::type_info &entityType, const std::u8string_view &tableName
  ) {
    this->implementation->Tables.emplace(
      std::type_index(entityType),
      TableInfo(std::u8string(tableName), entityType)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void GlobalEntityRegistry::AddEntityAttribute(
    const std::type_info &entityType,
    const std::u8string_view &columnName,
    GetAttributeValueFunction *getter,
    SetAttributeValueFunction *setter,
    const std::type_info &attributeType
  ) {
    Implementation::TypeTableInfoMap::iterator iterator = this->implementation->Tables.find(
      std::type_index(entityType)
    );
    if(iterator == this->implementation->Tables.end()) {
      throw std::invalid_argument(
        reinterpret_cast<const char *>(
          u8"Tried to set up a column<->attribute mapping for an entity class that "
          u8"had not been registered as an entity class first."
        )
      );
    }

    iterator->second.Columns.emplace(
      std::u8string(columnName),
      ColumnInfo(std::u8string(columnName), attributeType, getter, setter)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void GlobalEntityRegistry::SetColumnNullable(
    const std::type_info &entityType,
    const std::u8string_view &columnName,
    bool isNullable /* = true */
  ) {
    ColumnInfo &columnInfo = getColumnInfoForPropertyUpdateOrThrow(
      this->implementation->Tables, entityType, std::u8string(columnName)
    );
    columnInfo.IsNullable = isNullable;
  }

  // ------------------------------------------------------------------------------------------- //

  void GlobalEntityRegistry::SetColumnIsPrimaryKey(
    const std::type_info &entityType,
    const std::u8string_view &columnName,
    bool isPrimaryKey /* = true */
  ) {
    ColumnInfo &columnInfo = getColumnInfoForPropertyUpdateOrThrow(
      this->implementation->Tables, entityType, std::u8string(columnName)
    );
    columnInfo.IsPrimaryKey = isPrimaryKey;
  }

  // ------------------------------------------------------------------------------------------- //

  void GlobalEntityRegistry::SetColumnIsAutoGenerated(
    const std::type_info &entityType,
    const std::u8string_view &columnName,
    bool isAutoGenerated /* = true */
  ) {
    ColumnInfo &columnInfo = getColumnInfoForPropertyUpdateOrThrow(
      this->implementation->Tables, entityType, std::u8string(columnName)
    );
    columnInfo.IsAutogenerated = isAutoGenerated;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent
