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

#include "Nuclex/ThinOrm/Query.h"
#include "./Query.Implementation.h"

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Query::Query(const std::u8string &sqlStatement) :
    sqlStatement(sqlStatement),
    stateMutex(),
    implementation(std::make_unique<Implementation>()) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Query(const Query &other) :
    sqlStatement(other.sqlStatement),
    stateMutex(),
    implementation() {} //std::make_unique<Implementation>(other.implementation)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Query(Query &&other) :
    sqlStatement(std::move(other.sqlStatement)),
    stateMutex(),
    implementation(std::move(other.implementation)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::~Query() = default;

  // ------------------------------------------------------------------------------------------- //

  const std::u8string &Query::GetSqlStatement() const {
    return this->sqlStatement;
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t Query::CountParameters() const {
    throw u8"Not implemented yet";
    //return 0; // TODO This needs basic SQL statement parsing
  }

  // ------------------------------------------------------------------------------------------- //

  const Value &Query::GetParameterValue(std::size_t index) const {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  const Value &Query::GetParameterValue(const std::u8string &name) const {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  void Query::SetParameterValue(std::size_t index, const Value &value) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  void Query::SetParameterValue(const std::u8string &name, const Value &value) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  Query &Query::operator =(const Query &other) {
    this->sqlStatement = other.sqlStatement;
    //this->implementation = std::make_unique<Implementation>(other.implementation);
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Query &Query::operator =(Query &&other) {
    this->sqlStatement = std::move(other.sqlStatement);
    this->implementation = std::move(other.implementation);
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
