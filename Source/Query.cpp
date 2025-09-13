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
#include "Nuclex/ThinOrm/Errors/BadParameterNameError.h"

#include "./Query.Implementation.h"
#include "./Query.ImmutableState.h"

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Query::Query(const std::u8string &sqlStatement) :
    immutableState(std::make_shared<ImmutableState>(sqlStatement)),
    implementation(std::make_unique<Implementation>()) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Query(const Query &other) :
    immutableState(other.immutableState), // share owneship
    implementation(std::make_unique<Implementation>(*other.implementation.get())) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Query(Query &&other) :
    immutableState(std::move(other.immutableState)),
    implementation(std::move(other.implementation)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::~Query() = default;

  // ------------------------------------------------------------------------------------------- //

  const std::u8string &Query::GetSqlStatement() const {
    return this->immutableState->GetSqlStatement();
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t Query::CountParameters() const {
    return this->immutableState->GetParameterInfo().size();
  }

  // ------------------------------------------------------------------------------------------- //

  const std::vector<QueryParameterView> &Query::GetParameterInfo() const {
    return this->immutableState->GetParameterInfo();
  }

  // ------------------------------------------------------------------------------------------- //

  const Value &Query::GetParameterValue(std::size_t index) const {
    const std::vector<QueryParameterView> &parameters = this->immutableState->GetParameterInfo();
    std::u8string parameterName = std::u8string(parameters.at(index).Name);
    return this->implementation->GetParameterValue(parameterName);
  }

  // ------------------------------------------------------------------------------------------- //

  const Value &Query::GetParameterValue(const std::u8string &name) const {
    return this->implementation->GetParameterValue(name);
  }

  // ------------------------------------------------------------------------------------------- //

  void Query::SetParameterValue(std::size_t index, const Value &value) {
    const std::vector<QueryParameterView> &parameters = this->immutableState->GetParameterInfo();
    std::u8string parameterName = std::u8string(parameters.at(index).Name);
    this->implementation->SetParameterValueUnchecked(parameterName, value);
  }

  // ------------------------------------------------------------------------------------------- //

  void Query::SetParameterValue(const std::u8string &name, const Value &value) {
    using Nuclex::Support::Text::StringMatcher;
    constexpr bool CaseSensitive = false;

    const std::vector<QueryParameterView> &parameters = this->immutableState->GetParameterInfo();
    for(std::size_t index = 0; index < parameters.size(); ++index) {
      if(StringMatcher::AreEqual<CaseSensitive>(parameters[index].Name, name)) {
        this->implementation->SetParameterValueUnchecked(name, value);
        return;
      }
    }

    std::u8string message(u8"No such query parameter: '", 26);
    message.append(name);
    message.push_back(u8'\'');
    throw Errors::BadParameterNameError(message);
  }

  // ------------------------------------------------------------------------------------------- //

  Query &Query::operator =(const Query &other) {
    this->immutableState = other.immutableState; // shared ownership
    this->implementation = std::make_unique<Implementation>(*other.implementation.get());
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

  Query &Query::operator =(Query &&other) {
    this->immutableState = std::move(other.immutableState);
    this->implementation = std::move(other.implementation);
    return *this;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
