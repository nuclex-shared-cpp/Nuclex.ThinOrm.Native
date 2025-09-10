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

#include "Nuclex/ThinOrm/ValueType.h"

#include <Nuclex/Support/Text/StringMatcher.h> // for StringMatcher::AreEqual()

#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Type name for the boolean value type</summary>
  const std::u8string booleanTypeName(u8"boolean", 7);

  /// <summary>Type name for the 8-bit unsigned integer value type</summary>
  const std::u8string uint8TypeName(u8"uint8", 5);

  /// <summary>Type name for the 16-bit integer value type</summary>
  const std::u8string int16TypeName(u8"int16", 5);

  /// <summary>Type name for the 32-bit integer value type</summary>
  const std::u8string int32TypeName(u8"int32", 5);

  /// <summary>Type name for the 64-bit integer value type</summary>
  const std::u8string int64TypeName(u8"int64", 5);

  /// <summary>Type name for the decimal value type</summary>
  const std::u8string decimalTypeName(u8"decimal", 7);

  /// <summary>Type name for the 32-bit floating point value type</summary>
  const std::u8string floatTypeName(u8"float", 5);

  /// <summary>Type name for the 64-bit floating point value type</summary>
  const std::u8string doubleTypeName(u8"double", 6);

  /// <summary>Type name for the string value type</summary>
  const std::u8string stringTypeName(u8"string", 6);

  /// <summary>Type name for the date-only value type</summary>
  const std::u8string dateTypeName(u8"date", 4);

  /// <summary>Type name for the time-only value type</summary>
  const std::u8string timeTypeName(u8"time", 4);

  /// <summary>Type name for the date and time value type</summary>
  const std::u8string dateTimeTypeName(u8"datetime", 8);

  /// <summary>Type name for the binary blob value type</summary>
  const std::u8string blobTypeName(u8"blob", 4);

  /// <summary>Type name that indicates an error</summary>
  const std::u8string invalidTypeName(u8"<invalid>", 9);

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  const std::u8string &StringFromValueType(ValueType valueType) noexcept {
    switch(valueType) {
      case ValueType::Boolean: { return booleanTypeName; }
      case ValueType::UInt8: { return uint8TypeName; }
      case ValueType::Int16: { return int16TypeName; }
      case ValueType::Int32: { return int32TypeName; }
      case ValueType::Int64: { return int64TypeName; }
      case ValueType::Decimal: { return decimalTypeName; }
      case ValueType::Float: { return floatTypeName; }
      case ValueType::Double: { return doubleTypeName; }
      case ValueType::String: { return stringTypeName; }
      case ValueType::Date: { return dateTypeName; }
      case ValueType::Time: { return timeTypeName; }
      case ValueType::DateTime: { return dateTimeTypeName; }
      case ValueType::Blob: { return blobTypeName; }
      default: {
        assert(false && u8"Unsupported value type");
        return invalidTypeName;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  ValueType ValueTypeFromString(const std::u8string &valueTypeString) noexcept {
    using Nuclex::Support::Text::StringMatcher;
    constexpr const bool caseSensitive = false;

    if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, booleanTypeName)) {
      return ValueType::Boolean;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, uint8TypeName)) {
      return ValueType::UInt8;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, int16TypeName)) {
      return ValueType::Int16;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, int32TypeName)) {
      return ValueType::Int32;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, int64TypeName)) {
      return ValueType::Int64;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, decimalTypeName)) {
      return ValueType::Decimal;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, floatTypeName)) {
      return ValueType::Float;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, doubleTypeName)) {
      return ValueType::Double;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, stringTypeName)) {
      return ValueType::String;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, dateTypeName)) {
      return ValueType::Date;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, timeTypeName)) {
      return ValueType::Time;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, dateTimeTypeName)) {
      return ValueType::DateTime;
    } else if(StringMatcher::AreEqual<caseSensitive>(valueTypeString, blobTypeName)) {
      return ValueType::Blob;
    } else {
      assert(false && u8"Specified type name not recognized");
      return static_cast<ValueType>(-1); // indicate invalid
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
