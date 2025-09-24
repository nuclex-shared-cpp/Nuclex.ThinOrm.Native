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

  std::optional<bool> Value::AsBool() const {
    if(this->empty) [[unlikely]] {
      return std::optional<bool>();
    } else {
      switch (this->type) {
        case ValueType::Boolean: { return this->value.Boolean; }
        case ValueType::UInt8: { return (0 < this->value.Uint8); }
        case ValueType::Int16: { return (this->value.Int16 != 0); }
        case ValueType::Int32: { return (this->value.Int32 != 0); }
        case ValueType::Int64: { return (this->value.Int64 != 0); }
        case ValueType::Decimal: { return !this->value.DecimalValue.IsZero(); }
        case ValueType::Float: { return (this->value.Float != 0.0f); }
        case ValueType::Double: { return (this->value.Float != 0.0); }
        case ValueType::String: {
          using Nuclex::Support::Text::StringMatcher;
          constexpr const bool caseSensitive = false;
          bool isSpecialTrueString = (
            StringMatcher::AreEqual<caseSensitive>(this->value.String, onBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, yesBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, trueBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, enabledBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, activeBooleanLiteral)
          );
          if(isSpecialTrueString) {
            return true;
          }

          if(this->value.String.find(u8'.') == std::string::npos) {
            return Nuclex::Support::Text::lexical_cast<int>(this->value.String) != 0;
          } else {
            float floatValue = Nuclex::Support::Text::lexical_cast<float>(this->value.String);
            return (floatValue < -0.5) || (floatValue >= 0.5);
          }
        }
        case ValueType::Date: {
          return (this->value.DateTimeValue.GetDateOnly().GetTicks() != 0);
        }
        case ValueType::Time: {
          return (this->value.DateTimeValue.GetTimeOnly().GetTicks() != 0);
        }
        case ValueType::DateTime: {
          return this->value.DateTimeValue.GetTicks() != 0;
        }
        case ValueType::Blob: {
          return !this->value.Blob.empty();
        }
        default: {
          assert(false && u8"Unsupported value type");
          return false;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::uint8_t> Value::AsUInt8() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::uint8_t>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: {
          return this->value.Boolean ? std::uint8_t(1) : std::uint8_t(0);
        }
        case ValueType::UInt8: { return this->value.Uint8; }
        case ValueType::Int16: { return static_cast<std::uint8_t>(this->value.Int16); }
        case ValueType::Int32: { return static_cast<std::uint8_t>(this->value.Int32); }
        case ValueType::Int64: { return static_cast<std::uint8_t>(this->value.Int64); }
        case ValueType::Decimal: {
          return static_cast<std::uint8_t>(
            this->value.DecimalValue.RoundToInt()
          );
        }
        case ValueType::Float: {
          return static_cast<std::uint8_t>(
            Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Float)
          );
        }
        case ValueType::Double: {
          return static_cast<std::uint8_t>(
            Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Double)
          );
        }
        case ValueType::String: {
          if(this->value.String.find(u8'.') == std::string::npos) {
            return Nuclex::Support::Text::lexical_cast<std::uint8_t>(this->value.String);
          } else {
            return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(
              Nuclex::Support::Text::lexical_cast<float>(this->value.String)
            );
          }
        }
        case ValueType::Date: {
          return static_cast<std::uint8_t>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<std::uint8_t>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::DateTime: {
          return static_cast<std::uint8_t>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Blob: {
          if(this->value.Blob.empty()) {
            return 0;
          } else {
            return std::to_integer<std::uint8_t>(this->value.Blob[0]);
          }
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::int16_t> Value::AsInt16() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::int16_t>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1 : 0; }
        case ValueType::UInt8: { return static_cast<std::int16_t>(this->value.Uint8); }
        case ValueType::Int16: { return this->value.Int16; }
        case ValueType::Int32: { return static_cast<std::int16_t>(this->value.Int32); }
        case ValueType::Int64: { return static_cast<std::int16_t>(this->value.Int64); }
        case ValueType::Decimal: {
          return static_cast<std::int16_t>(
            this->value.DecimalValue.RoundToInt()
          );
        }
        case ValueType::Float: {
          return static_cast<std::int16_t>(
            Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Float)
          );
        }
        case ValueType::Double: {
          return static_cast<std::int16_t>(
            Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Double)
          );
        }
        case ValueType::String: {
          if(this->value.String.find(u8'.') == std::string::npos) {
            return Nuclex::Support::Text::lexical_cast<std::int16_t>(this->value.String);
          } else {
            return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(
              Nuclex::Support::Text::lexical_cast<float>(this->value.String)
            );
          }
        }
        case ValueType::Date: {
          return static_cast<std::int16_t>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<std::int16_t>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::DateTime: {
          return static_cast<std::int16_t>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Blob: {
          return readIntegerFromBlob<std::int16_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::int32_t> Value::AsInt32() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::int32_t>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1 : 0; }
        case ValueType::UInt8: { return static_cast<std::int32_t>(this->value.Uint8); }
        case ValueType::Int16: { return static_cast<std::int32_t>(this->value.Int16); }
        case ValueType::Int32: { return this->value.Int32; }
        case ValueType::Int64: { return static_cast<std::int32_t>(this->value.Int64); }
        case ValueType::Decimal: {
          return static_cast<std::int32_t>(
            this->value.DecimalValue.RoundToInt()
          );
        }
        case ValueType::Float: {
          return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Float);
        }
        case ValueType::Double: {
          return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(this->value.Double);
        }
        case ValueType::String: {
          if(this->value.String.find(u8'.') == std::string::npos) {
            return Nuclex::Support::Text::lexical_cast<std::int32_t>(this->value.String);
          } else {
            return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt32(
              Nuclex::Support::Text::lexical_cast<float>(this->value.String)
            );
          }
        }
        case ValueType::Date: {
          return static_cast<std::int32_t>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<std::int32_t>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::DateTime: {
          return static_cast<std::int32_t>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Blob: {
          return readIntegerFromBlob<std::int32_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::int64_t> Value::AsInt64() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::int64_t>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1 : 0; }
        case ValueType::UInt8: { return static_cast<std::int64_t>(this->value.Uint8); }
        case ValueType::Int16: { return static_cast<std::int64_t>(this->value.Int16); }
        case ValueType::Int32: { return static_cast<std::int64_t>(this->value.Int32); }
        case ValueType::Int64: { return this->value.Int64; }
        case ValueType::Decimal: {
          return static_cast<std::int64_t>(
            this->value.DecimalValue.RoundToInt() // TODO: Maybe add ToInt64()?
          );
        }
        case ValueType::Float: {
          return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt64(this->value.Float);
        }
        case ValueType::Double: {
          return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt64(this->value.Double);
        }
        case ValueType::String: {
          if(this->value.String.find(u8'.') == std::string::npos) {
            return Nuclex::Support::Text::lexical_cast<std::int64_t>(this->value.String);
          } else {
            return Nuclex::ThinOrm::Utilities::Quantizer::NearestInt64(
              Nuclex::Support::Text::lexical_cast<double>(this->value.String)
            );
          }
        }
        case ValueType::Date: {
          return static_cast<std::int64_t>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<std::int64_t>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::DateTime: {
          return static_cast<std::int64_t>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Blob: {
          return readIntegerFromBlob<std::int64_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<Decimal> Value::AsDecimal() const {
    if(this->empty) [[unlikely]] {
      return std::optional<Decimal>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? Decimal(1) : Decimal(0); }
        case ValueType::UInt8: { return Decimal(this->value.Uint8); }
        case ValueType::Int16: { return Decimal(this->value.Int16); }
        case ValueType::Int32: { return Decimal(this->value.Int32); }
        case ValueType::Int64: { return Decimal(this->value.Int64); }
        case ValueType::Decimal: { return this->value.DecimalValue; }
        case ValueType::Float: { return Decimal(this->value.Float); }
        case ValueType::Double: { return Decimal(this->value.Double); }
        case ValueType::String: {
          return Decimal(0); // TODO: Parse 128-bit decimals from strings :-O
        }
        case ValueType::Date: {
          return Decimal(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return Decimal(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          ); // TOOD: Add fractional seconds as decimal places
        }
        case ValueType::DateTime: {
          return Decimal(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          ); // TODO: Add fractional seconds as decimal places
        }
        case ValueType::Blob: {
          // TODO: Implement reading 128-bit decimals from blobs
          return Decimal(readIntegerFromBlob<std::int64_t>(this->value.Blob));
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<float> Value::AsFloat() const {
    if(this->empty) [[unlikely]] {
      return std::optional<float>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1.0f : 0.0f; }
        case ValueType::UInt8: { return static_cast<float>(this->value.Uint8); }
        case ValueType::Int16: { return static_cast<float>(this->value.Int16); }
        case ValueType::Int32: { return static_cast<float>(this->value.Int32); }
        case ValueType::Int64: { return static_cast<float>(this->value.Int64); }
        case ValueType::Decimal: { return this->value.DecimalValue.ToFloat(); }
        case ValueType::Float: { return this->value.Float; }
        case ValueType::Double: { return static_cast<float>(this->value.Double); }
        case ValueType::String: {
          return Nuclex::Support::Text::lexical_cast<float>(this->value.String);
        }
        case ValueType::Date: {
          return static_cast<float>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<float>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          ) + this->value.DateTimeValue.GetSecondFraction();
        }
        case ValueType::DateTime: {
          return static_cast<float>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          ) + this->value.DateTimeValue.GetSecondFraction();
        }
        case ValueType::Blob: {
          // TODO: Implement readFloatFromBlob()
          return readIntegerFromBlob<std::int64_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0.0f;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<double> Value::AsDouble() const {
    if(this->empty) [[unlikely]] {
      return std::optional<double>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1.0 : 0.0; }
        case ValueType::UInt8: { return static_cast<double>(this->value.Uint8); }
        case ValueType::Int16: { return static_cast<double>(this->value.Int16); }
        case ValueType::Int32: { return static_cast<double>(this->value.Int32); }
        case ValueType::Int64: { return static_cast<double>(this->value.Int64); }
        case ValueType::Decimal: { return this->value.DecimalValue.ToFloat(); }
        case ValueType::Float: { return static_cast<double>(this->value.Float); }
        case ValueType::Double: { return this->value.Double; }
        case ValueType::String: {
          return Nuclex::Support::Text::lexical_cast<double>(this->value.String);
        }
        case ValueType::Date: {
          return static_cast<double>(
            this->value.DateTimeValue.GetDateOnly().ToSecondSinceUnixEpoch()
          );
        }
        case ValueType::Time: {
          return static_cast<double>(
            this->value.DateTimeValue.GetTimeOnly().ToSecondSinceUnixEpoch()
          ) + this->value.DateTimeValue.GetSecondFraction();
        }
        case ValueType::DateTime: {
          return static_cast<double>(
            this->value.DateTimeValue.ToSecondSinceUnixEpoch()
          ) + this->value.DateTimeValue.GetSecondFraction();
        }
        case ValueType::Blob: {
          // TODO: Implement readFloatFromBlob()
          return readIntegerFromBlob<std::int64_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return 0.0;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> Value::AsString() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::u8string>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: {
          return this->value.Boolean ? trueBooleanLiteral : falseBooleanLiteral;
        }
        case ValueType::UInt8: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Uint8);
        }
        case ValueType::Int16: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Int16);
        }
        case ValueType::Int32: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Int32);
        }
        case ValueType::Int64: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Int64);
        }
        case ValueType::Decimal: { return this->value.DecimalValue.ToString(); }
        case ValueType::Float: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Float);
        }
        case ValueType::Double: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Double);
        }
        case ValueType::String: { return this->value.String; }
        case ValueType::Date: {
          return this->value.DateTimeValue.GetDateOnly().ToIso8601DateTime();
        }
        case ValueType::Time: {
          return this->value.DateTimeValue.ToIso8601Time();
        }
        case ValueType::DateTime: {
          return this->value.DateTimeValue.ToIso8601DateTime();
        }
        case ValueType::Blob: {
          return Nuclex::Support::Text::lexical_cast<std::u8string>(this->value.Blob.size());
        }
        default: {
          assert(false && u8"Unsupported value type");
          return std::u8string();
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::vector<std::byte>> Value::AsBlob() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::vector<std::byte>>();
    } else {
      std::vector<std::byte> result;
      switch(this->type) {
        case ValueType::Boolean: {
          result.reserve(1);
          result.push_back(std::byte(this->value.Boolean ? 1 : 0));
          break;
        }
        case ValueType::UInt8: {
          result.reserve(1);
          writeIntegerToBlob(result, this->value.Uint8);
          break;
        }
        case ValueType::Int16: {
          result.reserve(2);
          writeIntegerToBlob(result, this->value.Int16);
          break;
        }
        case ValueType::Int32: {
          result.reserve(4);
          writeIntegerToBlob(result, this->value.Int32);
          break;
        }
        case ValueType::Int64: {
          result.reserve(8);
          writeIntegerToBlob(result, this->value.Int64);
          break;
        }
        case ValueType::Decimal: {
          // TODO: Implement 'Decimal' to blob conversion
          break;
        }
        case ValueType::Float: {
          result.reserve(4);
          writeFloatToBlob(result, this->value.Float);
          break;
        }
        case ValueType::Double: {
          result.reserve(8);
          writeFloatToBlob(result, this->value.Double);
          break;
        }
        case ValueType::String: {
          result.resize(this->value.String.size());
          std::copy_n(
            this->value.String.data(),
            this->value.String.size(),
            reinterpret_cast<char8_t *>(result.data())
          );
          break;
        }
        case ValueType::Date: {
          result.reserve(8);
          writeIntegerToBlob(result, this->value.DateTimeValue.GetDateOnly().GetTicks());
          break;
        }
        case ValueType::Time: {
          result.reserve(8);
          writeIntegerToBlob(result, this->value.DateTimeValue.GetTimeOnly().GetTicks());
        }
        case ValueType::DateTime: {
          result.reserve(8);
          writeIntegerToBlob(result, this->value.DateTimeValue.GetTicks());
        }
        case ValueType::Blob: {
          return this->value.Blob;
        }
        default: {
          assert(false && u8"Unsupported value type");
          break;
        }
      } // switch on value type

      return result;
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
