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
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, CanBeInitializedFromTicks) {
    DateTime test(1234567890123456789ll);
    EXPECT_EQ(test.GetTicks(), 1234567890123456789ll);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(DateTimeTest, CanQueryCurrentDateAndTime) {
    DateTime now = DateTime::Now();

    std::time_t secondsSinceUnixEpochNow = std::time(nullptr);
    std::time_t secondsSinceUnixEpochInDateTime = now.ToSecondSinceUnixEpoch();

    // Even the slowest, laggiest system should manage to execute the unit test
    // without 5 seconds time :)
    EXPECT_GT(secondsSinceUnixEpochInDateTime, secondsSinceUnixEpochNow - 1);
    EXPECT_LT(secondsSinceUnixEpochInDateTime, secondsSinceUnixEpochNow + 4);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
