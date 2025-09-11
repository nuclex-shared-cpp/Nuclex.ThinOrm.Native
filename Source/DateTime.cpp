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

namespace {

  // ------------------------------------------------------------------------------------------- //
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

  DateTime DateTime::FromTm(
    const std::tm &dateAndTime, std::uint32_t tenthMicroseconds /* = 0 */
  ) {
    return DateTime(0);
    // TODO: Initialize a DateTime value from std::tm
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::FromSecondsSinceUnixEpoch(
    std::time_t secondsSinceUnixEpoch, std::uint32_t tenthMicrosseconds /* = 0 */
  ) {
    return DateTime(0);
    // TODO: Initialize a DateTime vlaue from std::time_t
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
    return DateTime(this->ticks - (this->ticks % 864'000'000'000ll));
  }

  // ------------------------------------------------------------------------------------------- //

  DateTime DateTime::GetTimeOnly() const {
    return DateTime(this->ticks % 864'000'000'000ll);
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
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  float DateTime::GetSecondFraction() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
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
