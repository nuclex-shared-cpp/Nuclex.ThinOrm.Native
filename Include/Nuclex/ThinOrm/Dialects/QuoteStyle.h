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

#ifndef NUCLEX_THINORM_DIALECTS_QUOTESTYLE_H
#define NUCLEX_THINORM_DIALECTS_QUOTESTYLE_H

#include "Nuclex/ThinOrm/Config.h"

namespace Nuclex::ThinOrm::Dialects {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Quote style that should be used around identifiers (table and column names)
  /// </summary>
  enum class QuoteStyle {

    /// <summary>Identifiers can be put in double quotes</summary>
    /// <remarks>
    ///   <para>
    ///     This is the ANSI SQL standard. Identifiers such as table names and column names
    ///     can be put in double quotes to allow for spaces and other characters that would
    ///     ordinarely interfere with the SQL syntax. Actual quotes appearing in an identifier
    ///     should be escaped via a repeated double quote.
    ///   </para>
    ///   <para>
    ///     Unfortunately, it's not always supported: Microsoft SQL Server defaults to
    ///     brackets around identifiers and MySQL / MariaDB default to backticks. Both can
    ///     be configured to honor the SQL standard, however.
    ///   </para>
    /// </remarks>
    DoubleQuotes,

    /// <summary>Identifier can be put in brackets</summary>
    /// <remarks>
    ///   This is only a convention on Microsoft SQL Server and not understood by any other
    ///   database, except for SQLite, which accepts these as a compatibility feature.
    /// </remarks>
    Brackets,

    /// <summary>Identifier can be put in backticks</summary>
    /// <remarks>
    ///   This is only a convention on MySQL / MariaDB and not understood by any other
    ///   database, except for SQLite, which accepts these as a compatibility feature.
    /// </remarks>
    Backticks

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Dialects

#endif // NUCLEX_THINORM_DIALECTS_QUOTESTYLE_H
