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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_THINORM_SOURCE 1

#include "./Query.ImmutableState.h"

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Parses the named parameters from the SQL statement</summary>
  /// <param name="sqlStatement">SQL statement containing query parameters</param>
  /// <returns>A list of query parameters covering their names and locations</returns>
  std::vector<Nuclex::ThinOrm::QueryParameterView> parseQueryParameters(
    const std::u8string &sqlStatement
  ) {
    using Nuclex::ThinOrm::QueryParameterView;
    std::vector<QueryParameterView> result;

    // Scan for all opening braces. Those that stand alone indicate parameters.
    std::u8string::size_type length = sqlStatement.length();
    std::u8string::size_type index = sqlStatement.find(u8'{');
    while(index != std::u8string::npos) {
      QueryParameterView current;
      current.StartIndex = index;

      // Prepare to scan for the end. If the next character in the string is a repeated
      // brace, then this is an escaped brace and we've already found the end.
      ++index;
      if(index >= length) [[unlikely]] {
        break; // If the opening brace was at the end of the string, ignore it
      }

      // Was the brace repeated? In that case, it's an escaped brace that should be
      // treated as an intentional, literal brace appearing in the SQL statement
      if(sqlStatement[index] == u8'{') [[unlikely]] {
        ++index;
        if(index >= length) [[unlikely]] {
          break; // If the opening brace was at the end of the string, ignore it
        }
      } else {
        index = sqlStatement.find(u8'}', index);
        if(index == std::u8string::npos) [[unlikely]] {
          break; // Brace opened but not closed, ignore it
        }

        // We could check for and ignore escaped closing braces here, but that would
        // only be resonable if the user wanted parameter names like 'some}parameter'
        // with curly braces in them and ti would take another look in here, so using
        // curly braces for parameter names is simply not allowed.
        current.Length = index - current.StartIndex + 1;
        current.Name = std::u8string_view(
          sqlStatement.data() + current.StartIndex + 1, current.Length - 2
        );
        result.push_back(current);

        // Move to the next character after the closing brace
        ++index;
        if(index >= length) [[unlikely]] {
          break; // Closing brace was last character in SQL statement
        }
      }

      // Scan for the next opening brace
      index = sqlStatement.find(u8'{', index);
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Query::ImmutableState::ImmutableState(const std::u8string &sqlStatement) :
    sqlStatement(sqlStatement),
    parameters(parseQueryParameters(sqlStatement)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::ImmutableState::ImmutableState(const ImmutableState &other) :
    sqlStatement(other.sqlStatement),
    parameters(other.parameters) {}

  // ------------------------------------------------------------------------------------------- //

  Query::ImmutableState::ImmutableState(ImmutableState &&other) :
    sqlStatement(std::move(other.sqlStatement)),
    parameters(std::move(other.parameters)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::ImmutableState::~ImmutableState() = default;

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
