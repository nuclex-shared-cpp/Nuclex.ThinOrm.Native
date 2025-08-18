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

#include "Nuclex/ThinOrm/Configuration/ConnectionUrl.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, HasDefaultConstructor) {
    EXPECT_NO_THROW(
      ConnectionUrl u;
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, ParsingEmptyStringFails) {
    EXPECT_THROW(
      ConnectionUrl::Parse(std::u8string()),
      std::invalid_argument
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, DatabaseCanBePathBased) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"sqlite:///tmp/test.db");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"sqlite"));
    EXPECT_EQ(u.GetHostnameOrPath(), std::u8string(u8"/tmp"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"test.db"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, UserCanBeSpecified) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"mariadb://me@localhost/mydatabase");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"mariadb"));
    EXPECT_EQ(u.GetUser(), std::u8string(u8"me"));
    EXPECT_EQ(u.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"mydatabase"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, UserAndPasswordCanBeSpecified) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"mariadb://user:pass@localhost/mydatabase");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"mariadb"));
    EXPECT_EQ(u.GetUser(), std::u8string(u8"user"));
    EXPECT_EQ(u.GetPassword(), std::u8string(u8"pass"));
    EXPECT_EQ(u.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"mydatabase"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, PortCanBeLastElement) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"mariadb://localhost:1433");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"mariadb"));
    EXPECT_EQ(u.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::uint16_t> portValue = u.GetPort();
    ASSERT_TRUE(portValue.has_value());
    EXPECT_EQ(portValue.value(), std::uint16_t(1433));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
