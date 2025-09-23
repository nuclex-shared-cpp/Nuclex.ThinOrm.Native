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

#ifndef NUCLEX_THINORM_QUERYPARAMETERVIEW_H
#define NUCLEX_THINORM_QUERYPARAMETERVIEW_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Indicates the name and characters of a parameter placeholder inside an SQL statement
  /// </summary>
  /// <remarks>
  ///   This is not part of the mechanism to assign values to parameters (you can find that
  ///   inside of the <see cref="Query" /> class). This type is used to store where, within
  ///   an SQL statement, parameter placeholders are located. This information is then used
  ///   by conrete implementations of the <see cref="Connection" /> class to transform our
  ///   generic placeholders (&quot;{parameter}&quot;) into placeholders specific to
  ///   the SQL dialect spoken by a database engine (i.e. &quot;:1&quot; or
  ///   &quot;@parameter&quot;).
  /// </remarks>
  struct NUCLEX_THINORM_TYPE QueryParameterView {

    /// <summary>Name of the parameter</summary>
    public: std::u8string_view Name;

    /// <summary>Character index in the string as which the segment begins</summary>
    /// <remarks>
    ///   This is an index into the overall query string. It cannot be used with
    ///   the <see cref="Name" /> string_view.
    /// </remarks>
    public: std::u8string::size_type StartIndex;

    /// <summary>Length of the segment in characters</summary>
    public: std::u8string::size_type Length;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_QUERYPARAMETERVIEW_H
