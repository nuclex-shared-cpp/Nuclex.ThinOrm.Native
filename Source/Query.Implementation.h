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

#ifndef NUCLEX_THINORM_QUERY_IMPLEMENTATION_H
#define NUCLEX_THINORM_QUERY_IMPLEMENTATION_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Query.h"
#include "Nuclex/ThinOrm/Value.h"

#include <Nuclex/Support/Text/StringMatcher.h>

#include <unordered_map> // for std::unordered_map
#include <mutex> // for std::mutex

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Private implementation of the Query class</summary>
  class Query::Implementation {

    /// <summary>Initializes the implementation details for a query</summary>
    public: Implementation();
    /// <summary>Initializes the implementation details for a query</summary>
    public: Implementation(const Implementation &other);
    /// <summary>Initializes the implementation details for a query</summary>
    public: Implementation(Implementation &&other);
    /// <summary>Frees all resources owned by the implementation details</summary>
    public: ~Implementation();

    /// <summary>Clears the values of all parameters</summary>
    public: void ClearParameterValues();

    /// <summary>Retrieves the value assigned to a parameter by its name</summary>
    /// <param name="name">Name of the parameter whose value to fetch</param>
    /// <returns>The current value of the parameter with the specified name</returns>
    public: const Value &GetParameterValue(const std::u8string &name) const;

    /// <summary>Sets the value assigned to a parameter by its name</summary>
    /// <param name="name">Name of the parameter whose value to set</param>
    /// <param name="value">Value to assign to the parameter</param>
    public: void SetParameterValueUnchecked(
      const std::u8string &name, const Value &value
    );

    /// <summary>Map of values indexed by the name</summary>
    private: typedef std::unordered_map<
      std::u8string, Value,
      Nuclex::Support::Text::CaseInsensitiveUtf8Hash,
      Nuclex::Support::Text::CaseInsensitiveUtf8EqualTo
    > ParameterValueMap;

    /// <summary>Mutex to synchronize state (parameters + prepared statement) updates</summary>
    private: std::mutex stateMutex;
    /// <summary>Values assigned to the parameters in the query</summary>
    private: ParameterValueMap parameterValues;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_QUERY_IMPLEMENTATION_H
