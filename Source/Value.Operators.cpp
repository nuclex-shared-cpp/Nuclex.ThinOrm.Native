#pragma region Apache License 2.0
/*
Nuclex Native Framework
Copyright (C) 2002-2024 Markus Ewald / Nuclex Development Labs

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

#include "Nuclex/ThinOrm/Value.h"

#include <Nuclex/Support/Text/StringMatcher.h> // for StringMatcher::AreEqual()
#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>
#include <Nuclex/Support/Endian.h> // for Endian

#include "./Utilities/Quantizer.h"

#include <ctime> // for std::gmtime()
#include <cassert> // for assert()
#include <algorithm> // for std::copy_n()
#include <cstring> // for std::memcpy()

// Turns a C++20 UTF-8 char8_t array (u8"") into a plain char array
//
// Why? Because using u8 expresses that the text should be stored as UTF-8,
// regardless of how the source file is encoded.
//
// Safe? Yes because any type in C++ can be aliased as a char sequence. In fact, this
// reinterpret_cast is shown as a correct example in the char8_t addition to
// the C++ standard. The opposite way (char * to char8_t *) invokes UB, though.
#define U8CHARS(x) (reinterpret_cast<const char *>(x))

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>String for the word 'on' to indicate a boolean value is true</summary>
  const std::u8string onBooleanLiteral(u8"on", 2);

  /// <summary>String for the word 'yes' to indicate a boolean value is true</summary>
  const std::u8string yesBooleanLiteral(u8"yes", 3);

  /// <summary>String for the word 'true' to indicate a boolean value is true</summary>
  const std::u8string trueBooleanLiteral(u8"true", 4);

  /// <summary>String for the word 'enabled' to indicate a boolean value is true</summary>
  const std::u8string enabledBooleanLiteral(u8"enabled", 7);

  /// <summary>String for the word 'active' to indicate a boolean value is true</summary>
  const std::u8string activeBooleanLiteral(u8"active", 6);

  /// <summary>String for the word 'false' to indicate a boolean value is false</summary>
  const std::u8string falseBooleanLiteral(u8"false", 5);

  /// <summary>Number of DateTime ticks on January, 1st in 1970</summary>
  const std::int64_t TicksOnJanuary1st1970 = 621'355'968'000'000'000ll;

  /// <summary>Blob that stores the binary state of a true boolean</summary>
  const std::vector<std::byte> trueBlob(1, std::byte(1));

  /// <summary>Blob that stores the binary state of a false boolean</summary>
  const std::vector<std::byte> falseBlob(1, std::byte(0));

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads an integer value out of a binary blob</summary>
  /// <param name="blob">Binary blob containing the data the integer will be read from</param>
  /// <returns>The integer read from the blob</returns>
  /// <remarks>
  ///   This matches the behavior of most databases where coercing blob data into a type
  ///   that is longer than the blob gives the result as if the blob was zero-padded.
  /// </remarks>
  template<typename TInteger>
  TInteger readIntegerFromBlob(const std::vector<std::byte> &blob) {
    std::vector<std::byte>::size_type size = blob.size();
    size = std::min(size, sizeof(TInteger));

    TInteger result = 0;
    switch(size) {
      case 8: {
        result = static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[7]));
        [[fallthrough]];
      }
      case 7: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[6]));
        result <<= 8;
        [[fallthrough]];
      }
      case 6: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[5]));
        result <<= 8;
        [[fallthrough]];
      }
      case 5: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[4]));
        result <<= 8;
        [[fallthrough]];
      }
      case 4: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[3]));
        result <<= 8;
        [[fallthrough]];
      }
      case 3: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[2]));
        result <<= 8;
        [[fallthrough]];
      }
      case 2: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[1]));
        result <<= 8;
        [[fallthrough]];
      }
      case 1: {
        result |= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[0]));
        break;
      }
      default: { break; } // 0 bytes = leave result set to 0
    }
    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Writes a little endian integer value into a binary blob</summary>
  /// <typeparam name="TInteger">Type of integer that will be written</typeparam>
  /// <param name="blob">Binary blob into which the integer will be written</param>
  /// <param name="value">Integer value that will be written into the blob</param>
  template<typename TInteger>
  void writeIntegerToBlob(std::vector<std::byte> &blob, TInteger value) {
    constexpr size_t byteCount = sizeof(TInteger);

    blob.resize(byteCount);

    if constexpr(byteCount >= 1) { blob[0] = static_cast<std::byte>(value >> (0*8)); }
    if constexpr(byteCount >= 2) { blob[1] = static_cast<std::byte>(value >> (1*8)); }
    if constexpr(byteCount >= 3) { blob[2] = static_cast<std::byte>(value >> (2*8)); }
    if constexpr(byteCount >= 4) { blob[3] = static_cast<std::byte>(value >> (3*8)); }
    if constexpr(byteCount >= 5) { blob[4] = static_cast<std::byte>(value >> (4*8)); }
    if constexpr(byteCount >= 6) { blob[5] = static_cast<std::byte>(value >> (5*8)); }
    if constexpr(byteCount >= 7) { blob[6] = static_cast<std::byte>(value >> (6*8)); }
    if constexpr(byteCount >= 8) { blob[7] = static_cast<std::byte>(value >> (7*8)); }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Writes an IEEE-754 floating point value into a binary blob</summary>
  /// <typeparam name="TFloat">Type of float that will be written</typeparam>
  /// <param name="blob">Binary blob into which the float will be written</param>
  /// <param name="value">Float value that will be written into the blob</param>
  template<typename TFloat>
  void writeFloatToBlob(std::vector<std::byte> &blob, TFloat value) {
    typedef typename std::conditional<
      sizeof(TFloat) == 4, std::uint32_t, std::uint64_t
    >::type SameSizedIntegerType;

    writeIntegerToBlob<SameSizedIntegerType>(
      blob, std::bit_cast<SameSizedIntegerType>(value)
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Value::operator bool() const {
    return AsBool().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::uint8_t() const {
    return AsUInt8().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::int16_t() const {
    return AsInt16().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::int32_t() const {
    return AsInt32().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::int64_t() const {
    return AsInt64().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator Decimal() const {
    return AsDecimal().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator float() const {
    return AsFloat().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator double() const {
    return AsDouble().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::u8string() const {
    return AsString().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator DateTime() const {
    return AsDateTime().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::vector<std::byte>() const {
    return AsBlob().value();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<bool>() const {
    return AsBool();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::uint8_t>() const {
    return AsUInt8();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::int16_t>() const {
    return AsInt16();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::int32_t>() const {
    return AsInt32();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::int64_t>() const {
    return AsInt64();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<Decimal>() const {
    return AsDecimal();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<float>() const {
    return AsFloat();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<double>() const {
    return AsDouble();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::u8string>() const {
    return AsString();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<DateTime>() const {
    return AsDateTime();
  }

  // ------------------------------------------------------------------------------------------- //

  Value::operator std::optional<std::vector<std::byte>>() const {
    return AsBlob();
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
