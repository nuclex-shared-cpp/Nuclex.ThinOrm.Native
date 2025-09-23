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

#include "../../Source/Utilities/Quantizer.h" // for Quantizewr

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  TEST(QuantizerTest, FloatsCanBeRoundedToInts) {
    EXPECT_EQ(Quantizer::NearestInt32(2.0f), 2);
    EXPECT_EQ(Quantizer::NearestInt32(2.3f), 2);
    EXPECT_EQ(Quantizer::NearestInt32(2.5f), 3); // !! Ordinarily depends on rounding mode
    EXPECT_EQ(Quantizer::NearestInt32(2.7f), 3);

    EXPECT_EQ(Quantizer::NearestInt32(-2.0f), -2);
    EXPECT_EQ(Quantizer::NearestInt32(-2.3f), -2);
    EXPECT_EQ(Quantizer::NearestInt32(-2.5f), -3); // !! Ordinarily depends on rounding mode
    EXPECT_EQ(Quantizer::NearestInt32(-2.7f), -3);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QuantizerTest, DoublesCanBeRoundedToInts) {
    EXPECT_EQ(Quantizer::NearestInt32(2.0), 2);
    EXPECT_EQ(Quantizer::NearestInt32(2.3), 2);
    EXPECT_EQ(Quantizer::NearestInt32(2.5), 3); // !! Ordinarily depends on rounding mode
    EXPECT_EQ(Quantizer::NearestInt32(2.7), 3);

    EXPECT_EQ(Quantizer::NearestInt32(-2.0), -2);
    EXPECT_EQ(Quantizer::NearestInt32(-2.3), -2);
    EXPECT_EQ(Quantizer::NearestInt32(-2.5), -3); // !! Ordinarily depends on rounding mode
    EXPECT_EQ(Quantizer::NearestInt32(-2.7), -3);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities
