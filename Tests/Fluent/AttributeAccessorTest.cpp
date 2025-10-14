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

  class TestEntity {

    public: int Id;
    public: std::u8string Name;
    public: std::optional<std::u8string> PasswordHash;

  };

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Fluent {

  // ------------------------------------------------------------------------------------------- //

  TEST(AttributeAccessorTest, CanReadAttributes) {
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

  TEST(AttributeAccessorTest, CanWriteAttributes) {
    TestEntity t = {};
    Value v(123);

    AttributeAccessor<&TestEntity::Id>::Set(static_cast<void *>(&t), v);

    EXPECT_EQ(t.Id, 123);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Fluent
