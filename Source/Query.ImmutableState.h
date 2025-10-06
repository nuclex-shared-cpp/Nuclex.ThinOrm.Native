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

#ifndef NUCLEX_THINORM_QUERY_IMMUTABLESTATE_H
#define NUCLEX_THINORM_QUERY_IMMUTABLESTATE_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Query.h"

#include <atomic> // for std::atomic

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Private implementation of the Query class</summary>
  class Query::ImmutableState {

    /// <summary>Initializes the implementation details for a query</summary>
    /// <param name="sqlStatement">SQL statement the query will execute</param>
    public: ImmutableState(const std::u8string &sqlStatement);
    /// <summary>Initializes the implementation details for a query</summary>
    public: ImmutableState(const ImmutableState &other);
    /// <summary>Initializes the implementation details for a query</summary>
    public: ImmutableState(ImmutableState &&other);
    /// <summary>Frees all resources owned by the implementation details</summary>
    public: ~ImmutableState();

    /// <summary>Retrieves the SQL statement the query was build for</summary>
    /// <returns>The SQL statement the query will execute</returns>
    public: inline const std::u8string &GetSqlStatement() const;

    /// <summary>Returns the unique id of the SQL statement for caching</summary>
    /// <returns>A unique ID that will stay the same for cloned queries</returns>
    public: inline std::size_t GetSqlStatementId() const;

    /// <summary>
    ///   Retrieves the names and locations of parameter placeholders in the SQL statement
    /// </summary>
    /// <returns>
    ///   A list of the name and location of each parameter in the SQL statement
    /// </returns>
    public: inline const std::vector<QueryParameterView> &GetParameterInfo() const;

    /// <summary>Stores names and locations of parameters</summary>
    private: typedef std::vector<QueryParameterView> ParameterViewVector;

    /// <summary>Generator that is consulted to obtain a unique ID for a query</summary>
    private: static std::atomic<std::size_t> nextUniqueId;

    /// <summary>SQL statement the query will execute</summary>
    private: std::u8string sqlStatement;
    /// <summary>Unique id of the SQL statement</summary>
    private: std::size_t sqlStatementId;
    /// <summary>Names and locations of the query parameters in the query string</summary>
    private: ParameterViewVector parameters;

  };

  // ------------------------------------------------------------------------------------------- //

  inline const std::u8string &Query::ImmutableState::GetSqlStatement() const {
    return this->sqlStatement;
  }

  // ------------------------------------------------------------------------------------------- //

  inline std::size_t Query::ImmutableState::GetSqlStatementId() const {
    return this->sqlStatementId;
  }

  // ------------------------------------------------------------------------------------------- //

  const std::vector<QueryParameterView> &Query::ImmutableState::GetParameterInfo() const {
    return this->parameters;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_QUERY_IMMUTABLESTATE_H
