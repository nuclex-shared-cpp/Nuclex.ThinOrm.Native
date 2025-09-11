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

#ifndef NUCLEX_THINORM_DATETIME_H
#define NUCLEX_THINORM_DATETIME_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <cstdint> // for std::uint16_t, std::int16_t
#include <ctime> // for std::tm

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Date and/or time with fractional seconds</summary>
  class NUCLEX_THINORM_TYPE DateTime {

    /// <summary>Initializes a date and time value by copying an existing value</summary>
    /// <param name="other">Existing date and time value that will be copied</param>
    public: NUCLEX_THINORM_API DateTime(const DateTime &other);

    /// <summary>Initializes a date and time value by adopting an existing value</summary>
    /// <param name="other">Existing date and time value that will be taken over</param>
    public: NUCLEX_THINORM_API DateTime(DateTime &&other);

    /// <summary>Initializes a date and time value from a tick count</summary>
    /// <param name="ticks">
    ///   Ticks that indicate the date and time with microsecond accuracy
    /// </param>
    /// <remarks>
    ///   Ticks are the internal format of a date and time value, counting from midnight
    ///   of the first day in year 1 by steps of one tenth of a microsecond. This format is
    ///   inspired by .NET's DateTime structure which uses the same unit for its internal
    ///   ticks, because it is an established standard and because it has worked well.
    /// </remarks>
    public: NUCLEX_THINORM_API explicit DateTime(const std::int64_t ticks);

    /// <summary>
    ///   Initializes a date and time value from a <see cref="std::tm" /> structure
    /// </summary>
    /// <param name="dateAndTime">Date and time in &quot;broken down&quot; format</param>
    /// <param name="tenthMicroseconds">
    ///   Number of tenths of microseconds after the time indicated
    /// </param>
    public: NUCLEX_THINORM_API static DateTime FromTm(
      const std::tm &dateAndTime, std::uint32_t tenthMicroseconds = 0
    );

    /// <summary>
    ///   Initializes a date and time value from a number of seconds after the Unix epoch
    /// </summary>
    /// <param name="secondsSinceUnixEpoch">Number of seconds after the Unix epoch</param>
    /// <param name="tenthMicroseconds">
    ///   Number of tenths of microseconds after the time indicated
    /// </param>
    public: NUCLEX_THINORM_API static DateTime FromSecondsSinceUnixEpoch(
      std::time_t secondsSinceUnixEpoch, std::uint32_t tenthMicrosseconds = 0
    );

    /// <summary>
    ///   Parses a date and time value from an ISO 8601 date (with optional time)
    /// </summary>
    /// <param name="iso8601Date">Date (and optionally time) in ISO 8601 format</param>
    /// <returns>The date and time value parsed from the ISO 8601 date</returns>
    public: NUCLEX_THINORM_API static DateTime ParseIso8601DateTime(
      const std::u8string &iso8601Date
    );

    /// <summary>Parses a date and time value from an ISO 8601 time</summary>
    /// <param name="iso8601Date">Time in ISO 8601 format</param>
    /// <returns>The date and time value parsed from the ISO 8601 time</returns>
    public: NUCLEX_THINORM_API static DateTime ParseIso8601Time(
      const std::u8string &iso8601Time
    );

    /// <summary>Converts the date and time value to an ISO 8601 date and time</summary>
    /// <returns>A string containing the date and time in the ISO 8601 date format</returns>
    public: NUCLEX_THINORM_API DateTime GetDateOnly() const;

    /// <summary>Converts the date and time value to an ISO 8601 time</summary>
    /// <returns>A string containing the time in the ISO 8601 time format</returns>
    public: NUCLEX_THINORM_API DateTime GetTimeOnly() const;

    /// <summary>Converts the date and time value to an ISO 8601 date and time</summary>
    /// <returns>A string containing the date and time in the ISO 8601 date format</returns>
    public: NUCLEX_THINORM_API std::u8string ToIso8601DateTime() const;

    /// <summary>Converts the date and time value to an ISO 8601 time</summary>
    /// <returns>A string containing the time in the ISO 8601 time format</returns>
    public: NUCLEX_THINORM_API std::u8string ToIso8601Time() const;

    /// <summary>
    ///   Converts the date and time value into a Posix &quot;broken down&quot; time structure
    /// </summary>
    /// <returns>The date and time in a Posix &quot;broken down&quot; time structure</returns>
    public: NUCLEX_THINORM_API std::tm ToTm() const;

    /// <summary>
    ///  Converts the date and time value into the number of seconds since the Unix epoch
    /// </summary>
    /// <returns>The date and time as the number of seconds since the Unix epoch</returns>
    public: NUCLEX_THINORM_API std::time_t ToSecondSinceUnixEpoch() const;

    /// <summary>Returns the fraction of a second past the date and time</summary>
    /// <returns>The fraction of a second in the date and time value</returns>
    public: NUCLEX_THINORM_API float GetSecondFraction() const;

    /// <summary>Returns the number of one tenth microseconds since the year 1</summary>
    /// <returns>The date and time as the number of one tenths of microseconds</returns>
    public: NUCLEX_THINORM_API inline std::int64_t GetTicks() const;

    /// <summary>Clones the date and time assumed by another instance</summary>
    /// <param name="other">Other instance whose contents will be cloned</param>
    public: NUCLEX_THINORM_API DateTime &operator =(const DateTime &other) noexcept;
    /// <summary>Adopts the date and time from another instance</summary>
    /// <param name="other">Other instance whose contents will be adopted</param>
    public: NUCLEX_THINORM_API DateTime &operator =(DateTime &&other) noexcept;

    /// <summary>Microseconds since the midnight on the first day of year 1</summary>
    private: std::int64_t ticks;

  };

  // ------------------------------------------------------------------------------------------- //

  inline std::int64_t DateTime::GetTicks() const {
    return this->ticks;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_DATETIME_H
