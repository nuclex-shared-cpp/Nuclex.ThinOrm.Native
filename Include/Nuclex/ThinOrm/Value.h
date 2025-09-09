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

#ifndef NUCLEX_THINORM_VALUE_H
#define NUCLEX_THINORM_VALUE_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/ValueType.h"
#include "Nuclex/ThinOrm/Decimal.h"

#include <optional> // for std::optional
#include <cstdint> // for std::size_t, std::byte
#include <cstdint> // for std::uint8_t, std::int16_t, etc.
#include <ctime> // for std::tm
#include <string> // for std::u8string
#include <vector> // for std::vector

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Value returned from a query or provided as parameter to one</summary>
  /// <remarks>
  ///   This is in essence a &quot;variant&quot; class which can store one of many different
  ///   data types. Once a value has been stored, it will retain its type and becomes
  ///   immutable, but upon reading, it can be coerced into any of the supported types.
  /// </remarks>
  class NUCLEX_THINORM_TYPE Value {

    /// <summary>Initializes a new value as a copy of an existing value</summary>
    /// <param name="other">Existing value that will be cloned</param>
    public: NUCLEX_THINORM_API Value(const Value &other);
    /// <summary>Initializes a new value by taking over an existing value</summary>
    /// <param name="other">Existing value that will be adopted</param>
    public: NUCLEX_THINORM_API Value(Value &&other);

    /// <summary>Initializes a new value as container of a boolean value</summary>
    /// <param name="booleanValue">Boolean value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<bool> booleanValue);
    /// <summary>Initializes a new value as container of an 8-bit integer value</summary>
    /// <param name="uint8Value">8-bit integer value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::uint8_t> uint8Value);
    /// <summary>Initializes a new value as container of a 16-bit integer value</summary>
    /// <param name="int16Value">16-bit integer value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::int16_t> int16Value);
    /// <summary>Initializes a new value as container of a 32-bit integer value</summary>
    /// <param name="int32Value">32-bit integer value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::int32_t> int32Value);
    /// <summary>Initializes a new value as container of a 64-bit integer value</summary>
    /// <param name="int64Value">64-bit integer value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::int64_t> int64Value);
    /// <summary>Initializes a new value as container of a decimal value</summary>
    /// <param name="decimalValue">Decimal value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<Decimal> decimalValue);
    /// <summary>Initializes a new value as container of a 32-bit floating point value</summary>
    /// <param name="floatValue">32-bit floating point value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<float> floatValue);
    /// <summary>Initializes a new value as container of a 64-bit floating point value</summary>
    /// <param name="doubleValue">64-bit floating point value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<double> doubleValue);
    /// <summary>Initializes a new value as container of a string value</summary>
    /// <param name="stringValue">String value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::u8string> stringValue);
    /// <summary>Initializes a new value as container of a blob value</summary>
    /// <param name="blobValue">Blob value to assume</param>
    public: NUCLEX_THINORM_API Value(const std::optional<std::vector<std::byte>> blobValue);
    /// <summary>Frees all memory owned by the value</summary>
    public: NUCLEX_THINORM_API ~Value();

    /// <summary>Retrieves the type of value stored in the value container</summary>
    /// <returns>The type of value currently held in the container</returns>
    public: NUCLEX_THINORM_API ValueType GetType() const;
    /// <summary>Checks whether the value is empty (NULL in database terms)</summary>
    /// <returns>True if the contained value is empty, false otherwise</returns>
    public: NUCLEX_THINORM_API bool IsEmpty() const;
    /// <summary>Throws an exception if the value is not of the specified type</summary>
    /// <param name="requiredType">Type the value must have</param>
    public: NUCLEX_THINORM_API void Require(ValueType requiredType) const;
    /// <summary>
    ///   Throws an exception if the value is not of the specified type or is not
    ///   matching the empty/not empty expectation
    /// </summary>
    /// <param name="requiredType">Type the value must have</param>
    public: NUCLEX_THINORM_API void Require(
      ValueType requiredType, bool requiredPresence
    ) const;

    /// <summary>Returns the value in the container as boolean</summary>
    /// <returns>The container's stored value as a boolean</returns>
    public: NUCLEX_THINORM_API std::optional<bool> ToBool() const;
    /// <summary>Returns the value in the container as an 8-bit integer</summary>
    /// <returns>The container's stored value as an 8-bit integer</returns>
    public: NUCLEX_THINORM_API std::optional<std::uint8_t> ToUint8() const;
    /// <summary>Returns the value in the container as a 16-bit integer</summary>
    /// <returns>The container's stored value as a 16-bit integer</returns>
    public: NUCLEX_THINORM_API std::optional<std::int16_t> ToInt16() const;
    /// <summary>Returns the value in the container as a 32-bit integer</summary>
    /// <returns>The container's stored value as a 32-bit integer</returns>
    public: NUCLEX_THINORM_API std::optional<std::int32_t> ToInt32() const;
    /// <summary>Returns the value in the container as a 64-bit integer</summary>
    /// <returns>The container's stored value as a 64-bit integer</returns>
    public: NUCLEX_THINORM_API std::optional<std::int64_t> ToInt64() const;
    /// <summary>Returns the value in the container as a fixed point decimal</summary>
    /// <returns>The container's stored value as a fixed point decimal</returns>
    public: NUCLEX_THINORM_API std::optional<Decimal> ToDecimal() const;
    /// <summary>Returns the value in the container as a 32-bit floating point value</summary>
    /// <returns>The container's stored value as a 32-bit floating point value</returns>
    public: NUCLEX_THINORM_API std::optional<float> ToFloat() const;
    /// <summary>Returns the value in the container as a 64-bit floating point value</summary>
    /// <returns>The container's stored value as a 64-bit floating point value</returns>
    public: NUCLEX_THINORM_API std::optional<double> ToDouble() const;
    /// <summary>Returns the value in the container as an UTF-8 string</summary>
    /// <returns>The container's stored value as an UTF-8 string</returns>
    public: NUCLEX_THINORM_API std::optional<std::u8string> ToString() const;
    /// <summary>Returns the value in the container as a binary blob</summary>
    /// <returns>The container's stored value as a binary blob</returns>
    public: NUCLEX_THINORM_API std::optional<std::vector<std::byte>> ToBlob() const;

    /// <summary>Clones the value assumed by another value container</summary>
    /// <param name="other">Other value container whose contents will be cloned</param>
    public: NUCLEX_THINORM_API Value &operator =(const Value &other);
    /// <summary>Adopts the value from another value container</summary>
    /// <param name="other">Other value container whose contents will be adopted</param>
    public: NUCLEX_THINORM_API Value &operator =(Value &&other);

    /// <summary>Sets the stored value to a boolean value</summary>
    /// <param name="booleanValue">Boolean that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<bool> booleanValue);
    /// <summary>Sets the stored value to an 8-bit integer value</summary>
    /// <param name="uint8Value">8-bit integer value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::uint8_t> uint8Value);
    /// <summary>Sets the stored value to a 16-bit integer value</summary>
    /// <param name="int16Value">16-bit integer value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::int16_t> int16Value);
    /// <summary>Sets the stored value to a 32-bit integer value</summary>
    /// <param name="int32Value">32-bit integer value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::int32_t> int32Value);
    /// <summary>Sets the stored value to a 64-bit integer value</summary>
    /// <param name="int642Value">64-bit integer value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::int64_t> int64Value);
    /// <summary>Sets the stored value to a fixed point decimal value</summary>
    /// <param name="decimalValue">Fixed point decimal value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<Decimal> decimalValue);
    /// <summary>Sets the stored value to a 32-bit floating point value</summary>
    /// <param name="floatValue">32-bit floating point value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<float> floatValue);
    /// <summary>Sets the stored value to a 64-bit floating point value</summary>
    /// <param name="doubleValue">64-bit floating point value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<double> doubleValue);
    /// <summary>Sets the stored value to a UTF-8 string value</summary>
    /// <param name="stringValue">UTF-8 string value that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::u8string> stringValue);
    /// <summary>Sets the stored value to a binary blob</summary>
    /// <param name="blobValue">Binary blob that will be stored</param>
    public: NUCLEX_THINORM_API Value &operator =(std::optional<std::vector<std::byte>> blobValue);

    /// <summary>Reads the stored value as a non-nullable boolean</summary>
    /// <returns>The stored value as a boolean</returns>
    public: NUCLEX_THINORM_API explicit operator bool() const;
    /// <summary>Reads the stored value as a non-nullable 8-bit integer</summary>
    /// <returns>The stored value as an 8-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::uint8_t() const;
    /// <summary>Reads the stored value as a non-nullable 16-bit integer</summary>
    /// <returns>The stored value as a 16-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::int16_t() const;
    /// <summary>Reads the stored value as a non-nullable 32-bit integer</summary>
    /// <returns>The stored value as a 32-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::int32_t() const;
    /// <summary>Reads the stored value as a non-nullable 64-bit integer</summary>
    /// <returns>The stored value as a 64-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::int64_t() const;
    /// <summary>Reads the stored value as a non-nullable fixed point decimal</summary>
    /// <returns>The stored value as a fixed point decimal</returns>
    public: NUCLEX_THINORM_API explicit operator Decimal() const;
    /// <summary>Reads the stored value as a non-nullable 32-bit floating point value</summary>
    /// <returns>The stored value as a 32-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator float() const;
    /// <summary>Reads the stored value as a non-nullable 64-bit floating point value</summary>
    /// <returns>The stored value as a 64-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator double() const;
    /// <summary>Reads the stored value as a non-nullable UTF-8 string</summary>
    /// <returns>The stored value as a UTF-8 string</returns>
    public: NUCLEX_THINORM_API explicit operator std::u8string() const;
    /// <summary>Reads the stored value as a non-nullable blob</summary>
    /// <returns>The stored value as a blob</returns>
    public: NUCLEX_THINORM_API explicit operator std::vector<std::byte>() const;

    /// <summary>Reads the stored value as a boolean</summary>
    /// <returns>The stored value as a boolean</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<bool>() const;
    /// <summary>Reads the stored value as an 8-bit integer</summary>
    /// <returns>The stored value as an 8-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::uint8_t>() const;
    /// <summary>Reads the stored value as a 16-bit integer</summary>
    /// <returns>The stored value as a 16-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::int16_t>() const;
    /// <summary>Reads the stored value as a 32-bit integer</summary>
    /// <returns>The stored value as a 32-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::int32_t>() const;
    /// <summary>Reads the stored value as a 64-bit integer</summary>
    /// <returns>The stored value as a 64-bit integer</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::int64_t>() const;
    /// <summary>Reads the stored value as a fixed point decimal</summary>
    /// <returns>The stored value as a fixed point decimal</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<Decimal>() const;
    /// <summary>Reads the stored value as a 32-bit floating point value</summary>
    /// <returns>The stored value as a 32-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<float>() const;
    /// <summary>Reads the stored value as a 64-bit floating point value</summary>
    /// <returns>The stored value as a 64-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<double>() const;
    /// <summary>Reads the stored value as a UTF-8 string</summary>
    /// <returns>The stored value as a UTF-8 string</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::u8string>() const;
    /// <summary>Reads the stored value as a blob</summary>
    /// <returns>The stored value as a blob</returns>
    public: NUCLEX_THINORM_API explicit operator std::optional<std::vector<std::byte>>() const;

    /// <summary>Current type of value stored in the value container</summary>
    private: ValueType type;
    /// <summary>Whether the value currently stored is empty (NULL)</summary>
    private: bool empty;
    /// <summary>Actual value as a union</summary>
    private: union ValueContainer {
      /// <summary>Do-nothing constructor, needed because of the non-trivial types</summary>
      public: ValueContainer();
      /// <summary>Do-nothing destructor, needed because of the non-trivial types</summary>
      public: ~ValueContainer();
      /// <summary>Boolean value if the container stores a boolean</summary>
      public: bool BooleanValue;
      /// <summary>8-bit integer value if the container stores an 8-bit integer</summary>
      public: std::uint8_t Uint8Value;
      /// <summary>16-bit integer value if the container stores a 16-bit integer</summary>
      public: std::int16_t Int16Value;
      /// <summary>32-bit integer value if the container stores a 32-bit integer</summary>
      public: std::int32_t Int32Value;
      /// <summary>64-bit integer value if the container stores a 64-bit integer</summary>
      public: std::int64_t Int64Value;
      /// <summary>Fixed point decimal value if the container stores a decimal</summary>
      public: Decimal DecimalValue;
      /// <summary>32-bit floating point value if the container stores a float</summary>
      public: float FloatValue;
      /// <summary>64-bit floating point value if the container stores a double</summary>
      public: double DoubleValue;
      /// <summary>UTF-8 string value if the container stores a UTF-8 string</summary>
      public: std::u8string StringValue;
      /// <summary>Date and time value if the container stores a date and/or time</summary>
      public: std::tm DateTimeValue;
      /// <summary>Blob value if the container stores a blob</summary>
      public: std::vector<std::byte> BlobValue;
    } value;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_VALUE_H

