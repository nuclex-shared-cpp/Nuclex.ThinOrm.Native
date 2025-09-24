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

#include "Nuclex/ThinOrm/Value.h"
#include "Nuclex/ThinOrm/Errors/BadValueTypeError.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, BooleansCanBeCoercedToOtherTypes) {
    Value falseValue(false);
    Value trueValue(true);

    EXPECT_EQ(falseValue.AsBool(), false);
    EXPECT_EQ(trueValue.AsBool(), true);
    EXPECT_EQ(falseValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(trueValue.AsUInt8(), std::uint8_t(1));
    EXPECT_EQ(falseValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(trueValue.AsInt16(), std::int16_t(1));
    EXPECT_EQ(falseValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(trueValue.AsInt32(), std::int32_t(1));
    EXPECT_EQ(falseValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(trueValue.AsInt64(), std::int64_t(1));
    //EXPECT_EQ(falseValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(trueValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(falseValue.AsFloat(), 0.0f);
    EXPECT_EQ(trueValue.AsFloat(), 1.0f);
    EXPECT_EQ(falseValue.AsDouble(), 0.0);
    EXPECT_EQ(trueValue.AsDouble(), 1.0);
    EXPECT_EQ(falseValue.AsString(), std::u8string(u8"false"));
    EXPECT_EQ(trueValue.AsString(), std::u8string(u8"true"));

    std::optional<std::vector<std::byte>> falseBlob = falseValue.AsBlob();
    std::optional<std::vector<std::byte>> trueBlob = trueValue.AsBlob();

    ASSERT_EQ(falseBlob->size(), 1U);
    EXPECT_EQ(falseBlob->at(0), std::byte(0));
    ASSERT_EQ(trueBlob->size(), 1U);
    EXPECT_EQ(trueBlob->at(0), std::byte(1));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, UInt8sCanBeCoercedToOtherTypes) {
    Value zeroValue(std::uint8_t(0));
    Value testValue(std::uint8_t(234));

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(234));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(234));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(234));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(234));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), 234.0f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), 234.0);
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"234"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 1U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    ASSERT_EQ(testBlob->size(), 1U);
    EXPECT_EQ(testBlob->at(0), std::byte(234));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, Int16sCanBeCoercedToOtherTypes) {
    Value zeroValue(std::int16_t(0));
    Value testValue(std::int16_t(-23456));

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(96));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(-23456));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(-23456));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(-23456));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), -23456.0f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), -23456.0);
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"-23456"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 2U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    EXPECT_EQ(zeroBlob->at(1), std::byte(0));
    ASSERT_EQ(testBlob->size(), 2U);
    EXPECT_EQ(testBlob->at(0), std::byte(0x60));
    EXPECT_EQ(testBlob->at(1), std::byte(0xa4));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, Int32sCanBeCoercedToOtherTypes) {
    Value zeroValue(std::int32_t(0));
    Value testValue(std::int32_t(-1234567890));

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(46));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(-722));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(-1234567890));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(-1234567890));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), -1234567890.0f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), -1234567890.0);
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"-1234567890"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 4U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    EXPECT_EQ(zeroBlob->at(1), std::byte(0));
    EXPECT_EQ(zeroBlob->at(2), std::byte(0));
    EXPECT_EQ(zeroBlob->at(3), std::byte(0));
    ASSERT_EQ(testBlob->size(), 4U);
    EXPECT_EQ(testBlob->at(0), std::byte(0x2e));
    EXPECT_EQ(testBlob->at(1), std::byte(0xfd));
    EXPECT_EQ(testBlob->at(2), std::byte(0x69));
    EXPECT_EQ(testBlob->at(3), std::byte(0xb6));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, Int64sCanBeCoercedToOtherTypes) {
    Value zeroValue(std::int64_t(0));
    Value testValue(std::int64_t(-1234567890123456789LL));

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(235));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(32491));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(-2112454933));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(-1234567890123456789LL));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), -1234567890123456789.0f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), -1234567890123456789.0);
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"-1234567890123456789"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 8U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    EXPECT_EQ(zeroBlob->at(1), std::byte(0));
    EXPECT_EQ(zeroBlob->at(2), std::byte(0));
    EXPECT_EQ(zeroBlob->at(3), std::byte(0));
    EXPECT_EQ(zeroBlob->at(4), std::byte(0));
    EXPECT_EQ(zeroBlob->at(5), std::byte(0));
    EXPECT_EQ(zeroBlob->at(6), std::byte(0));
    EXPECT_EQ(zeroBlob->at(7), std::byte(0));
    ASSERT_EQ(testBlob->size(), 8U);
    EXPECT_EQ(testBlob->at(0), std::byte(0xeb));
    EXPECT_EQ(testBlob->at(1), std::byte(0x7e));
    EXPECT_EQ(testBlob->at(2), std::byte(0x16));
    EXPECT_EQ(testBlob->at(3), std::byte(0x82));
    EXPECT_EQ(testBlob->at(4), std::byte(0x0b));
    EXPECT_EQ(testBlob->at(5), std::byte(0xef));
    EXPECT_EQ(testBlob->at(6), std::byte(0xdd));
    EXPECT_EQ(testBlob->at(7), std::byte(0xee));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, DecimalsCanBeCoercedToOtherTypes) {
    Value zeroValue(std::int64_t(0));
    Value testValue(std::int64_t(-1234567890123456789LL));
    // TODO: Implement decimal coercion unit test once decimals are done
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, FloatsCanBeCoercedToOtherTypes) {
    Value zeroValue(0.0f);
    Value testValue(234.567f);

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(235));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(235));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(235));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(235));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), 234.567f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), double(234.567f));
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0.0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"234.567"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 4U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    EXPECT_EQ(zeroBlob->at(1), std::byte(0));
    EXPECT_EQ(zeroBlob->at(2), std::byte(0));
    EXPECT_EQ(zeroBlob->at(3), std::byte(0));
    ASSERT_EQ(testBlob->size(), 4U);
    EXPECT_EQ(testBlob->at(0), std::byte(0x27));
    EXPECT_EQ(testBlob->at(1), std::byte(0x91));
    EXPECT_EQ(testBlob->at(2), std::byte(0x6a));
    EXPECT_EQ(testBlob->at(3), std::byte(0x43));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, DoublesCanBeCoercedToOtherTypes) {
    Value zeroValue(0.0);
    Value testValue(12345.56789);

    EXPECT_EQ(zeroValue.AsBool(), false);
    EXPECT_EQ(testValue.AsBool(), true);
    EXPECT_EQ(zeroValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(testValue.AsUInt8(), std::uint8_t(58));
    EXPECT_EQ(zeroValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(testValue.AsInt16(), std::int16_t(12346));
    EXPECT_EQ(zeroValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(testValue.AsInt32(), std::int32_t(12346));
    EXPECT_EQ(zeroValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(testValue.AsInt64(), std::int64_t(12346));
    //EXPECT_EQ(zeroValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(testValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(zeroValue.AsFloat(), 0.0f);
    EXPECT_EQ(testValue.AsFloat(), 12345.56789f);
    EXPECT_EQ(zeroValue.AsDouble(), 0.0);
    EXPECT_EQ(testValue.AsDouble(), 12345.56789);
    EXPECT_EQ(zeroValue.AsString(), std::u8string(u8"0.0"));
    EXPECT_EQ(testValue.AsString(), std::u8string(u8"12345.56789"));

    std::optional<std::vector<std::byte>> zeroBlob = zeroValue.AsBlob();
    std::optional<std::vector<std::byte>> testBlob = testValue.AsBlob();

    ASSERT_EQ(zeroBlob->size(), 8U);
    EXPECT_EQ(zeroBlob->at(0), std::byte(0));
    EXPECT_EQ(zeroBlob->at(1), std::byte(0));
    EXPECT_EQ(zeroBlob->at(2), std::byte(0));
    EXPECT_EQ(zeroBlob->at(3), std::byte(0));
    EXPECT_EQ(zeroBlob->at(4), std::byte(0));
    EXPECT_EQ(zeroBlob->at(5), std::byte(0));
    EXPECT_EQ(zeroBlob->at(6), std::byte(0));
    EXPECT_EQ(zeroBlob->at(7), std::byte(0));
    ASSERT_EQ(testBlob->size(), 8U);
    EXPECT_EQ(testBlob->at(0), std::byte(0xdd));
    EXPECT_EQ(testBlob->at(1), std::byte(0x98));
    EXPECT_EQ(testBlob->at(2), std::byte(0x9e));
    EXPECT_EQ(testBlob->at(3), std::byte(0xb0));
    EXPECT_EQ(testBlob->at(4), std::byte(0xc8));
    EXPECT_EQ(testBlob->at(5), std::byte(0x1c));
    EXPECT_EQ(testBlob->at(6), std::byte(0xc8));
    EXPECT_EQ(testBlob->at(7), std::byte(0x40));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, StringsCanBeCoercedToOtherTypes) {
    Value textValue(std::u8string(u8"This is not a number", 20));
    Value numberValue(std::u8string(u8"432.654", 7));

    EXPECT_EQ(textValue.AsBool(), false);
    EXPECT_EQ(numberValue.AsBool(), true);
    EXPECT_EQ(textValue.AsUInt8(), std::uint8_t(0));
    EXPECT_EQ(numberValue.AsUInt8(), std::uint8_t(177));
    EXPECT_EQ(textValue.AsInt16(), std::int16_t(0));
    EXPECT_EQ(numberValue.AsInt16(), std::int16_t(433));
    EXPECT_EQ(textValue.AsInt32(), std::int32_t(0));
    EXPECT_EQ(numberValue.AsInt32(), std::int32_t(433));
    EXPECT_EQ(textValue.AsInt64(), std::int64_t(0));
    EXPECT_EQ(numberValue.AsInt64(), std::int64_t(433));
    //EXPECT_EQ(textValue.AsDecimal(), Decimal(0)); // TODO: Implement decimal conversion
    //EXPECT_EQ(numberValue.AsDecimal(), Decimal(1)); // TODO: Implement decimal conversion
    EXPECT_EQ(textValue.AsFloat(), 0.0f);
    EXPECT_EQ(numberValue.AsFloat(), 432.654f);
    EXPECT_EQ(textValue.AsDouble(), 0.0);
    EXPECT_EQ(numberValue.AsDouble(), 432.654);
    EXPECT_EQ(textValue.AsString(), std::u8string(u8"This is not a number"));
    EXPECT_EQ(numberValue.AsString(), std::u8string(u8"432.654"));

    std::optional<std::vector<std::byte>> textBlob = textValue.AsBlob();
    std::optional<std::vector<std::byte>> numberBlob = numberValue.AsBlob();

    ASSERT_EQ(textBlob->size(), 20U);
    EXPECT_EQ(textBlob->at(0), std::byte(u8'T'));
    EXPECT_EQ(textBlob->at(1), std::byte(u8'h'));
    EXPECT_EQ(textBlob->at(2), std::byte(u8'i'));
    EXPECT_EQ(textBlob->at(3), std::byte(u8's'));
    EXPECT_EQ(textBlob->at(4), std::byte(u8' '));
    EXPECT_EQ(textBlob->at(5), std::byte(u8'i'));
    EXPECT_EQ(textBlob->at(6), std::byte(u8's'));
    EXPECT_EQ(textBlob->at(7), std::byte(u8' '));
    EXPECT_EQ(textBlob->at(8), std::byte(u8'n'));
    EXPECT_EQ(textBlob->at(9), std::byte(u8'o'));
    EXPECT_EQ(textBlob->at(10), std::byte(u8't'));
    EXPECT_EQ(textBlob->at(11), std::byte(u8' '));
    EXPECT_EQ(textBlob->at(12), std::byte(u8'a'));
    EXPECT_EQ(textBlob->at(13), std::byte(u8' '));
    EXPECT_EQ(textBlob->at(14), std::byte(u8'n'));
    EXPECT_EQ(textBlob->at(15), std::byte(u8'u'));
    EXPECT_EQ(textBlob->at(16), std::byte(u8'm'));
    EXPECT_EQ(textBlob->at(17), std::byte(u8'b'));
    EXPECT_EQ(textBlob->at(18), std::byte(u8'e'));
    EXPECT_EQ(textBlob->at(19), std::byte(u8'r'));
    ASSERT_EQ(numberBlob->size(), 7U);
    EXPECT_EQ(numberBlob->at(0), std::byte(0x34));
    EXPECT_EQ(numberBlob->at(1), std::byte(0x33));
    EXPECT_EQ(numberBlob->at(2), std::byte(0x32));
    EXPECT_EQ(numberBlob->at(3), std::byte(0x2e));
    EXPECT_EQ(numberBlob->at(4), std::byte(0x36));
    EXPECT_EQ(numberBlob->at(5), std::byte(0x35));
    EXPECT_EQ(numberBlob->at(6), std::byte(0x34));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
