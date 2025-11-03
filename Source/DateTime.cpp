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

#include "Nuclex/ThinOrm/DateTime.h"

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()

#include "./Utilities/Iso8601Converter.h" // for Iso8601Converter

#include <cassert> // for assert()
#include <stdexcept> // for std::runtime_error()
#include <chrono> // for std::chrono::system_clock

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

  /// <summary>Number of 1/10th microseconds on midnight of January the 1st in 1900</summary>
  constexpr const std::int64_t TicksAtTmMin = 693'595ll * TicksPerDay;

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Ticks time unit</summary>
  typedef std::chrono::duration<std::int64_t, std::ratio<1, TicksPerSecond>> Ticks;

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  DateTime::DateTime(const DateTime &other) :
    ticks(other.ticks) {}

  // ------------------------------------------------------------------------------------------- //

  DateTime::DateTime(DateTime &&other) :
    ticks(other.ticks) {} // no point "destroying" other

  // ------------------------------------------------------------------------------------------- //

  DateTime::DateTime(const std::int64_t ticks) :
    ticks(ticks) {}

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::Now() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::int64_t microsecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    return DateTime(microsecondsSinceUnixEpoch * TicksPerMicrosecond + TicksAtStartOfUnixEpoch);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::Today() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::int64_t microsecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    microsecondsSinceUnixEpoch -= (microsecondsSinceUnixEpoch % TicksPerDay);
    return DateTime(microsecondsSinceUnixEpoch * TicksPerMicrosecond + TicksAtStartOfUnixEpoch);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::TimeOfDay() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::int64_t microsecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    return DateTime(
      microsecondsSinceUnixEpoch * TicksPerMicrosecond % TicksPerDay + TicksAtStartOfUnixEpoch
    );
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::FromTm(
    const std::tm &dateAndTime, std::uint32_t tenthMicroseconds /* = 0 */
  ) {

    // Form the tick count incrementally. First, calculate the number of ticks
    // for the given date. This is the complicated part because it involves leap years.
    std::int64_t ticks;
    {
      std::chrono::year_month_day chronoDate(
        std::chrono::year(dateAndTime.tm_year + 1900),
        std::chrono::month(dateAndTime.tm_mon + 1),
        std::chrono::day(dateAndTime.tm_mday)
      );
      if(!chronoDate.ok()) {
        throw std::invalid_argument(
          reinterpret_cast<const char *>(u8"std::tm specified an invalid calendar date")
        );
      }

      ticks = TicksAtStartOfUnixEpoch;
      ticks += std::chrono::time_point_cast<Ticks>(
        std::chrono::sys_days(chronoDate)
      ).time_since_epoch().count();
    }

    // Add the hours, minutes and seconds directly.
    //
    // - Leap seconds before this day should have been handled by the chrono::sys_days
    //   to tick conversion above.
    // - Leap seconds on this day would be handled (should the call happen at exactly
    //   that moment) here (tm_sec would be 60)
    //
    // This is correct because leap seconds are added at the end of the day, so we'll
    // not be skipping a second by assuming 60 seconds per minute and 3600 seconds per hour.
    ticks += dateAndTime.tm_hour * 60 * 60 * TicksPerSecond;
    ticks += dateAndTime.tm_min * 60 * TicksPerSecond;
    ticks += dateAndTime.tm_sec * TicksPerSecond;
    ticks += tenthMicroseconds;

    return DateTime(ticks);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::FromSecondsSinceUnixEpoch(
    std::time_t secondsSinceUnixEpoch, std::uint32_t tenthMicroseconds /* = 0 */
  ) {
    std::int64_t ticks = secondsSinceUnixEpoch * TicksPerSecond + TicksAtStartOfUnixEpoch;
    return DateTime(ticks + tenthMicroseconds);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::ParseIso8601DateTime(const std::u8string_view &iso8601Date) {
    return DateTime(Utilities::Iso8601Converter::ParseIso8601DateTime(iso8601Date));
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::ParseIso8601Time(const std::u8string_view &iso8601Time) {
    return DateTime(Utilities::Iso8601Converter::ParseIso8601Time(iso8601Time));
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::GetDateOnly() const {
    return DateTime(this->ticks - (this->ticks % TicksPerDay));
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::GetTimeOnly() const {
    return DateTime(this->ticks % TicksPerDay);
  }

  // ------------------------------------------------------------------------------------------- //
  
  std::u8string DateTime::ToIso8601Date() const {
    std::u8string result(10, u8'\0');
    Utilities::Iso8601Converter::PrintIso8601Date(result.data(), this->ticks);
    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string DateTime::ToIso8601DateTime() const {
    std::u8string result(19, u8'\0');
    Utilities::Iso8601Converter::PrintIso8601DateTime(result.data(), this->ticks);
    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string DateTime::ToIso8601Time() const {
    std::u8string result(8, u8'\0');
    Utilities::Iso8601Converter::PrintIso8601Time(result.data(), this->ticks);
    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::tm DateTime::ToTm() const {
    constexpr const Ticks unixEpochStart(TicksAtStartOfUnixEpoch);

    if(ticks < TicksAtTmMin) {
      throw std::out_of_range(
        reinterpret_cast<const char *>(u8"Date is not representable as std::tm")
      );
    }

    std::tm result = {};
    {
      std::int64_t secondOfDay = (ticks % TicksPerDay) / TicksPerSecond;
      result.tm_sec = secondOfDay % 60;
      result.tm_min = secondOfDay / 60 % 60;
      result.tm_hour = secondOfDay / 3600;
    }
    {
      std::chrono::days daysFromUnixEpoch(
        (ticks / TicksPerDay) - (TicksAtStartOfUnixEpoch / TicksPerDay)
      );

      std::chrono::sys_days sysDays(daysFromUnixEpoch);
      std::chrono::year_month_day date = sysDays;
      result.tm_mday = static_cast<unsigned>(date.day());
      result.tm_mon = static_cast<unsigned>(date.month()) - 1;
      result.tm_year = static_cast<int>(date.year()) - 1900;

      result.tm_wday = std::chrono::weekday(sysDays).c_encoding();
      result.tm_yday = (sysDays - std::chrono::sys_days{date.year()/1/1}).count();
    }

    result.tm_isdst = -1;

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::time_t DateTime::ToSecondSinceUnixEpoch() const {
    if(this->ticks < TicksAtStartOfUnixEpoch) [[unlikely]] {
      return std::time_t(0);
    }

    return static_cast<std::time_t>(
      (this->ticks - TicksAtStartOfUnixEpoch) / TicksPerSecond
    );
  }

  // ------------------------------------------------------------------------------------------- //

  float DateTime::GetSecondFraction() const {
    std::int64_t tenthMicroseconds = this->ticks % TicksPerSecond;
    return static_cast<float>(static_cast<double>(tenthMicroseconds) / 10'000'000.0);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime &DateTime::operator =(const DateTime &other) noexcept {
    this->ticks = other.ticks;
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime &DateTime::operator =(DateTime &&other) noexcept {
    this->ticks = other.ticks;
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
