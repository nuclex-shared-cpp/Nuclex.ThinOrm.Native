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

#ifndef NUCLEX_THINORM_ERRORS_BADDATEFORMATERROR_H
#define NUCLEX_THINORM_ERRORS_BADDATEFORMATERROR_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <variant> // for std::bad_variant_access

namespace Nuclex::ThinOrm::Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Indicates that a date/time value did not match the required format</summary>
  class NUCLEX_THINORM_TYPE BadDateFormatError : public std::bad_variant_access {

    /// <summary>Initializes a bad date format error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadDateFormatError(const std::u8string &message) noexcept;

    /// <summary>Initializes a bad date format error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadDateFormatError(const char8_t *message) noexcept;

    /// <summary>Retrieves an error message describing the date format error</summary>
    /// <returns>A message describing the error on the date format</returns>>
    public: NUCLEX_THINORM_API virtual const char *what() const noexcept override;

    /// <summary>Error message describing the reason for the error</summary>
    private: std::u8string message;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Errors

#endif // NUCLEX_THINORM_ERRORS_BADDATEFORMATERROR_H
