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

    std::optional<std::u8string> timeoutValue = s.GetOption(u8"Timeout");
    ASSERT_TRUE(timeoutValue.has_value());
    EXPECT_EQ(timeoutValue.value(), std::u8string(u8"30"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, KeysAreCaseInsensitive) {
    ConnectionString s = ConnectionString::Parse(u8"dRiVeR=sqlite; pAtH=/tmp/example.db");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"sqlite"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"/tmp/example.db"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, DuplicateKeysAreDisallowed) {
    EXPECT_THROW(
      ConnectionString::Parse(u8"Driver=sqlite; Host=.; driver=postgres"),
      std::invalid_argument
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, DuplicateOptionKeysAreDisallowed) {
    EXPECT_THROW(
      ConnectionString::Parse(u8"Driver=sqlite; Host=.; timeout=30; tImEoUt=40"),
      std::invalid_argument
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, EmptySegmentsAreIgnored) {
    ConnectionString s = ConnectionString::Parse(u8"Driver=mariadb;;; Host=localhost;");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mariadb"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, NonExistentOptionsCanBeQueried) {
    ConnectionString s = ConnectionString::Parse(u8"Driver=mysql; Host=localhost; dummy=yes");

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mysql"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::u8string> nonExistentValue = s.GetOption(u8"DoesNotExist");
    EXPECT_FALSE(nonExistentValue.has_value());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, UsernameAndPasswordCanBeSpecified) {
    ConnectionString s = ConnectionString::Parse(
      u8"Driver=mysql; Host=localhost; User=yes; Password=no"
    );

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mysql"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::u8string> userValue = s.GetUser();
    ASSERT_TRUE(userValue.has_value());
    EXPECT_EQ(userValue.value(), std::u8string(u8"yes"));

    std::optional<std::u8string> passwordValue = s.GetPassword();
    ASSERT_TRUE(passwordValue.has_value());
    EXPECT_EQ(passwordValue.value(), std::u8string(u8"no"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, PortCanBeSpecified) {
    ConnectionString s = ConnectionString::Parse(
      u8"Driver=mysql; Host=localhost; Port=1433"
    );

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mysql"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::uint16_t> portValue = s.GetPort();
    ASSERT_TRUE(portValue.has_value());
    EXPECT_EQ(portValue.value(), std::uint16_t(1433));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, OptionsCanBeValueless) {
    ConnectionString s = ConnectionString::Parse(
      u8"Driver=mysql; Host=localhost; Extra1; Extra2"
    );

    EXPECT_EQ(s.GetDriver(), std::u8string(u8"mysql"));
    EXPECT_EQ(s.GetHostnameOrPath(), std::u8string(u8"localhost"));

    std::optional<std::u8string> extra1Value = s.GetOption(u8"Extra1");
    ASSERT_TRUE(extra1Value.has_value());
    EXPECT_TRUE(extra1Value.value().empty());

    std::optional<std::u8string> extra2Value = s.GetOption(u8"Extra2");
    ASSERT_TRUE(extra2Value.has_value());
    EXPECT_TRUE(extra2Value.value().empty());
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(ConnectionStringTest, CanBeConvertedIntoPlainString) {
    ConnectionString s = ConnectionString::Parse(
      u8"Driver=mariadb; Host=db.local; User=me; Timeout=30; Pooling"
    );

    std::u8string plainString = s.ToString();

    EXPECT_NE(plainString.find(ConnectionString::DriverPropertyName), std::u8string::npos);
    EXPECT_NE(plainString.find(u8"=mariadb"), std::u8string::npos);

    EXPECT_NE(plainString.find(ConnectionString::HostPropertyName), std::u8string::npos);
    EXPECT_NE(plainString.find(u8"=db.local"), std::u8string::npos);

    EXPECT_NE(plainString.find(ConnectionString::UserPropertName), std::u8string::npos);
    EXPECT_NE(plainString.find(u8"=me"), std::u8string::npos);

    EXPECT_EQ(plainString.find(ConnectionString::PortPropertyName), std::u8string::npos);
    EXPECT_EQ(plainString.find(ConnectionString::PasswordPropertName), std::u8string::npos);

    EXPECT_NE(plainString.find(u8"Timeout"), std::u8string::npos);
    EXPECT_NE(plainString.find(u8"=30"), std::u8string::npos);

    EXPECT_NE(plainString.find(u8"Pooling"), std::u8string::npos);
    EXPECT_EQ(plainString.find(u8"Pooling="), std::u8string::npos);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
