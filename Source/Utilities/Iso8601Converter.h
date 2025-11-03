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

#ifndef NUCLEX_THINORM_UTILITIES_ISO8601CONVERTER_H
#define NUCLEX_THINORM_UTILITIES_ISO8601CONVERTER_H

#include "Nuclex/ThinOrm/Config.h"

#include <ctime> // for std::tm
#include <string> // for u8string_view
#include <cstdint> // for std::int64_t

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Converts date and time values between different formats</summary>
  class Iso8601Converter {

    public: static std::int64_t ParseIso8601DateTime(const std::u8string_view &value);

    public: static std::int64_t ParseIso8601Time(const std::u8string_view &value);

    public: static void PrintIso8601Date(char8_t *target/*[10]*/, std::int64_t ticks);

    public: static void PrintIso8601Time(char8_t *target/*[8]*/, std::int64_t ticks);

    public: static void PrintIso8601DateTime(char8_t *target/*[19]*/, std::int64_t ticks);

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // NUCLEX_THINORM_UTILITIES_DATETIMECONVERTER_H
