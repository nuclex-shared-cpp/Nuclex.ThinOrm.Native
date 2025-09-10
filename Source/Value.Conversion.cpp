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

#include "./Utilities/Quantizer.h"

#include <ctime> // for std::gmtime()
#include <cassert> // for assert()

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

  // ------------------------------------------------------------------------------------------- //

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
        result <= static_cast<TInteger>(std::to_integer<std::uint8_t>(blob[0]));
        break;
      }
      default: { break; } // 0 bytes = leave result set to 0
    }
    return result;
  }

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  std::optional<bool> Value::ToBool() const {
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
          return (
            StringMatcher::AreEqual<caseSensitive>(this->value.String, onBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, yesBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, trueBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, enabledBooleanLiteral) ||
            StringMatcher::AreEqual<caseSensitive>(this->value.String, activeBooleanLiteral)
          );
        }
        case ValueType::Date: {
          return (
            (this->value.DateTime.tm_year != 0) ||
            (this->value.DateTime.tm_mon != 0) ||
            (this->value.DateTime.tm_yday != 0)
          );
        }
        case ValueType::Time: {
          return (
            (this->value.DateTime.tm_hour != 0) ||
            (this->value.DateTime.tm_min != 0) ||
            (this->value.DateTime.tm_sec != 0)
          );
        }
        case ValueType::DateTime: {
          return (
            (this->value.DateTime.tm_year != 0) ||
            (this->value.DateTime.tm_mon != 0) ||
            (this->value.DateTime.tm_yday != 0) ||
            (this->value.DateTime.tm_hour != 0) ||
            (this->value.DateTime.tm_min != 0) ||
            (this->value.DateTime.tm_sec != 0)
          );
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

  std::optional<std::uint8_t> Value::ToUInt8() const {
    if(this->empty) [[unlikely]] {
      return std::optional<std::uint8_t>();
    } else {
      switch(this->type) {
        case ValueType::Boolean: { return this->value.Boolean ? 1 : 0; }
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
          return Nuclex::Support::Text::lexical_cast<std::uint8_t>(this->value.String);
        }
        case ValueType::Date:
        case ValueType::DateTime: {
          // For the date-only type, the hours, minutes and seconds should be set to zero,
          // so we'd obtain the seconds since the UNIX epoch in multiples of 86'400 (= days)
          std::tm dateTimeCopy = this->value.DateTime;
          std::time_t secondsSinceUnixEpoch = std::mktime(&dateTimeCopy);
          return static_cast<std::uint8_t>(secondsSinceUnixEpoch);
        }
        case ValueType::Time: {
          // If a time is stored, year, month and day should be set to zero,
          // so we'd obtain the seconds since midnight.
          std::tm dateTimeCopy = this->value.DateTime;
          std::time_t secondsSinceUnixEpoch = std::mktime(&dateTimeCopy);
          return static_cast<std::uint8_t>(secondsSinceUnixEpoch);
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
          return false;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::int16_t> Value::ToInt16() const {
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
          return Nuclex::Support::Text::lexical_cast<std::int16_t>(this->value.String);
        }
        case ValueType::Date:
        case ValueType::DateTime: {
          // For the date-only type, the hours, minutes and seconds should be set to zero,
          // so we'd obtain the seconds since the UNIX epoch in multiples of 86'400 (= days)
          std::tm dateTimeCopy = this->value.DateTime;
          std::time_t secondsSinceUnixEpoch = std::mktime(&dateTimeCopy);
          return static_cast<std::int16_t>(secondsSinceUnixEpoch);
        }
        case ValueType::Time: {
          // If a time is stored, year, month and day should be set to zero,
          // so we'd obtain the seconds since midnight.
          std::tm dateTimeCopy = this->value.DateTime;
          std::time_t secondsSinceUnixEpoch = std::mktime(&dateTimeCopy);
          return static_cast<std::int16_t>(secondsSinceUnixEpoch);
        }
        case ValueType::Blob: {
          return readIntegerFromBlob<std::int16_t>(this->value.Blob);
        }
        default: {
          assert(false && u8"Unsupported value type");
          return false;
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
