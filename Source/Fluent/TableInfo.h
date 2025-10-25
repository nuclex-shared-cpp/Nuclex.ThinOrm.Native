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

#ifndef NUCLEX_THINORM_FLUENT_TABLEINFO_H
#define NUCLEX_THINORM_FLUENT_TABLEINFO_H

#include "Nuclex/ThinOrm/Config.h"

#include "./ColumnInfo.h"

#include <unordered_map> // for std::unordered_map<>

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Private implementation of the global entity registry</summary>
  class TableInfo {

    /// <summary>Name of the table in the database</summary>
    public: std::u8string Name;

    /// <summary>Maps strings to column information records</summary>
    public: typedef std::unordered_map<std::size_t, ColumnInfo> SizeColumnInfoMap;

    /// <summary>Column metadata and getter/setter functions keyed by column offset</summary>
    public: SizeColumnInfoMap Columns;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent

#endif // NUCLEX_THINORM_FLUENT_TABLEINFO_H
