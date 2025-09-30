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

#include <cassert> // for assert()
#include <stdexcept> // for std::runtime_error()
#include <chrono> // for std::chrono::system_clock

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Number of 1/10th microseconds that elapse every second</summary>
  std::int64_t TicksPerSecond = 1000 * 1000 * 10;

  /// <summary>Number of 1/10th microseconds that elapse in a single day</summary>
  std::int64_t TicksPerDay = 86400ll * TicksPerSecond;

  /// <summary>Number of 1/10th microseconds on midnight of January the 1st in 1970</summary>
  std::int64_t TicksAtStartOfUnixEpoch = 719162ll * TicksPerDay;

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
    std::int64_t microSecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    return DateTime(microSecondsSinceUnixEpoch * 10ll + TicksAtStartOfUnixEpoch);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::Today() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::int64_t microSecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    microSecondsSinceUnixEpoch -= (microSecondsSinceUnixEpoch % TicksPerDay);
    return DateTime(microSecondsSinceUnixEpoch * 10ll + TicksAtStartOfUnixEpoch);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::TimeOfDay() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::int64_t microSecondsSinceUnixEpoch = (
      std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()
    );

    return DateTime(microSecondsSinceUnixEpoch % TicksPerDay * 10ll + TicksAtStartOfUnixEpoch);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::FromTm(
    const std::tm &dateAndTime, std::uint32_t tenthMicroseconds /* = 0 */
  ) {
    return DateTime(0);
    // TODO: Initialize a DateTime value from std::tm
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::FromSecondsSinceUnixEpoch(
    std::time_t secondsSinceUnixEpoch, std::uint32_t tenthMicroseconds /* = 0 */
  ) {
    std::int64_t ticks = secondsSinceUnixEpoch * TicksPerSecond + TicksAtStartOfUnixEpoch;
    return DateTime(ticks + tenthMicroseconds);
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::ParseIso8601DateTime(const std::u8string &iso8601Date) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::ParseIso8601Time(const std::u8string &iso8601Time) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
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
  
  std::u8string DateTime::ToIso8601DateTime() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string DateTime::ToIso8601Time() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::tm DateTime::ToTm() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
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
