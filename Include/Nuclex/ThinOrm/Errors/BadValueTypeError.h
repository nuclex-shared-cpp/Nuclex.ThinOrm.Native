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

#ifndef NUCLEX_THINORM_ERRORS_BADVALUETYPEERROR_H
#define NUCLEX_THINORM_ERRORS_BADVALUETYPEERROR_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <variant> // for std::bad_variant_access

namespace Nuclex::ThinOrm::Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Indicates that a <see cref="Value" /> was of the wrong type</summary>
  /// <remarks>
  ///   The <see cref="Value" /> class is intended to store the results of queries executed
  ///   on a database. Depending on your preferences, you can coerce the values to your
  ///   desired type or you can require it to be of an expected type. In the latter case,
  ///   this exception is thrown to indicate when a <see cref="Value" /> was of a type that
  ///   did not meet the expectation. It may indicate that a database schema is different
  ///   from the one the application expects but can also be mere programmer error.
  /// </remarks>
  class NUCLEX_THINORM_TYPE BadValueTypeError : public std::bad_variant_access {

    /// <summary>Initializes a bad value type error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadValueTypeError(const std::u8string &message) noexcept;

    /// <summary>Initializes a bad value type error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadValueTypeError(const char8_t *message) noexcept;

    /// <summary>Retrieves an error message describing the value type mismatch</summary>
    /// <returns>A message describing the mismatch on the value type</returns>>
    public: NUCLEX_THINORM_API virtual const char *what() const noexcept override;

    /// <summary>Error message describing the reason for the error</summary>
    private: std::u8string message;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Errors

#endif // NUCLEX_THINORM_ERRORS_BADVALUETYPEERROR_H
