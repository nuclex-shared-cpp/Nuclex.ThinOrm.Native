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

#include <cassert> // for assert()

#include "Nuclex/ThinOrm//Errors/BadValueTypeError.h" // for BadValueTypeError

// Turns a C++20 UTF-8 char8_t array (u8"") into a plain char array
//
// Why? Because using u8 expresses that the text should be stored as UTF-8,
// regardless of how the source file is encoded.
//
// Safe? Yes because any type in C++ can be aliased as a char sequence. In fact, this
// reinterpret_cast is shown as a correct example in the char8_t addition to
// the C++ standard. The opposite way (char * to char8_t *) invokes UB, though.
#define U8CHARS(x) (reinterpret_cast<const char *>(x))

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

#if defined(_MSC_VER)
  [[gsl::suppress("type.6")]] // uninitialized members -- intentional, union is managed by outer
#endif
  Value::ValueContainer::ValueContainer() noexcept {}

  // ------------------------------------------------------------------------------------------- //

  Value::ValueContainer::~ValueContainer() noexcept {}
  
  // ------------------------------------------------------------------------------------------- //

  Value::Value(const Value &other) noexcept :
    type(other.type),
    empty(other.empty),
    value() {

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.Boolean = other.value.Boolean; break; }
        case ValueType::UInt8: { this->value.Uint8 = other.value.Uint8; break; }
        case ValueType::Int16: { this->value.Int16 = other.value.Int16; break; }
        case ValueType::Int32: { this->value.Int32 = other.value.Int32; break; }
        case ValueType::Int64: { this->value.Int64 = other.value.Int64; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(other.value.DecimalValue); break;
        }
        case ValueType::Float: { this->value.Float = other.value.Float; break; }
        case ValueType::Double: { this->value.Double = other.value.Double; break; }
        case ValueType::String: {
          new(&this->value.String) std::u8string(other.value.String); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: {
          new(&this->value.DateTimeValue) DateTime(other.value.DateTimeValue); break;
        }
        case ValueType::Blob: {
          new(&this->value.Blob) std::vector<std::byte>(other.value.Blob); break;
        }
        default: { assert(!U8CHARS(u8"Unsupported value type")); }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(Value &&other) noexcept :
    type(other.type),
    empty(other.empty),
    value() {

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.Boolean = other.value.Boolean; break; }
        case ValueType::UInt8: { this->value.Uint8 = other.value.Uint8; break; }
        case ValueType::Int16: { this->value.Int16 = other.value.Int16; break; }
        case ValueType::Int32: { this->value.Int32 = other.value.Int32; break; }
        case ValueType::Int64: { this->value.Int64 = other.value.Int64; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(std::move(other.value.DecimalValue)); break;
        }
        case ValueType::Float: { this->value.Float = other.value.Float; break; }
        case ValueType::Double: { this->value.Double = other.value.Double; break; }
        case ValueType::String: {
          new(&this->value.String) std::u8string(std::move(other.value.String)); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: {
          new(&this->value.DateTimeValue) DateTime(std::move(other.value.DateTimeValue)); break;
        }
        case ValueType::Blob: {
          new(&this->value.Blob) std::vector<std::byte>(std::move(other.value.Blob)); break;
        }
        default: { assert(!U8CHARS(u8"Unsupported value type")); }
      }

      other.empty = true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(bool booleanValue) noexcept :
    type(ValueType::Boolean),
    empty(false),
    value() {
    this->value.Boolean = booleanValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(std::uint8_t uint8Value) noexcept :
    type(ValueType::UInt8),
    empty(false),
    value() {
    this->value.Uint8 = uint8Value;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(std::int16_t int16Value) noexcept :
    type(ValueType::Int16),
    empty(false),
    value() {
    this->value.Int16 = int16Value;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(std::int32_t int32Value) noexcept :
    type(ValueType::Int32),
    empty(false),
    value() {
    this->value.Int32 = int32Value;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(std::int64_t int64Value) noexcept :
    type(ValueType::Int64),
    empty(false),
    value() {
    this->value.Int64 = int64Value;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const Decimal &decimalValue) noexcept :
    type(ValueType::Decimal),
    empty(false),
    value() {
    this->value.DecimalValue = decimalValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(float floatValue) noexcept :
    type(ValueType::Float),
    empty(false),
    value() {
    this->value.Float = floatValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(double doubleValue) noexcept :
    type(ValueType::Double),
    empty(false),
    value() {
    this->value.Double = doubleValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::u8string &stringValue) noexcept :
    type(ValueType::String),
    empty(false),
    value() {
    this->value.String = stringValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::vector<std::byte> &blobValue) noexcept :
    type(ValueType::Blob),
    empty(false),
    value() {
    this->value.Blob = blobValue;
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<bool> &booleanValue) noexcept :
    type(ValueType::Boolean),
    empty(!booleanValue.has_value()),
    value() {
    if(!empty) {
      this->value.Boolean = booleanValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::uint8_t> &uint8Value) noexcept :
    type(ValueType::UInt8),
    empty(!uint8Value.has_value()),
    value() {
    if(!empty) {
      this->value.Uint8 = uint8Value.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::int16_t> &int16Value) noexcept :
    type(ValueType::Int16),
    empty(!int16Value.has_value()),
    value() {
    if(!empty) {
      this->value.Int16 = int16Value.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::int32_t> &int32Value) noexcept :
    type(ValueType::Int32),
    empty(!int32Value.has_value()),
    value() {
    if(!empty) {
      this->value.Int32 = int32Value.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::int64_t> &int64Value) noexcept :
    type(ValueType::Int64),
    empty(!int64Value.has_value()),
    value() {
    if(!empty) {
      this->value.Int64 = int64Value.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<Decimal> &decimalValue) noexcept :
    type(ValueType::Decimal),
    empty(!decimalValue.has_value()),
    value() {
    if(!empty) {
      this->value.DecimalValue = decimalValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<float> &floatValue) noexcept :
    type(ValueType::Float),
    empty(!floatValue.has_value()),
    value() {
    if(!empty) {
      this->value.Float = floatValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<double> &doubleValue) noexcept :
    type(ValueType::Double),
    empty(!doubleValue.has_value()),
    value() {
    if(!empty) {
      this->value.Double = doubleValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::u8string> &stringValue) noexcept :
    type(ValueType::String),
    empty(!stringValue.has_value()),
    value() {
    if(!empty) {
      this->value.String = stringValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::vector<std::byte>> &blobValue) noexcept :
    type(ValueType::Blob),
    empty(!blobValue.has_value()),
    value() {
    if(!empty) {
      this->value.Blob = blobValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::~Value() noexcept {
    if(!this->empty) [[likely]] {
      switch(this->type) {
        case ValueType::Decimal: { this->value.DecimalValue.~Decimal(); break; }
        case ValueType::String: { this->value.String.~basic_string(); break; }
        case ValueType::Blob: { this->value.Blob.~vector(); break; }
        default: break;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void Value::Require(ValueType requiredType) const {
    if(this->type != requiredType) [[unlikely]] {
      throw Errors::BadValueTypeError(u8"Value was not of the expected type");
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void Value::Require(ValueType requiredType, bool requiredPresence) const {
    if(this->type != requiredType) [[unlikely]] {
      throw Errors::BadValueTypeError(u8"Value was not of the expected type");
    }
    if(this->empty == requiredPresence) [[unlikely]] {
      if(this->empty) {
        throw Errors::BadValueTypeError(u8"Value was empty but should have been present");
      } else {
        throw Errors::BadValueTypeError(u8"Value was present but should have been empty");
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(const Value &other) noexcept {
    this->~Value();

    // Here, 'this->empty' will be 'true' after the destructor call,
    // which is ideal in case a copy constructor throws an exception.
    // We'll only set it at the end of the method if we filled in a value.

    this->type = other.type;

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.Boolean = other.value.Boolean; break; }
        case ValueType::UInt8: { this->value.Uint8 = other.value.Uint8; break; }
        case ValueType::Int16: { this->value.Int16 = other.value.Int16; break; }
        case ValueType::Int32: { this->value.Int32 = other.value.Int32; break; }
        case ValueType::Int64: { this->value.Int64 = other.value.Int64; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(other.value.DecimalValue); break;
        }
        case ValueType::Float: { this->value.Float = other.value.Float; break; }
        case ValueType::Double: { this->value.Double = other.value.Double; break; }
        case ValueType::String: {
          new(&this->value.String) std::u8string(other.value.String); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: {
          new(&this->value.DateTimeValue) DateTime(other.value.DateTimeValue); break;
        }
        case ValueType::Blob: {
          new(&this->value.Blob) std::vector<std::byte>(other.value.Blob); break;
        }
        default: { assert(!U8CHARS(u8"Unsupported value type")); }
      }

      this->empty = other.empty;
    }

    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(Value &&other) noexcept {
    this->~Value();

    // Here, 'this->empty' will be 'true' after the destructor call,
    // which is ideal in case a copy constructor throws an exception.
    // We'll only set it at the end of the method if we filled in a value.

    this->type = other.type;

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.Boolean = other.value.Boolean; break; }
        case ValueType::UInt8: { this->value.Uint8 = other.value.Uint8; break; }
        case ValueType::Int16: { this->value.Int16 = other.value.Int16; break; }
        case ValueType::Int32: { this->value.Int32 = other.value.Int32; break; }
        case ValueType::Int64: { this->value.Int64 = other.value.Int64; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(std::move(other.value.DecimalValue)); break;
        }
        case ValueType::Float: { this->value.Float = other.value.Float; break; }
        case ValueType::Double: { this->value.Double = other.value.Double; break; }
        case ValueType::String: {
          new(&this->value.String) std::u8string(std::move(other.value.String)); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: {
          new(&this->value.DateTimeValue) DateTime(std::move(other.value.DateTimeValue)); break;
        }
        case ValueType::Blob: {
          new(&this->value.Blob) std::vector<std::byte>(std::move(other.value.Blob)); break;
        }
        default: { assert(!U8CHARS(u8"Unsupported value type")); }
      }

      this->empty = other.empty;
      other.empty = true;
    }

    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<bool> booleanValue) noexcept {
    this->~Value();
    this->type = ValueType::Boolean;
    if(booleanValue.has_value()) [[likely]] {
      this->value.Boolean = booleanValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::uint8_t> uint8Value) noexcept {
    this->~Value();
    this->type = ValueType::UInt8;
    if(uint8Value.has_value()) [[likely]] {
      this->value.Uint8 = uint8Value.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::int16_t> int16Value) noexcept {
    this->~Value();
    this->type = ValueType::Int16;
    if(int16Value.has_value()) [[likely]] {
      this->value.Int16 = int16Value.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::int32_t> int32Value) noexcept {
    this->~Value();
    this->type = ValueType::Int32;
    if(int32Value.has_value()) [[likely]] {
      this->value.Int32 = int32Value.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::int64_t> int64Value) noexcept {
    this->~Value();
    this->type = ValueType::Int64;
    if(int64Value.has_value()) [[likely]] {
      this->value.Int64 = int64Value.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<Decimal> decimalValue) noexcept {
    this->~Value();
    this->type = ValueType::Decimal;
    if(decimalValue.has_value()) [[likely]] {
      this->value.DecimalValue = decimalValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<float> floatValue) noexcept {
    this->~Value();
    this->type = ValueType::Float;
    if(floatValue.has_value()) [[likely]] {
      this->value.Float = floatValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<double> doubleValue) noexcept {
    this->~Value();
    this->type = ValueType::Double;
    if(doubleValue.has_value()) [[likely]] {
      this->value.Double = doubleValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::u8string> stringValue) noexcept {
    this->~Value();
    this->type = ValueType::String;
    if(stringValue.has_value()) [[likely]] {
      this->value.String = stringValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Value &Value::operator =(std::optional<std::vector<std::byte>> blobValue) noexcept {
    this->~Value();
    this->type = ValueType::Blob;
    if(blobValue.has_value()) [[likely]] {
      this->value.Blob = blobValue.value();
      this->empty = false;
    } else {
      this->empty = true;
    }
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
