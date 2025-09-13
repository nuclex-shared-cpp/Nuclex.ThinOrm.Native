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

#include "Nuclex/ThinOrm/Query.h"
#include "Nuclex/ThinOrm/Value.h"
#include "Nuclex/ThinOrm/Errors/BadParameterNameError.h"
#include "Nuclex/ThinOrm/Errors/UnassignedParameterError.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, SqlStatementIsSaved) {
    std::u8string queryString(u8"SELECT * FROM users", 19);
    Query query(queryString);
    EXPECT_TRUE(query.GetSqlStatement() == queryString);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, CannotAssignParametersWhenNoneExist) {
    std::u8string queryString(u8"SELECT * FROM users", 19);
    Query query(queryString);

    EXPECT_EQ(query.GetParameterInfo().size(), 0U);
    EXPECT_THROW(
      query.SetParameterValue(u8"mooh", Value(10)),
      Nuclex::ThinOrm::Errors::BadParameterNameError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, CannotReadParametersWhenNoneExist) {
    std::u8string queryString(u8"SELECT * FROM users", 19);
    Query query(queryString);

    EXPECT_EQ(query.GetParameterInfo().size(), 0U);
    EXPECT_THROW(
      query.GetParameterValue(u8"mooh"),
      Nuclex::ThinOrm::Errors::BadParameterNameError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, ParametersInQueryStringAreDiscovered) {
    std::u8string queryString(u8"SELECT * FROM users WHERE name={userName}", 41);
    Query query(queryString);

    const std::vector<QueryParameterView> &parameters = query.GetParameterInfo();
    ASSERT_EQ(parameters.size(), 1U);
    EXPECT_TRUE(parameters.at(0).Name == std::u8string(u8"userName"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, ParametersInQueryStringCanBeAssigned) {
    std::u8string queryString(u8"SELECT * FROM users WHERE name={userName}", 41);
    Query query(queryString);

    query.SetParameterValue(u8"userName", 123);
    Value parameterValue = query.GetParameterValue(u8"userName");
    EXPECT_EQ(parameterValue.ToInt32(), 123);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, ReadingUnassignedParametersIsAnError) {
    std::u8string queryString(u8"SELECT * FROM users WHERE name={userName}", 41);
    Query query(queryString);

    EXPECT_THROW(
      Value parameterValue = query.GetParameterValue(u8"userName"),
      Nuclex::ThinOrm::Errors::UnassignedParameterError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QueryTest, CurlyBracesCanBeEscaped) {
    std::u8string queryString(
      u8"SELECT * FROM users WHERE age >= {minimumAge} AND name='{{curly}}'", 66
    );
    Query query(queryString);

    const std::vector<QueryParameterView> &parameters = query.GetParameterInfo();
    ASSERT_EQ(parameters.size(), 1U);
    EXPECT_TRUE(parameters.at(0).Name == std::u8string(u8"minimumAge"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
