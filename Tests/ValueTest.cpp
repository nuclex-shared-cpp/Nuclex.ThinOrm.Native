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

  TEST(ValueTest, CanBeConstructedFromBoolean) {
    Value v(false);
    EXPECT_EQ(v.GetType(), ValueType::Boolean);
    EXPECT_EQ(v.AsBool(), false);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromUInt8) {
    Value v(std::uint8_t(234));
    EXPECT_EQ(v.GetType(), ValueType::UInt8);
    EXPECT_EQ(v.AsUInt8(), std::uint8_t(234));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromInt16) {
    Value v(std::int16_t(-23456));
    EXPECT_EQ(v.GetType(), ValueType::Int16);
    EXPECT_EQ(v.AsInt16(), std::int16_t(-23456));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromInt32) {
    Value v(std::int32_t(-1234567890));
    EXPECT_EQ(v.GetType(), ValueType::Int32);
    EXPECT_EQ(v.AsInt32(), std::int32_t(-1234567890));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromInt64) {
    Value v(std::int64_t(-1234567890123456789LL));
    EXPECT_EQ(v.GetType(), ValueType::Int64);
    EXPECT_EQ(v.AsInt64(), std::int64_t(-1234567890123456789LL));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromDecimal) {
    Value v(Decimal(123456789));
    EXPECT_EQ(v.GetType(), ValueType::Decimal);
    //EXPECT_EQ(v.AsDecimal(), Decimal(123456789)); // TODO: Decimal needs operator ==()
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromFloat) {
    Value v(1234.5678f);
    EXPECT_EQ(v.GetType(), ValueType::Float);
    EXPECT_EQ(v.AsFloat(), 1234.5678f);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromDouble) {
    Value v(1234.5678);
    EXPECT_EQ(v.GetType(), ValueType::Double);
    EXPECT_EQ(v.AsDouble(), 1234.5678);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromU8String) {
    Value v(std::u8string(u8"Hello World"));
    EXPECT_EQ(v.GetType(), ValueType::String);
    EXPECT_EQ(v.AsString(), std::u8string(u8"Hello World"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeConstructedFromBlob) {
    Value v(std::vector<std::byte> { std::byte(1), std::byte(91), std::byte(191) });
    EXPECT_EQ(v.GetType(), ValueType::Blob);
    std::vector<std::byte> actual = v.AsBlob().value();
    ASSERT_EQ(actual.size(), 3U);
    EXPECT_EQ(actual.at(0), std::byte(1));
    EXPECT_EQ(actual.at(1), std::byte(91));
    EXPECT_EQ(actual.at(2), std::byte(191));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CanBeEmpty) {
    std::optional<std::int32_t> emptyInt;

    Value emptyValue(emptyInt);
    EXPECT_TRUE(emptyValue.IsEmpty());

    Value filledValue(std::int32_t(123456789));
    EXPECT_FALSE(filledValue.IsEmpty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, EmptyValueRetainsType) {
    std::optional<std::int32_t> emptyInt;
    Value emptyValue(emptyInt);

    EXPECT_TRUE(emptyValue.IsEmpty());
    EXPECT_EQ(emptyValue.GetType(), ValueType::Int32);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, SpecificValueTypeCanBeRequired) {
    Value v(1234.5678f);

    EXPECT_NO_THROW(
      v.Require(ValueType::Float);
    );
    EXPECT_THROW(
      v.Require(ValueType::String),
      Nuclex::ThinOrm::Errors::BadValueTypeError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, ValuePresenceCanBeRequired) {
    Value v(1234.5678f);

    EXPECT_NO_THROW(
      v.Require(ValueType::Float, true);
    );
    EXPECT_THROW(
      v.Require(ValueType::Float, false),
      Nuclex::ThinOrm::Errors::BadValueTypeError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, ValueAbsenceCanBeRequired) {
    std::optional<float> emptyFloat;
    Value v(emptyFloat);

    EXPECT_NO_THROW(
      v.Require(ValueType::Float, false);
    );
    EXPECT_THROW(
      v.Require(ValueType::Float, true),
      Nuclex::ThinOrm::Errors::BadValueTypeError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTest, CertainStringsCoerceToBooleanTrues) {
    Value arbitraryStringValue(std::u8string(u8"Hello World"));
    Value zeroStringValue(std::u8string(u8"0.0"));

    EXPECT_EQ(arbitraryStringValue.AsBool(), false);
    EXPECT_EQ(zeroStringValue.AsBool(), false);

    Value numberStringValue(u8"123.456");
    Value oneStringValue(u8"1");
    Value onStringValue(u8"on");
    Value yesStringValue(u8"yes");
    Value trueStringValue(u8"true");
    Value enabledStringValue(u8"enabled");
    Value activeStringValue(u8"active");

    EXPECT_EQ(numberStringValue.AsBool(), true);
    EXPECT_EQ(oneStringValue.AsBool(), true);
    EXPECT_EQ(onStringValue.AsBool(), true);
    EXPECT_EQ(yesStringValue.AsBool(), true);
    EXPECT_EQ(trueStringValue.AsBool(), true);
    EXPECT_EQ(enabledStringValue.AsBool(), true);
    EXPECT_EQ(activeStringValue.AsBool(), true);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
