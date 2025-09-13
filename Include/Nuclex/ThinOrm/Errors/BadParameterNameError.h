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

#ifndef NUCLEX_THINORM_ERRORS_BADPARAMETERNAMEERROR_H
#define NUCLEX_THINORM_ERRORS_BADPARAMETERNAMEERROR_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <stdexcept> // for std::invalid_argument

namespace Nuclex::ThinOrm::Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Indicates that a specified parameter name is wrong</summary>
  class NUCLEX_THINORM_TYPE BadParameterNameError : public std::invalid_argument {

    /// <summary>Initializes a bad parameter name error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadParameterNameError(const std::u8string &message) noexcept;

    /// <summary>Initializes a bad parameter name type error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadParameterNameError(const char8_t *message) noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Errors

#endif // NUCLEX_THINORM_ERRORS_BADPARAMETERNAMEERROR_H
