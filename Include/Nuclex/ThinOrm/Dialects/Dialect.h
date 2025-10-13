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

#ifndef NUCLEX_THINORM_DIALECTS_DIALECT_H
#define NUCLEX_THINORM_DIALECTS_DIALECT_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Dialects/QuoteStyle.h"
#include "Nuclex/ThinOrm/Dialects/DateTimeDialect.h"

namespace Nuclex::ThinOrm::Dialects {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Describes the SQL dialect spoken by a database engine</summary>
  class NUCLEX_THINORM_TYPE Dialect {

    /// <summary>Specifics of how the database engine handles date and time</summary>
    public: NUCLEX_THINORM_API DateTimeDialect DateTimeStyle;

    /// <summary>How identifiers (table and column names) can be quoted</summary>
    /// <remarks>
    ///   Generally, all alphanumeric ASCII names are fine so long as they don't contain
    ///   whitespace, line breaks, punctuation or such. Unusual or localized identifiers
    ///   can be put in quotes. This property specifies how those quotes should look.
    /// </remarks>
    public: NUCLEX_THINORM_API QuoteStyle IdentifierQuoteStyle;

    // Should we even expose this? It should be the user's job to match the casing and
    // otherwise, they messed up. We might actually verify casing rather than add crudges
    // that grant the user the ability to be lax about casing.
    //public: NUCLEX_THINORM_API bool CaseSensitiveIdentifiers;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Dialects

#endif // NUCLEX_THINORM_DIALECTS_QUOTESTYLE_H
