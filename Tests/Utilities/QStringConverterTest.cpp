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

#include "../../Source/Utilities/QStringConverter.h" // for QStringConverter

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  TEST(QStringConverterTest, U8StringCanBeConvertedToQString) {
    std::u8string utf8String(u8"Hello © π ☃ 🦄!");

    QString qtString = QStringConverter::FromU8(utf8String);
    EXPECT_TRUE(qtString == QString::fromWCharArray(L"Hello © π ☃ 🦄!"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QStringConverterTest, U8StringViewCanBeConvertedToQString) {
    const char8_t *characters = u8"### Hello © π ☃ 🦄! ###";
    std::u8string_view utf8StringView(characters + 4, 14 + 2 + 2 + 3);

    QString qtString = QStringConverter::FromU8(utf8StringView);
    EXPECT_TRUE(qtString == QString::fromWCharArray(L"Hello © π ☃ 🦄!"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QStringConverterTest, U8StringsCanBeAppendedToQString) {
    QString qtString = QString::fromWCharArray(L"Hello ©");

    QStringConverter::AppendU8(qtString, u8" π", 3);
    QStringConverter::AppendU8(qtString, u8" ☃", 4);
    QStringConverter::AppendU8(qtString, u8" 🦄", 5);
    QStringConverter::AppendU8(qtString, u8"!", 1);

    EXPECT_TRUE(qtString == QString::fromWCharArray(L"Hello © π ☃ 🦄!"));
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QStringConverterTest, QStringsCanBeConvertedToU8String) {
    QString qtString = QString::fromWCharArray(L"Hello © π ☃ 🦄!");

    std::u8string utf8String = QStringConverter::ToU8(qtString);
    EXPECT_TRUE(utf8String == std::u8string(u8"Hello © π ☃ 🦄!"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities
