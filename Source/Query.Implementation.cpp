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

#include "./Query.Implementation.h"

#include "Nuclex/ThinOrm/Errors/UnassignedParameterError.h"

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  Query::Implementation::Implementation() :
    stateMutex(),
    parameterValues() {}

  // ------------------------------------------------------------------------------------------- //

  Query::Implementation::Implementation(const Implementation &other) :
    stateMutex(),
    parameterValues(other.parameterValues) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Implementation::Implementation(Implementation &&other) :
    stateMutex(),
    parameterValues(std::move(other.parameterValues)) {}

  // ------------------------------------------------------------------------------------------- //

  Query::Implementation::~Implementation() = default;

  // ------------------------------------------------------------------------------------------- //
  
  const Value &Query::Implementation::GetParameterValue(const std::u8string &name) const {
    ParameterValueMap::const_iterator iterator = this->parameterValues.find(name);
    if(iterator == this->parameterValues.end()) {
      std::u8string message(u8"Parameter '", 11);
      message.append(name);
      message.append(u8"' has not been given a value yet", 32);
      throw Errors::UnassignedParameterError(message);
    }
    return iterator->second;
  }

  // ------------------------------------------------------------------------------------------- //

  void Query::Implementation::SetParameterValueUnchecked(
    const std::u8string &name, const Value &value
  ) {
    this->parameterValues.insert(
      ParameterValueMap::value_type(name, value)
    );
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm
