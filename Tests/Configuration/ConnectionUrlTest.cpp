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

  TEST(ConnectionUrlTest, NonExistentOptionsCanBeQueried) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"sqlite://example.db?dummy=yes");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"sqlite"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"example.db"));

    std::optional<std::u8string> nonExistentValue = u.GetOption(u8"DoesNotExist");
    EXPECT_FALSE(nonExistentValue.has_value());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, DuplicateOptionKeysAreDisallowed) {
    EXPECT_THROW(
      ConnectionUrl::Parse(u8"sqlite://example.db?timeout=30&tImEoUt=40"),
      std::invalid_argument
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, OptionsCanBeValueless) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"sqlite://example.db?extra1&extra2");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"sqlite"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"example.db"));

    std::optional<std::u8string> extra1Value = u.GetOption(u8"Extra1");
    ASSERT_TRUE(extra1Value.has_value());
    EXPECT_TRUE(extra1Value.value().empty());

    std::optional<std::u8string> extra2Value = u.GetOption(u8"Extra2");
    ASSERT_TRUE(extra2Value.has_value());
    EXPECT_TRUE(extra2Value.value().empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionUrlTest, UrlParametersBecomeOptions) {
    ConnectionUrl u = ConnectionUrl::Parse(u8"sqlite://example.db?timeout=30");

    EXPECT_EQ(u.GetDriver(), std::u8string(u8"sqlite"));

    std::optional<std::u8string> databaseValue = u.GetDatabaseName();
    ASSERT_TRUE(databaseValue.has_value());
    EXPECT_EQ(databaseValue.value(), std::u8string(u8"example.db"));

    std::optional<std::u8string> timeoutValue = u.GetOption(u8"Timeout");
    ASSERT_TRUE(timeoutValue.has_value());
    EXPECT_EQ(timeoutValue.value(), std::u8string(u8"30"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
