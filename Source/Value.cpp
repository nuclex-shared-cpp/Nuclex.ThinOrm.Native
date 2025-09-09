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

#include <stdexcept> // for std::invalid_argument

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

  Value::ValueContainer::ValueContainer() {}

  // ------------------------------------------------------------------------------------------- //

  Value::ValueContainer::~ValueContainer() {}
  
  // ------------------------------------------------------------------------------------------- //

  Value::Value(const Value &other) :
    type(other.type),
    empty(other.empty),
    value() {

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.BooleanValue = other.value.BooleanValue; break; }
        case ValueType::UInt8: { this->value.Uint8Value = other.value.Uint8Value; break; }
        case ValueType::Int16: { this->value.Int16Value = other.value.Int16Value; break; }
        case ValueType::Int32: { this->value.Int32Value = other.value.Int32Value; break; }
        case ValueType::Int64: { this->value.Int64Value = other.value.Int64Value; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(other.value.DecimalValue); break;
        }
        case ValueType::Float: { this->value.FloatValue = other.value.FloatValue; break; }
        case ValueType::Double: { this->value.DoubleValue = other.value.DoubleValue; break; }
        case ValueType::String: {
          new(&this->value.StringValue) std::u8string(other.value.StringValue); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: { this->value.DateTimeValue = other.value.DateTimeValue; break; }
        case ValueType::Blob: {
          new(&this->value.BlobValue) std::vector<std::byte>(other.value.BlobValue); break;
        }
        default: { throw std::invalid_argument(U8CHARS(u8"Unsupported value type")); }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(Value &&other) :
    type(other.type),
    empty(other.empty),
    value() {

    if(!other.empty) [[likely]] {
      switch(other.type) {
        case ValueType::Boolean: { this->value.BooleanValue = other.value.BooleanValue; break; }
        case ValueType::UInt8: { this->value.Uint8Value = other.value.Uint8Value; break; }
        case ValueType::Int16: { this->value.Int16Value = other.value.Int16Value; break; }
        case ValueType::Int32: { this->value.Int32Value = other.value.Int32Value; break; }
        case ValueType::Int64: { this->value.Int64Value = other.value.Int64Value; break; }
        case ValueType::Decimal: {
          new(&this->value.DecimalValue) Decimal(std::move(other.value.DecimalValue)); break;
        }
        case ValueType::Float: { this->value.FloatValue = other.value.FloatValue; break; }
        case ValueType::Double: { this->value.DoubleValue = other.value.DoubleValue; break; }
        case ValueType::String: {
          new(&this->value.StringValue) std::u8string(std::move(other.value.StringValue)); break;
        }
        case ValueType::Date:
        case ValueType::Time:
        case ValueType::DateTime: { this->value.DateTimeValue = other.value.DateTimeValue; break; }
        case ValueType::Blob: {
          new(&this->value.BlobValue) std::vector<std::byte>(std::move(other.value.BlobValue)); break;
        }
        default: { throw std::invalid_argument(U8CHARS(u8"Unsupported value type")); }
      }

      other.empty = true;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<bool> booleanValue) :
    type(ValueType::Boolean),
    empty(!booleanValue.has_value()),
    value() {
    if(!empty) {
      this->value.BooleanValue = booleanValue.value();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::uint8_t> uint8Value) :
    type(ValueType::UInt8),
    empty(!uint8Value.has_value()),
    value() {
    if(!empty) {
      this->value.Uint8Value = uint8Value.value();
    }

  }

  // ------------------------------------------------------------------------------------------- //
/*
  Value::Value(const std::optional<std::int16_t> int16Value) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::int32_t> int32Value) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::int64_t> int64Value) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<Decimal> decimalValue) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<float> floatValue) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<double> doubleValue) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::u8string> stringValue) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }

  // ------------------------------------------------------------------------------------------- //

  Value::Value(const std::optional<std::vector<std::byte>> blobValue) :
    type(ValueType::Boolean),
    empty(booleanValue.empty()) {

  }
*/
  // ------------------------------------------------------------------------------------------- //

  Value::~Value() {
    if(!this->empty) [[likely]] {
      switch (type) {
        case ValueType::Decimal: { this->value.DecimalValue.~Decimal(); break; }
        case ValueType::String: { this->value.StringValue.~basic_string(); break; }
        case ValueType::Blob: { this->value.BlobValue.~vector(); break; }
        default: break;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
