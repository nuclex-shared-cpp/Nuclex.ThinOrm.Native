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

#include "Nuclex/ThinOrm/Decimal.h"

#include <Nuclex/Support/Text/LexicalCast.h> // for lexical_cast<>

#include <cmath> // for std::pow()
#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Forms the low 64-bit integer in a 128-bit integer</summary>
  /// <param name="value">Value the low 64-bit integer should be based on</param>
  /// <returns>The value the low 64-bit integer in a 128-bit integer should be set to</returns>
  std::uint64_t makeLowInt64(std::int64_t value) {
    return static_cast<std::uint64_t>(value); // preserves bits + sign bit of negative ints
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Forms the high 64-bit integer in a 128-bit integer</summary>
  /// <param name="value">Value the high 64-bit integer should be based on</param>
  /// <returns>The value the high 64-bit integer in a 128-bit integer should be set to</returns>
  std::int64_t makeHighInt64(std::int64_t value) {
    return (value < 0) ? -1 : 0; // extend the sign into the high 64 bits
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Look-up table of the powers of then for a 64-bit integer</summary>
  constexpr std::int64_t PowersOfTen[18] = {
    10ll,
    100ll,
    1000ll,
    10000ll,
    100000ll,
    1000000ll,
    10000000ll,
    100000000ll,
    1000000000ll,
    10000000000ll,
    100000000000ll,
    1000000000000ll,
    10000000000000ll,
    100000000000000ll,
    1000000000000000ll,
    10000000000000000ll,
    100000000000000000ll,
    1000000000000000000ll,
  };  

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(const Decimal &other) noexcept :
    lowInt64(other.lowInt64),
    highInt64(other.highInt64),
    decimalDigitCount(other.decimalDigitCount) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(Decimal &&other) noexcept :
    lowInt64(other.lowInt64),
    highInt64(other.highInt64),
    decimalDigitCount(other.decimalDigitCount) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(int value) :
    lowInt64(makeLowInt64(value)),
    highInt64(makeHighInt64(value)),
    decimalDigitCount(0) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(std::int64_t value) :
    lowInt64(makeLowInt64(value)),
    highInt64(makeHighInt64(value)),
    decimalDigitCount(0) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(int value, int decimalDigitCount) :
    lowInt64(makeLowInt64(value)),
    highInt64(makeHighInt64(value)),
    decimalDigitCount(decimalDigitCount) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(std::int64_t value, int decimalDigitCount) :
    lowInt64(makeLowInt64(value)),
    highInt64(makeHighInt64(value)),
    decimalDigitCount(decimalDigitCount) {}

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(float value, int decimalDigitCount /* = 3 */) :
    lowInt64(0), // TODO: Implement decimal constructor from float
    highInt64(0),
    decimalDigitCount(decimalDigitCount) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  Decimal::Decimal(double value, int decimalDigitCount /* = 3 */) :
    lowInt64(0), // TODO: Implement decimal constructor from double
    highInt64(0),
    decimalDigitCount(decimalDigitCount) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  int Decimal::RoundToInt() const noexcept {
    // TODO: Implement reasonable decimal-to-int (with rounding) conversion
    return static_cast<int>(ToDouble());
  }

  // ------------------------------------------------------------------------------------------- //

  int Decimal::ToInt() const {
    // TODO: Implement reasonable decimal-to-int (with truncation) conversion
    return static_cast<int>(ToDouble());
  }

  // ------------------------------------------------------------------------------------------- //

  float Decimal::ToFloat() const {
    // TODO: Implement reasonable decimal-to-float conversion
    return static_cast<float>(ToDouble());
  }

  // ------------------------------------------------------------------------------------------- //

  double Decimal::ToDouble() const {
    // TODO: Implement reasonable decimal-to-double conversion
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string Decimal::ToString() const {
    // TODO: Implement decimal-to-string conversion
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
    /*
    std::u8string result = (
      Nuclex::Support::Text::lexical_cast<std::u8string>(this->fixedPointValue)
    );

    std::u8string::size_type length = result.length();
    if(length >= this->decimalDigitCount) {
      result = std::u8string(this->decimalDigitCount + 1 - length, u8'0') + result;
      length = this->decimalDigitCount + 1;
    }

    result.insert(result.begin() + length - this->decimalDigitCount, u8'.');
    */
  }

  // ------------------------------------------------------------------------------------------- //

  Decimal &Decimal::operator =(const Decimal &other) noexcept {
    this->lowInt64 = other.lowInt64;
    this->highInt64 = other.highInt64;
    this->decimalDigitCount = other.decimalDigitCount;
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Decimal &Decimal::operator =(Decimal &&other) noexcept {
    this->lowInt64 = other.lowInt64;
    this->highInt64 = other.highInt64;
    this->decimalDigitCount = other.decimalDigitCount;
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //
/*
  Decimal &Decimal::operator =(int value) noexcept;

  // ------------------------------------------------------------------------------------------- //

  Decimal &Decimal::operator =(float value) noexcept;

  // ------------------------------------------------------------------------------------------- //

  Decimal &Decimal::operator =(double value) noexcept;
*/
  // ------------------------------------------------------------------------------------------- //
/*
  public: NUCLEX_THINORM_API explicit operator int() const;

  // ------------------------------------------------------------------------------------------- //

  public: NUCLEX_THINORM_API explicit operator float() const;

  // ------------------------------------------------------------------------------------------- //

  public: NUCLEX_THINORM_API explicit operator double() const;

  // ------------------------------------------------------------------------------------------- //

  public: NUCLEX_THINORM_API explicit operator std::u8string() const;
*/
  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
