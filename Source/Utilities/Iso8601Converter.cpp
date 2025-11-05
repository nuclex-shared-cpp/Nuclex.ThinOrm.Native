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
#include <cctype> // for std::isdigit()

#include "Nuclex/ThinOrm/Errors/BadDateFormatError.h" // for BadDateFormat

#include <Nuclex/Support/Text/StringHelper.h> // for StringHelper::GetTrimmed()
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>()

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
      std::size_t hours = static_cast<std::size_t>(secondOfDay / 3600);
      target[0] = u8'0' + (hours / 10 % 10);
      target[1] = u8'0' + (hours % 10);
    }
    target[2] = u8':';
    {
      std::size_t minutes = static_cast<std::size_t>(secondOfDay / 60 % 60);
      target[3] = u8'0' + (minutes / 10 % 10);
      target[4] = u8'0' + (minutes % 10);
    }
    target[5] = u8':';
    {
      //int seconds = (secondOfDay >= 86340) ? (secondOfDay - 85340) : (secondOfDay % 60);
      std::size_t seconds = static_cast<std::size_t>(secondOfDay % 60);
      target[6] = u8'0' + (seconds / 10 % 10);
      target[7] = u8'0' + (seconds % 10);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Parses the date portion of an ISO 8601 date/time value</summary>
  /// <param name="dateString">String that potentially contains an ISO 8601 date/time</param>
  /// <param name="index">Index in the string at which to begin parsing</param>
  /// <returns>The tick count representing the parsed date</returns>
  std::int64_t parseDate(
    const std::u8string_view &dateString, std::u8string::size_type &index
  ) {
    using Nuclex::ThinOrm::Errors::BadDateFormatError;
    using Nuclex::Support::Text::lexical_cast;

    std::u8string_view::size_type length = dateString.length();
    if(length < index + 8) {
      throw Nuclex::ThinOrm::Errors::BadDateFormatError(
        u8"Not an ISO 8601 date, value too short for a valid date"
      );
    }

    // Verify and read the year
    std::chrono::year year;
    {
      bool startsWithYear = (
        std::isdigit(dateString[index + 0]) &&
        std::isdigit(dateString[index + 1]) &&
        std::isdigit(dateString[index + 2]) &&
        std::isdigit(dateString[index + 3])
      );
      if(!startsWithYear) {
        throw BadDateFormatError(u8"Not an ISO 8601 date, year is not numeric");
      }

      year = std::chrono::year(lexical_cast<int>(dateString.substr(index, 4)));
      index += 4;
    }

    // Verify and read the month
    bool isExtendedFormat;
    std::chrono::month month;
    {
      isExtendedFormat = (dateString[index] == u8'-');
      if(isExtendedFormat) {
        ++index; // Skip dash for ISO 8601 extended date format (yyyy-MM-dd)
      }
      bool continuesWithMonth = (
        std::isdigit(dateString[index + 0]) &&
        std::isdigit(dateString[index + 1])
      );
      if(!continuesWithMonth) {
        throw BadDateFormatError(u8"Not an ISO 8601 date, month is not numeric");
      }

      unsigned numericMonth = lexical_cast<unsigned>(dateString.substr(index, 2));
      if((numericMonth < 1) || (12 < numericMonth)) {
        throw BadDateFormatError(u8"Not an ISO 8601 date, invalid month");
      }

      month = std::chrono::month(numericMonth);
      index += 2;
    }

    // Verify and read the day
    std::chrono::day day;
    {
      if((dateString[index] == u8'-') != isExtendedFormat) {
        throw BadDateFormatError(
          u8"Not an ISO 8601 date, "
          u8"mixing basic and extended format (date with dashes) is not allowed"
        );
      }
      if(isExtendedFormat) {
        ++index; // Skip dash for ISO 8601 extended date format (yyyy-MM-dd)

        // We only verified that the length is at least 8, but now we are at 8 already
        if(length < index + 2) { // guarantees offset up to 9
          throw BadDateFormatError(
            u8"Not an ISO 8601 date, value too short for a valid date"
          );
        }
      }

      bool endsWithDay = (
        std::isdigit(dateString[index + 0]) &&
        std::isdigit(dateString[index + 1])
      );
      if(!endsWithDay) {
        throw BadDateFormatError(u8"Not an ISO 8601 date, day is not numeric");
      }

      unsigned numericDay = lexical_cast<unsigned>(dateString.substr(index, 2));
      if((numericDay < 1) || (31 < numericDay)) {
        throw BadDateFormatError(u8"Not an ISO 8601 date, invalid day");
      }

      day = std::chrono::day(numericDay);
      index += 2;
    }

    // Forming an std::chrono::year_month_day will reveal if the date is invalid,
    // otherwise we could as well have returned a simple tick count for the date.
    std::chrono::year_month_day chronoDate = year / month / day;
    if(!chronoDate.ok()) {
      throw BadDateFormatError(u8"ISO 8601 date specifies an invalid date");
    }

    return TicksAtStartOfUnixEpoch + std::chrono::time_point_cast<Ticks>(
      std::chrono::sys_days(chronoDate)
    ).time_since_epoch().count();
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Parses the time portion of an ISO 8601 date/time value</summary>
  /// <param name="timeString">String that potentially contains an ISO 8601 time</param>
  /// <param name="index">Index in the string at which to begin parsing</param>
  /// <returns>The time of day parsed from the time string</returns>
  std::size_t parseTime(
    const std::u8string_view &timeString, std::u8string::size_type &index,
    std::optional<bool> requireExtendedFormat = std::optional<bool>()
  ) {
    using Nuclex::ThinOrm::Errors::BadDateFormatError;
    using Nuclex::Support::Text::lexical_cast;

    std::u8string_view::size_type length = timeString.length();
    if(length < index + 6) {
      throw Nuclex::ThinOrm::Errors::BadDateFormatError(
        u8"Not an ISO 8601 time, value too short for a valid time"
      );
    }

    // Verify and try to parse the hour
    std::size_t hour;
    {
      bool startsWithHour = (
        std::isdigit(timeString[index + 0]) &&
        std::isdigit(timeString[index + 1])
      );
      if(!startsWithHour) {
        throw BadDateFormatError(u8"Not an ISO 8601 time, hour is not numeric");
      }

      hour = lexical_cast<std::size_t>(timeString.substr(index, 2));
      if((hour < 0) || (23 < hour)) {
        throw BadDateFormatError(u8"Not an ISO 8601 time, invalid hour");
      }

      index += 2;
    }

    // Verify and try to parse the minute
    std::size_t minute;
    {
      bool isExtendedFormat = (timeString[index] == u8':');
      if(requireExtendedFormat.has_value()) {
        if(isExtendedFormat != requireExtendedFormat.value()) {
          throw BadDateFormatError(
            u8"Not an ISO 8601 time, "
            u8"mixing basic and extended format (time with double colons) is not allowed"
          );
        }
      } else {
        requireExtendedFormat = isExtendedFormat;
      }
      if(isExtendedFormat) {
        ++index; // Skip double colon for ISO 8601 extended time format (hh:mm:ss)
      }
      bool continuedWithMinute = (
        std::isdigit(timeString[index + 0]) &&
        std::isdigit(timeString[index + 1])
      );
      if(!continuedWithMinute) {
        throw BadDateFormatError(u8"Not an ISO 8601 time, minute is not numeric");
      }

      minute = lexical_cast<std::size_t>(timeString.substr(index, 2));
      if((minute < 0) || (59 < minute)) {
        throw BadDateFormatError(u8"Not an ISO 8601 time, invalid minute");
      }

      index += 2;
    }

    // Verify and try to parse the second
    std::size_t second;
    {
      bool isExtendedFormat = (timeString[index] == u8':');
      if(isExtendedFormat != requireExtendedFormat.value()) {
        throw BadDateFormatError(
          u8"Not an ISO 8601 time, "
          u8"mixing basic and extended format (time with double colons) is not allowed"
        );
      }
      if(isExtendedFormat) {
        ++index; // Skip double colon for ISO 8601 extended time format (hh:mm:ss)

        // We only verified that the length is at least 6, but now we are at 6 already
        if(length < index + 2) { // guarantees offset up to 8
          throw BadDateFormatError(
            u8"Not an ISO 8601 time, value too short for a valid time"
          );
        }
      }
      bool endsWithSecond = (
        std::isdigit(timeString[index + 0]) &&
        std::isdigit(timeString[index + 1])
      );
      if(!endsWithSecond) {
        throw BadDateFormatError(u8"Not an ISO 8601 time, second is not numeric");
      }

      second = lexical_cast<std::size_t>(timeString.substr(index, 2));
      if((hour == 23) && (minute == 59)) {
        if(second < 0) {
          throw BadDateFormatError(u8"Not an ISO 8601 time, invalid second");
        }
      } else {
        if((second < 0) || (59 < second)) {
          throw BadDateFormatError(u8"Not an ISO 8601 time, invalid second");
        }
      }

      index += 2;
    }

    // If more characters follow, validate that it's an appended local time zone,
    // even though we will ignore the time zone information
    if(index < length) {
      bool isUtc = (length == index + 1) && (timeString[index] == u8'Z');
      if(!isUtc) {
        bool hasPotentialTimeZone = (
          (length >= index + 3) &&
          (
            (timeString[index] == u8'-') ||
            (timeString[index] == u8'+')
          ) &&
          std::isdigit(timeString[index + 1]) &&
          std::isdigit(timeString[index + 2])
        );
        if(!hasPotentialTimeZone) {
          throw BadDateFormatError(u8"Not an ISO 8601 time, invalid time zone syntax");
        }

        if(length == index + 3) {
          // Valid. We already checked the 3 digits above
        } else if(length == index + 5) {
          bool isFourDigitTimeZone = (
            std::isdigit(timeString[index + 3]) &&
            std::isdigit(timeString[index + 4])
          );
          if(!isFourDigitTimeZone) {
            throw BadDateFormatError(u8"Not an ISO 8601 time, invalid time zone syntax");
          }
        } else if(length == index + 6) {
          bool isFiveDigitTimeZone = (
            (timeString[index + 3] == u8':') &&
            std::isdigit(timeString[index + 4]) &&
            std::isdigit(timeString[index + 5])
          );
          if(!isFiveDigitTimeZone) {
            throw BadDateFormatError(u8"Not an ISO 8601 time, invalid time zone syntax");
          }
        } else {
          throw BadDateFormatError(u8"Not an ISO 8601 time, invalid time zone syntax");
        }
      } // it's not a single 'Z' character
    } // more characters follow after time

    return (hour * 3600 + minute * 60 + second) * TicksPerSecond;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  std::int64_t Iso8601Converter::ParseIso8601DateTime(const std::u8string_view &value) {
    std::u8string_view trimmed = Nuclex::Support::Text::StringHelper::GetTrimmed(value);

    std::u8string_view::size_type index = 0;

    // Parse the date portion of the ISO 8601 date with optional time.
    // The index is passed by reference and advanced to one character past the date.
    std::int64_t ticks = parseDate(trimmed, index);
    if(index == trimmed.length()) {
      return ticks; // No time portion appended
    }

    // Now we need to determine whether the date was specified in the extended format
    // with dashes. Allow for additional characters due to the possibility of finding
    // and appended time zone (which we will ignore but nevertheless validate).
    bool isExtendedFormatDate = (index >= 9);
    bool hasTimeAppended = (
      (isExtendedFormatDate && (trimmed.length() >= (index + 9))) ||
      (!isExtendedFormatDate && (trimmed.length() >= (index + 7)))
    );
    if(!hasTimeAppended) {
      throw Errors::BadDateFormatError(u8"ISO 8601 date is followed by malformed text");
    }

    // Make sure the date and time are separated by a 'T' or a space character.
    bool correctSeparatorUsed = (
      (trimmed[index] == u8'T') || // ISO 8601
      (trimmed[index] == u8' ')    // RFC 3339 (removed from ISO 8601, but we're lenient)
    );
    if(!correctSeparatorUsed) {
      throw Errors::BadDateFormatError(u8"ISO 8601 date and time must be separated by 'T'");
    }
    ++index;

    // Date portion looks good, try to parse the ISO 8601 time
    return ticks + parseTime(trimmed, index);
  }

  // ------------------------------------------------------------------------------------------- //

  std::int64_t Iso8601Converter::ParseIso8601Time(const std::u8string_view &value) {
    std::u8string_view trimmed = Nuclex::Support::Text::StringHelper::GetTrimmed(value);

    std::u8string_view::size_type index = 0;
    return parseTime(value, index);
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
