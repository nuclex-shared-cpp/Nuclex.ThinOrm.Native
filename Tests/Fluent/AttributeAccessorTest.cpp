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

#include "Nuclex/ThinOrm/Fluent/AttributeAccessor.h" // for AttributeAccessor

#include <ctime> // for std::time()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Example entity class for testing</summary>
  class TestEntity {

    /// <summary>An integer-typed attribute</summary>
    public: int Id;
    /// <summary>A UTF-8 string attribute</summary>
    public: std::u8string Name;
    /// <summary>An optional UTF-8 string attribute</summary>
    public: std::optional<std::u8string> PasswordHash;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  TEST(AttributeAccessorTest, CanReadIntegerAttributes) {
    TestEntity t;
    t.Id = 123;

    Value v = AttributeAccessor<&TestEntity::Id>::Get(static_cast<void *>(&t));

    bool valueIsInteger16BitsOrMore = (
      (v.GetType() == ValueType::Int16) ||
      (v.GetType() == ValueType::Int32) ||
      (v.GetType() == ValueType::Int64)
    );
    EXPECT_TRUE(valueIsInteger16BitsOrMore);
    EXPECT_EQ(v.AsInt32(), 123);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(AttributeAccessorTest, CanWriteIntegerAttributes) {
    TestEntity t = {};
    Value v(123);

    AttributeAccessor<&TestEntity::Id>::Set(static_cast<void *>(&t), v);

    EXPECT_EQ(t.Id, 123);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(AttributeAccessorTest, CanReadStringAttributes) {
    TestEntity t;
    t.Name = u8"John Doe";

    Value v = AttributeAccessor<&TestEntity::Name>::Get(static_cast<void *>(&t));

    EXPECT_EQ(v.GetType(), ValueType::String);
    EXPECT_EQ(v.AsString(), u8"John Doe");
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(AttributeAccessorTest, CanWriteStringAttributes) {
    TestEntity t = {};
    Value v(std::u8string(u8"Jane Doe"));

    AttributeAccessor<&TestEntity::Name>::Set(static_cast<void *>(&t), v);

    EXPECT_EQ(t.Name, std::u8string(u8"Jane Doe"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent
