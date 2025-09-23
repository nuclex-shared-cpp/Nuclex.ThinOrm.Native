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

#ifndef NUCLEX_THINORM_UTILITIES_QUANTIZER_H
#define NUCLEX_THINORM_UTILITIES_QUANTIZER_H

#include "Nuclex/ThinOrm/Config.h"

#include <cstdint> // for std::int32_t
#include <cmath> // for std::sgn

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Helper methods to convert floating point values to quantized integers</summary>
  /// <remarks>
  ///   This simply ensures predictable rounding independend of the rounding mode
  ///   configured for the CPU (though it's of course a little slower).
  /// </remarks>
  class Quantizer {

    /// <summary>Rounds a floating point value to the nearest integer</summary>
    /// <param name="value">Floating point value that will be rounded</param>
    /// <returns>The 32-bit integer nearest to the floating point value</returns>
    public: static inline std::int32_t NearestInt32(float value);

    /// <summary>Rounds a floating point value to the nearest integer</summary>
    /// <param name="value">Floating point value that will be rounded</param>
    /// <returns>The 32-bit integer nearest to the floating point value</returns>
    public: static inline std::int32_t NearestInt32(double value);

  };

  // ------------------------------------------------------------------------------------------- //

  inline std::int32_t Quantizer::NearestInt32(float value) {
    return static_cast<std::int32_t>(std::trunc(value + std::copysign(0.5f, value)));
  }

  // ------------------------------------------------------------------------------------------- //

  inline std::int32_t Quantizer::NearestInt32(double value) {
    return static_cast<std::int32_t>(std::trunc(value + std::copysign(0.5, value)));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // NUCLEX_THINORM_UTILITIES_QUANTIZER_H
