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

#ifndef NUCLEX_THINORM_ERRORS_BADSQLSTATEMENTERROR_H
#define NUCLEX_THINORM_ERRORS_BADSQLSTATEMENTERROR_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string
#include <stdexcept> // for std::invalid_argument

namespace Nuclex::ThinOrm::Errors {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Indicates that an SQL statement you specified had an error and was not understood.
  /// </summary>
  /// <remarks>
  ///   <para>
  ///     This can be caused by many things. The most obvious case is an actual syntax error,
  ///     such as a typo or forgetting to close a brace. Another potential cause is if you
  ///     concatenate table names with non-ASCII characters that may have to be quoted in
  ///     different fashions depending on the SQL dialect spoken by the target database.
  ///   </para>
  ///   <para>
  ///     This is treated as a <code>runtime_error</code> because it can involve user issues,
  ///     too, such as configuring the application for a database server it wasn't tested
  ///     against, thus leading to this error without the application code being wrong.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE BadSqlStatementError : public std::runtime_error {

    /// <summary>Initializes a bad parameter name error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadSqlStatementError(
      const std::u8string &message
    ) noexcept;

    /// <summary>Initializes a bad parameter name type error</summary>
    /// <param name="message">Message that describes the error</param>
    public: NUCLEX_THINORM_API explicit BadSqlStatementError(
      const char8_t *message
    ) noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Errors

#endif // NUCLEX_THINORM_ERRORS_BADSQLSTATEMENTERROR_H
