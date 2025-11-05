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

  /// <summary>Converts date and time values to and from ISO 8601</summary>
  /// <remarks>
  ///   This is a validating parser and printer for ISO 8601 date and time strings.
  ///   It supports only 4-digit years (no extended -/+ prefix 6-digit years) but covers
  ///   all other ISO 8601 date/time properties. It can parse the basic format, extended
  ///   format and it validates the time zone, if appended (the time zone itself is not
  ///   used as <see cref="DateTime" /> only supports UTC dates and times).
  /// </remarks>
  class Iso8601Converter {

    /// <summary>Parses an ISO 8601 date with optional time value</summary>
    /// <param name="value">Date (and, optionally, time) that will be parsed</param>
    /// <returns>
    ///   The tick count (tenth microseconds since the year 1) that matches the specified
    ///   ISO 8601 date (and, optionally, time)
    /// </returns>
    /// <remrks>
    ///   If the ISO 8601 string containsa time zone, it will be ignored. Dates are
    ///   always in UTC format
    /// </remarks>
    public: static std::int64_t ParseIso8601DateTime(const std::u8string_view &value);

    /// <summary>Parses an ISO 8601 time</summary>
    /// <param name="value">Time that will be parsed</param>
    /// <returns>
    ///   The tick count (tenth microseconds) that matches the specified ISO 8601 time
    /// </returns>
    /// <remrks>
    ///   If the ISO 8601 string containsa time zone, it will be ignored. Dates are
    ///   always in UTC format
    /// </remarks>
    public: static std::int64_t ParseIso8601Time(const std::u8string_view &value);

    /// <summary>Prints a tick count as an ISO 8601 date</summary>
    /// <param name="target">UTF-8 string buffer the date will be printed to</param>
    /// <param name="ticks">Tick count that will be printed as an ISO 8601 date</param>
    public: static void PrintIso8601Date(char8_t *target/*[10]*/, std::int64_t ticks);

    /// <summary>Prints a tick count as an ISO 8601 time</summary>
    /// <param name="target">UTF-8 string buffer the time will be printed to</param>
    /// <param name="ticks">Tick count that will be printed as an ISO 8601 time</param>
    public: static void PrintIso8601Time(char8_t *target/*[8]*/, std::int64_t ticks);

    /// <summary>Prints a tick count as an ISO 8601 date and time</summary>
    /// <param name="target">UTF-8 string buffer the date and time will be printed to</param>
    /// <param name="ticks">Tick count that will be printed as an ISO 8601 date and time</param>
    public: static void PrintIso8601DateTime(char8_t *target/*[19]*/, std::int64_t ticks);

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // NUCLEX_THINORM_UTILITIES_ISO8601CONVERTER_H
