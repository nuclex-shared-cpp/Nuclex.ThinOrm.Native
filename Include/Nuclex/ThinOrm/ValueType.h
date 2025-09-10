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

#ifndef NUCLEX_THINORM_VALUETYPE_H
#define NUCLEX_THINORM_VALUETYPE_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>SQL query that can be executed on a database connection</summary>
  enum class NUCLEX_THINORM_TYPE ValueType {

    /// <summary>A boolean (true or false) value, sometimes called a bit</summary>
    Boolean,

    /// <summary>An 8-bit integer with values from 0 to 255 (usually unsigned)</summary>
    UInt8,

    /// <summary>A 16-bit integer with values from -32768 to 32767</summary>
    Int16,

    /// <summary>A normal 32-bit integer with values from -2 billion to 2 billion</summary>
    Int32,

    /// <summary>A 64-bit integer with values from -9 qintillion to 9 quintillion</summary>
    Int64,

    /// <summary>Fixed-point number with a configurable number of decimal places</summary>
    Decimal,

    /// <summary>32-bit floating point value with good precision for small numbers</summary>
    Float,

    /// <summary>32-bit floating point value with good precision for most numbers</summary>
    Double,

    /// <summary>Variable length text in UTF-8 format</summary>
    String,

    /// <summary>A pure date value without its time component</summary>
    Date,

    /// <summary>A pure time value without an accompanying date</summary>
    Time,

    /// <summary>A date and time value</summary>
    DateTime,

    /// <summary>Binary data of variable or fixed length</summary>
    Blob

  };

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Returns a string representation of a member of the <see cref="ValueType" /> enumeration
  /// </summary>
  /// <param name="valueType">Value type for which a string will be returned</param>
  /// <returns>A string representaiton of the provided value type</returns>
  NUCLEX_THINORM_API const std::u8string &StringFromValueType(ValueType valueType) noexcept;

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Returns the <see cef="ValueTYpe" /> represented by the provided string which
  ///   must match the strings returned from the <see cref="StringFromValueType" /> method
  /// </summary>
  /// <param name="valueTypeString">
  ///   Value type represented as a string matching the strings returned by
  ///   the <see cref="StringFromValueType" /> method
  /// </param>
  /// <returns>The value type matching the specified string</returns>
  NUCLEX_THINORM_API ValueType ValueTypeFromString(const std::u8string &valueTypeString) noexcept;

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_VALUETYPE_H
