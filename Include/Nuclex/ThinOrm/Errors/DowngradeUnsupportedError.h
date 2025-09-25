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
#include <stdexcept> // for std::logic_error

namespace Nuclex::ThinOrm::Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Indicates that a migration cannot be reversed</summary>
  class NUCLEX_THINORM_TYPE DowngradeUnsupportedError : public std::logic_error {

    /// <summary>Initializes a downgrade unsupported error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit DowngradeUnsupportedError(
      const std::u8string &message
    ) noexcept;

    /// <summary>Initializes a downgrade unsupported error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit DowngradeUnsupportedError(const char8_t *message) noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Errors

#endif // NUCLEX_THINORM_ERRORS_BADVALUETYPEERROR_H
