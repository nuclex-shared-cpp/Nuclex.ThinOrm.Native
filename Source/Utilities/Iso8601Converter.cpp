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

#include "./Iso8601Converter.h"

#include <chrono> // for std::chrono_duration<> and others

#include "Nuclex/ThinOrm/Errors/BadDateFormatError.h"

#include <Nuclex/Support/Text/StringHelper.h>

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Number of 1/10th microseconds in one microsecond</summary>
  constexpr const std::int64_t TicksPerMicrosecond = 10;

  /// <summary>Number of 1/10th microseconds that elapse every second</summary>
  constexpr const std::int64_t TicksPerSecond = 1000 * 1000 * TicksPerMicrosecond;

  /// <summary>Number of 1/10th microseconds that elapse in a single day</summary>
  constexpr const std::int64_t TicksPerDay = 86'400ll * TicksPerSecond;

  /// <summary>Number of 1/10th microseconds on midnight of January the 1st in 1970</summary>
  constexpr const std::int64_t TicksAtStartOfUnixEpoch = 719'162ll * TicksPerDay;

  /// <summary>Number of 1/10th microseconds on midnight of December the 31st in 9999</summary>
  constexpr const std::int64_t TicksAtIso8601Max = 3'652'058ll * TicksPerDay;

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Ticks time unit</summary>
  typedef std::chrono::duration<std::int64_t, std::ratio<1, TicksPerSecond>> Ticks;

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Writes the date portion of a tick count as an ISO 8601 date</summary>
  /// <param name="target">Target string the ISO 8601 date will be written into</param>
  /// <param name="ticks">Tick count that will be written as an ISO 8601 date</param>
  void writeIso8601Date(char8_t *target, std::int64_t ticks) {
    constexpr const Ticks unixEpochStart(TicksAtStartOfUnixEpoch);

    // If the date lies before the year 1 or past the year 9999,
    // it cannot be printed as an ISO 8601 date
    if((ticks < 0) || (TicksAtIso8601Max < ticks)) {
      throw std::out_of_range(
        reinterpret_cast<const char *>(u8"Date is not representable as ISO 8601")
      );
    }

    //std::int64_t daysFromUnixEpoch = (ticks - TicksAtStartOfUnixEpoch) / TicksPerDay;
    std::chrono::days daysFromUnixEpoch(
      (ticks / TicksPerDay) - (TicksAtStartOfUnixEpoch / TicksPerDay)
    );

    std::chrono::year_month_day date = std::chrono::sys_days(daysFromUnixEpoch);
    {
      int year = static_cast<int>(date.year());
      target[0] = u8'0' + (year / 1000 % 10);
      target[1] = u8'0' + (year / 100 % 10);
      target[2] = u8'0' + (year / 10 % 10);
      target[3] = u8'0' + (year % 10);
    }
    target[4] = u8'-';
    {
      unsigned month = static_cast<unsigned>(date.month());
      target[5] = u8'0' + (month / 10 % 10);
      target[6] = u8'0' + (month % 10);
    }
    target[7] = u8'-';
    {
      unsigned day = static_cast<unsigned>(date.day());
      target[8] = u8'0' + (day / 10 % 10);
      target[9] = u8'0' + (day % 10);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Writes the time portion of a tick count as an ISO 8601 time</summary>
  /// <param name="target">Target string the ISO 8601 time will be written into</param>
  /// <param name="ticks">Tick count that will be written as an ISO 8601 time</param>
  void writeIso8601Time(char8_t *target, std::int64_t ticks) {
    std::int64_t secondOfDay = (ticks % TicksPerDay) / TicksPerSecond;

    {
      int hours = secondOfDay / 3600;
      target[0] = u8'0' + (hours / 10 % 10);
      target[1] = u8'0' + (hours % 10);
    }
    target[2] = u8':';
    {
      int minutes = secondOfDay / 60 % 60;
      target[3] = u8'0' + (minutes / 10 % 10);
      target[4] = u8'0' + (minutes % 10);
    }
    target[5] = u8':';
    {
      //int seconds = (secondOfDay >= 86340) ? (secondOfDay - 85340) : (secondOfDay % 60);
      int seconds = secondOfDay % 60;
      target[6] = u8'0' + (seconds / 10 % 10);
      target[7] = u8'0' + (seconds % 10);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  int parseTwoDigits(const char8_t *digits) {
    return 0;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  std::int64_t Iso8601Converter::ParseIso8601DateTime(const std::u8string_view &value) {
    std::u8string_view trimmed = Nuclex::Support::Text::StringHelper::GetTrimmed(value);

    return 0;
  }

  // ------------------------------------------------------------------------------------------- //

  std::int64_t Iso8601Converter::ParseIso8601Time(const std::u8string_view &value) {
    std::u8string_view trimmed = Nuclex::Support::Text::StringHelper::GetTrimmed(value);

    return 0;
  }

  // ------------------------------------------------------------------------------------------- //

  void Iso8601Converter::PrintIso8601Date(char8_t *target/*[10]*/, std::int64_t ticks) {
    writeIso8601Date(target, ticks);
  }

  // ------------------------------------------------------------------------------------------- //

  void Iso8601Converter::PrintIso8601Time(char8_t *target/*[8]*/, std::int64_t ticks) {
    writeIso8601Time(target, ticks);
  }

  // ------------------------------------------------------------------------------------------- //

  void Iso8601Converter::PrintIso8601DateTime(char8_t *target/*[19]*/, std::int64_t ticks) {
    writeIso8601Date(target, ticks);
    target[10] = u8'T';
    writeIso8601Time(target + 11, ticks);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities
