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

#include "Nuclex/ThinOrm/Configuration/ConnectionString.h"

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, HasDefaultConstructor) {
    EXPECT_NO_THROW(
      ConnectionString s;
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, ParsingEmptyStringFails) {
    EXPECT_THROW(
      ConnectionString::Parse(std::u8string()),
      std::invalid_argument
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, DriverAndPathSuffice) {
    ConnectionString s = ConnectionString::Parse(u8"Driver=sqlite; Path=/tmp/test.db");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"sqlite"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"/tmp/test.db"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, HostnameWithoutDatabaseSuffices) {
    ConnectionString s = ConnectionString::Parse(u8"Driver=sqlite; Host=localhost");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"sqlite"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, ValuesAreTrimmed) {
    ConnectionString s = ConnectionString::Parse(u8" Driver = mysql ; Host = db ; User = me ");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mysql"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"db"));
    EXPECT_EQ(s.GetUser(), std::u8string(u8"me"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, UnknownKeysBecomeOptions) {
    ConnectionString s = ConnectionString::Parse(u8"Driver=mariadb; Host=localhost; Timeout=30");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mariadb"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));
    EXPECT_EQ(s.GetOption(u8"Timeout"), std::u8string(u8"30"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, KeysAreCaseInsensitive) {
    ConnectionString s = ConnectionString::Parse(u8"dRiVeR=sqlite; pAtH=/tmp/example.db");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"sqlite"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"/tmp/example.db"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
