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

#include "Nuclex/ThinOrm/Errors/BadDateFormatError.h"
#include "../../Source/Utilities/Iso8601Converter.h" // for Iso8601Converter

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Number of 1/10th microseconds in one microsecond</summary>
  constexpr const std::int64_t TicksPerMicrosecond = 10;

  /// <summary>Number of 1/10th microseconds that elapse every second</summary>
  constexpr const std::int64_t TicksPerSecond = 1000 * 1000 * TicksPerMicrosecond;

  /// <summary>Number of 1/10th microseconds that elapse in a single day</summary>
  constexpr const std::int64_t TicksPerDay = 86'400ll * TicksPerSecond;

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CanParseBasicFormatDate) {
    const std::int64_t ExpectedTickCount = 730'518ll * TicksPerDay;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"20010203");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CanParseExtendedFormatDate) {
    const std::int64_t ExpectedTickCount = 731'706ll * TicksPerDay;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"2004-05-06");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingTooShortDateCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"123456"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingNonDateCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"1999-XX-XX"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingImpossibleDateCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"1980-13-01"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CanParseBasicFormatTime) {
    const std::int64_t ExpectedTickCount = 45296 * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601Time(u8"123456");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CanParseExtendedFormatTime) {
    const std::int64_t ExpectedTickCount = 45296 * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601Time(u8"12:34:56");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingTooShortTimeCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601Time(u8"1312"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingNonTimeCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601Time(u8"10:33:??"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingImpossibleTimeCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601Time(u8"29:12:21"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, ParsingTimeWithLeapSecondIsAllowed) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601Time(u8"23:58:60"),
      Errors::BadDateFormatError
    );

    std::int64_t ticks = Iso8601Converter::ParseIso8601Time(u8"23:59:60");
    EXPECT_EQ(ticks, TicksPerDay);
  }


  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeCanBeParsed) {
    const std::int64_t ExpectedTickCount = 63'390'867'631ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"2009-10-11T14:20:31");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, SpaceBetweenCombinedDateAndTimeIsAccepted) {
    const std::int64_t ExpectedTickCount = 63'050'579'662ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"1998-12-30 01:54:22");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeAllowsExplicitUtc) {
    const std::int64_t ExpectedTickCount = 62'585'006'170ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"1984-03-29T11:56:10Z");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeIgnoresShortTimeZone) {
    const std::int64_t ExpectedTickCount = 64'079'614'318ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"2031-08-09T04:51:58+02");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeIgnoresLongTimeZone) {
    const std::int64_t ExpectedTickCount = 64'236'662'041ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"2036-07-30T21:14:01-0630");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeIgnoresXLongTimeZone) {
    const std::int64_t ExpectedTickCount = 63'293'861'505ll * TicksPerSecond;

    std::int64_t ticks = Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45+08:15");
    EXPECT_EQ(ticks, ExpectedTickCount);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, CombinedDateAndTimeWithBadTimezoneCausesException) {
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45+123"),
      Errors::BadDateFormatError
    );
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45-12345"),
      Errors::BadDateFormatError
    );
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45Z10"),
      Errors::BadDateFormatError
    );
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45scrap"),
      Errors::BadDateFormatError
    );
    EXPECT_THROW(
      Iso8601Converter::ParseIso8601DateTime(u8"2006-09-14T20:11:45+mooh"),
      Errors::BadDateFormatError
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, DateCanBePrinted) {
    const std::int64_t TickCount = 730'518ll * TicksPerDay;

    std::u8string printed(10, u8'\0');
    Iso8601Converter::PrintIso8601Date(printed.data(), TickCount);

    std::u8string expected(u8"2001-02-03");
    EXPECT_EQ(expected, printed);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, DateAndTimeCanBePrinted) {
    const std::int64_t TickCount = 731'706ll * TicksPerDay + 45296 * TicksPerSecond;

    std::u8string printed(19, u8'\0');
    Iso8601Converter::PrintIso8601DateTime(printed.data(), TickCount);

    std::u8string expected(u8"2004-05-06T12:34:56");
    EXPECT_EQ(expected, printed);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(Iso8601ConverterTest, TimeCanBePrinted) {
    const std::int64_t TickCount = 731'706ll * TicksPerDay + 45296 * TicksPerSecond;

    std::u8string printed(8, u8'\0');
    Iso8601Converter::PrintIso8601Time(printed.data(), TickCount);

    std::u8string expected(u8"12:34:56");
    EXPECT_EQ(expected, printed);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities
