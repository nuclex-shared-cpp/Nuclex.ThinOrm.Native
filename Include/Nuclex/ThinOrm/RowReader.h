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

#ifndef NUCLEX_THINORM_ROWREADER_H
#define NUCLEX_THINORM_ROWREADER_H

#include "Nuclex/ThinOrm/Config.h"

#include <cstdint> // for std::int16_t, std::int32_t, etc.
#include <string> // for std::u8string

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads rows from a query that results in multiple rows</summary>
  class NUCLEX_THINORM_TYPE RowReader {

    public: NUCLEX_THINORM_API RowReader() {}
    public: NUCLEX_THINORM_API virtual ~RowReader() = default;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_ROWREADER_H
