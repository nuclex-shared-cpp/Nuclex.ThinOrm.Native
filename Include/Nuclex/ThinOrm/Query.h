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

#ifndef NUCLEX_THINORM_QUERY_H
#define NUCLEX_THINORM_QUERY_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/QueryParameterView.h"

#include <string> // for std::u8string
#include <memory> // for std::unique_ptr
#include <vector> // for std::vector

namespace Nuclex::ThinOrm {
  class Value;
}

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>SQL query that can be executed on a database connection</summary>
  class NUCLEX_THINORM_TYPE Query {

    /// <summary>Initializes a new query</summary>
    /// <param name="sqlStatement">SQL statement the query should run</param>
    public: NUCLEX_THINORM_API Query(const std::u8string &sqlStatement);

    /// <summary>Initializes a new query as clone of an existing query</summary>
    /// <param name="other">Existing query that will be cloned</param>
    public: NUCLEX_THINORM_API Query(const Query &other);

    /// <summary>Initializes a new query adopting the state of another query</summary>
    /// <param name="other">Existing query whose state will be adopted</param>
    public: NUCLEX_THINORM_API Query(Query &&other);

    /// <summary>Frees all resources owned by the query</summary>
    public: NUCLEX_THINORM_API ~Query();

    /// <summary>Retrieves the SQL statement the query was build for</summary>
    /// <returns>The SQL statement the query will execute</returns>
    public: NUCLEX_THINORM_API const std::u8string &GetSqlStatement() const;

    /// <summary>Counts the number of parameters in the SQL statement</summary>
    /// <returns>The number of parameters in the SQL statement</returns>
    public: NUCLEX_THINORM_API std::size_t CountParameters() const;

    /// <summary>Clears all assigned parameter values</summary>
    public: NUCLEX_THINORM_API void ClearParameterValues();

    /// <summary>
    ///   Retrieves the names and locations of parameter placeholders in the SQL statement
    /// </summary>
    /// <returns>
    ///   A list of the name and location of each parameter in the SQL statement
    /// </returns>
    /// <remarks>
    ///   This is typically not required to run queryies, but it is used by implementations
    ///   of the <see cref="Connection" /> class to preprocess the query and transform
    ///   the query's generic parameter placeholders into the notation expected by
    ///   the respective database engine.
    /// </remarks>
    public: NUCLEX_THINORM_API const std::vector<QueryParameterView> &GetParameterInfo() const;

    /// <summary>Retrieves the value assigned to a parameter by index</summary>
    /// <param name="index">Zero-based index of the parameter whose value to fetch</param>
    /// <returns>The current value of the parameter with the specified index</returns>
    public: NUCLEX_THINORM_API const Value &GetParameterValue(std::size_t index) const;

    /// <summary>Retrieves the value assigned to a parameter by its name</summary>
    /// <param name="name">Name of the parameter whose value to fetch</param>
    /// <returns>The current value of the parameter with the specified name</returns>
    public: NUCLEX_THINORM_API const Value &GetParameterValue(const std::u8string &name) const;

    /// <summary>Sets the value assigned to a parameter by index</summary>
    /// <param name="index">Zero-based index of the parameter whose value to set</param>
    /// <param name="value">Value to assign to the parameter</param>
    public: NUCLEX_THINORM_API void SetParameterValue(
      std::size_t index, const Value &value
    );

    /// <summary>Sets the value assigned to a parameter by its name</summary>
    /// <param name="name">Name of the parameter whose value to set</param>
    /// <param name="value">Value to assign to the parameter</param>
    public: NUCLEX_THINORM_API void SetParameterValue(
      const std::u8string &name, const Value &value
    );

    /// <summary>Clones the state of another query</summary>
    /// <param name="other">Other query that this query will become a clone of</param>
    /// <returns>This query</returns>
    public: NUCLEX_THINORM_API Query &operator =(const Query &other);

    /// <summary>Adopts the state of another query</summary>
    /// <param name="other">Other query whose state will be adopted</param>
    /// <returns>This query</returns>
    public: NUCLEX_THINORM_API Query &operator =(Query &&other);

    /// <summary>The part of the query that remains the same when the query is copied</summary>
    private: class ImmutableState;
    /// <summary>The part of the query that can change (i.e. basic pImpl idiom)</summary>
    private: class Implementation;

    /// <summary>Unchanging part of the query that is cached for multiple executions</summary>
    private: std::shared_ptr<const ImmutableState> immutableState;
    /// <summary>Transient state, including parameters, belonging only to one query</summary>
    private: std::unique_ptr<Implementation> implementation;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_QUERY_H
