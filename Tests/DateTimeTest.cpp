#pragma region Apache License 2.0
/*
Nuclex Native Framework
Copyright (C) 2002-2025 Markus Ewald / Nuclex Development Labs

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

#include <gtest/gtest.h>

#include "Nuclex/ThinOrm/DateTime.h"

#include <ctime> // for std::time()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Returns the date and time of Carl Sagan's birth as an std::tm</summary>
  /// <returns>The date and time of Carl Sagan's birth as an std::tm value</returns>
  inline std::tm getCarlSaganBirthDateAsTm() {
    std::tm result = {};

    result.tm_year = 34;
    result.tm_mon = 10;
    result.tm_mday = 9;
    result.tm_wday = 5;
    result.tm_yday = 312;
    result.tm_hour = 17;
    result.tm_min = 5;
    result.tm_sec = 30;

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Tick count of carl sagan's birth date</summary>
  constexpr std::int64_t CarlSagaBirthDateTicks = 610'265'415'300'000'000ll;

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, CanBeInitializedFromTicks) {
    DateTime test(1'234'567'890'123'456'789ll);
    EXPECT_EQ(test.GetTicks(), 1'234'567'890'123'456'789ll);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, CurrentDateAndTimeCanBeQueried) {
    DateTime now = DateTime::Now();

    std::time_t secondsSinceUnixEpochNow = std::time(nullptr);
    std::time_t secondsSinceUnixEpochInDateTime = now.ToSecondSinceUnixEpoch();

    // Even the slowest, laggiest system should manage to execute the unit test
    // without 5 seconds time :)
    EXPECT_GT(secondsSinceUnixEpochInDateTime, secondsSinceUnixEpochNow - 1);
    EXPECT_LT(secondsSinceUnixEpochInDateTime, secondsSinceUnixEpochNow + 4);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, TmIsConvertibleToDateTime) {
    std::tm carlSaganBirthDateTm = getCarlSaganBirthDateAsTm();

    DateTime birthDate = DateTime::FromTm(carlSaganBirthDateTm, 1'234'567);
    EXPECT_EQ(610'265'415'301'234'567ll, birthDate.GetTicks());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, IsConvertibletoTm) {
    DateTime carlSaganBirthDate(CarlSagaBirthDateTicks);

    std::tm carlSaganBirthDateTm = carlSaganBirthDate.ToTm();
    EXPECT_EQ(carlSaganBirthDateTm.tm_year, 34);
    EXPECT_EQ(carlSaganBirthDateTm.tm_mon, 10);
    EXPECT_EQ(carlSaganBirthDateTm.tm_mday, 9);
    EXPECT_EQ(carlSaganBirthDateTm.tm_wday, 5);
    EXPECT_EQ(carlSaganBirthDateTm.tm_yday, 312);
    EXPECT_EQ(carlSaganBirthDateTm.tm_hour, 17);
    EXPECT_EQ(carlSaganBirthDateTm.tm_min, 5);
    EXPECT_EQ(carlSaganBirthDateTm.tm_sec, 30);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, PrintsAsIso8601DateTime) {
    DateTime carlSaganBirthDate(CarlSagaBirthDateTicks);

    std::u8string iso8601 = carlSaganBirthDate.ToIso8601DateTime();
    EXPECT_EQ(iso8601, std::u8string(u8"1934-11-09T17:05:30"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, PrintsAsIso8601Date) {
    DateTime carlSaganBirthDate(CarlSagaBirthDateTicks);

    std::u8string iso8601 = carlSaganBirthDate.ToIso8601Date();
    EXPECT_EQ(iso8601, std::u8string(u8"1934-11-09"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, PrintsAsIso8601Time) {
    DateTime carlSaganBirthDate(CarlSagaBirthDateTicks);

    std::u8string iso8601 = carlSaganBirthDate.ToIso8601Time();
    EXPECT_EQ(iso8601, std::u8string(u8"17:05:30"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
