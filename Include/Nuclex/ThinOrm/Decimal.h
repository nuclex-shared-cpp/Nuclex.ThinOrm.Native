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

#ifndef NUCLEX_THINORM_DECIMAL_H
#define NUCLEX_THINORM_DECIMAL_H

#include "Nuclex/ThinOrm/Config.h"

#include <cstdint> // for std::int16_t, std::int32_t, etc.
#include <string> // for std::u8string

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Fixed-point decimal value</summary>
  class NUCLEX_THINORM_TYPE Decimal {

    /// <summary>Initializes a new decimal as a copy of an existing decimal</summary>
    /// <param name="other">Existing value that will be cloned</param>
    public: NUCLEX_THINORM_API Decimal(const Decimal &other) noexcept;
    /// <summary>Initializes a new decimal by taking over an existing decimal</summary>
    /// <param name="other">Existing decimal that will be adopted</param>
    public: NUCLEX_THINORM_API Decimal(Decimal &&other) noexcept;

    /// <summary>Initializes a new decimal from the specified integer</summary>
    /// <param name="value">Integer whose value the decimal will assume</param>
    public: NUCLEX_THINORM_API Decimal(int value);
    /// <summary>Initializes a new decimal from the specified integer</summary>
    /// <param name="value">Integer whose value the decimal will be based on</param>
    /// <param name="decimalPlaces">
    ///   Number of decimal places to move the integer to the right (i.e. if you
    ///   specify 3 here, the integer would be treated as if it was divided by 10
    ///   three times)
    /// </param>
    public: NUCLEX_THINORM_API Decimal(int value, int decimalPlaces);
    /// <summary>Initializes a new decimal from the specified 32-bit float</summary>
    /// <param name="value">Floating point value the decimal will be set to</param>
    /// <param name="decimalPlaces">Number of decimal places to preserve in the float</param>
    public: NUCLEX_THINORM_API Decimal(float value, int decimalPlaces = 3);
    /// <summary>Initializes a new decimal from the specified 64-bit float</summary>
    /// <param name="value">Floating point value the decimal will be set to</param>
    /// <param name="decimalPlaces">Number of decimal places to preserve in the float</param>
    public: NUCLEX_THINORM_API Decimal(double value, int decimalPlaces = 3);

    /// <summary>Checks whether the decimal has the value 0</summary>
    /// <returns>True if the decimal contains the value 0</returns>
    public: NUCLEX_THINORM_API bool IsZero() const noexcept;

    /// <summary>Returns the value of the decimal rounded to the nearest integer</summary>
    /// <returns>True decimal rounded to the nearest full integer</returns>
    public: NUCLEX_THINORM_API int RoundToInt() const noexcept;

    /// <summary>Returns the value in the container as an integer</summary>
    /// <returns>The container's stored value as an integer</returns>
    public: NUCLEX_THINORM_API int ToInt() const;
    /// <summary>Returns the value in the container as a 32-bit floating point value</summary>
    /// <returns>The container's stored value as a 32-bit floating point value</returns>
    public: NUCLEX_THINORM_API float ToFloat() const;
    /// <summary>Returns the value in the container as a 64-bit floating point value</summary>
    /// <returns>The container's stored value as a 64-bit floating point value</returns>
    public: NUCLEX_THINORM_API double ToDouble() const;
    /// <summary>Returns the value in the container as an UTF-8 string</summary>
    /// <returns>The container's stored value as an UTF-8 string</returns>
    public: NUCLEX_THINORM_API std::u8string ToString() const;

    /// <summary>Clones the value assumed by another decimal</summary>
    /// <param name="other">Other decimal whose value will be cloned</param>
    public: NUCLEX_THINORM_API Decimal &operator =(const Decimal &other) noexcept;
    /// <summary>Adopts the value from another decimal</summary>
    /// <param name="other">Other decimal whose value will be adopted</param>
    public: NUCLEX_THINORM_API Decimal &operator =(Decimal &&other) noexcept;

    /// <summary>Sets the decimal's value to an integer</summary>
    /// <param name="int642Value">64-bit integer value that will be stored</param>
    public: NUCLEX_THINORM_API Decimal &operator =(int value) noexcept;
    /// <summary>Sets the decimal's value to a 32-bit floating point value</summary>
    /// <param name="floatValue">32-bit floating point value that will be stored</param>
    public: NUCLEX_THINORM_API Decimal &operator =(float value) noexcept;
    /// <summary>Sets the decimal's value to a 64-bit floating point value</summary>
    /// <param name="doubleValue">64-bit floating point value that will be stored</param>
    public: NUCLEX_THINORM_API Decimal &operator =(double value) noexcept;

    /// <summary>Reads the stored value as an integer</summary>
    /// <returns>The stored value as an integer</returns>
    public: NUCLEX_THINORM_API explicit operator int() const;
    /// <summary>Reads the stored value as a 32-bit floating point value</summary>
    /// <returns>The stored value as a 32-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator float() const;
    /// <summary>Reads the stored value as a 64-bit floating point value</summary>
    /// <returns>The stored value as a 64-bit floating point value</returns>
    public: NUCLEX_THINORM_API explicit operator double() const;
    /// <summary>Reads the stored value as a UTF-8 string</summary>
    /// <returns>The stored value as a UTF-8 string</returns>
    public: NUCLEX_THINORM_API explicit operator std::u8string() const;

  }; // TODO Implement decimal values

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_DECIMAL_H
