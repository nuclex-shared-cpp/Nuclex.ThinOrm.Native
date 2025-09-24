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

  TEST(ValueTypeTest, CanBeConvertedToString) {
    EXPECT_EQ(StringFromValueType(ValueType::Boolean), std::u8string(u8"boolean"));
    EXPECT_EQ(StringFromValueType(ValueType::UInt8), std::u8string(u8"uint8"));
    EXPECT_EQ(StringFromValueType(ValueType::Int16), std::u8string(u8"int16"));
    EXPECT_EQ(StringFromValueType(ValueType::Int32), std::u8string(u8"int32"));
    EXPECT_EQ(StringFromValueType(ValueType::Int64), std::u8string(u8"int64"));
    EXPECT_EQ(StringFromValueType(ValueType::Decimal), std::u8string(u8"decimal"));
    EXPECT_EQ(StringFromValueType(ValueType::Float), std::u8string(u8"float"));
    EXPECT_EQ(StringFromValueType(ValueType::Double), std::u8string(u8"double"));
    EXPECT_EQ(StringFromValueType(ValueType::String), std::u8string(u8"string"));
    EXPECT_EQ(StringFromValueType(ValueType::Date), std::u8string(u8"date"));
    EXPECT_EQ(StringFromValueType(ValueType::Time), std::u8string(u8"time"));
    EXPECT_EQ(StringFromValueType(ValueType::DateTime), std::u8string(u8"datetime"));
    EXPECT_EQ(StringFromValueType(ValueType::Blob), std::u8string(u8"blob"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ValueTypeTest, CanBeRestoredFromString) {
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"boolean")), ValueType::Boolean);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"uint8")), ValueType::UInt8);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"int16")), ValueType::Int16);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"int32")), ValueType::Int32);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"int64")), ValueType::Int64);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"decimal")), ValueType::Decimal);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"float")), ValueType::Float);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"double")), ValueType::Double);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"string")), ValueType::String);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"date")), ValueType::Date);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"time")), ValueType::Time);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"datetime")), ValueType::DateTime);
    EXPECT_EQ(ValueTypeFromString(std::u8string(u8"blob")), ValueType::Blob);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
