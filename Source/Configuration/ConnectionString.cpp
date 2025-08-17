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

#include "Nuclex/ThinOrm/Configuration/ConnectionString.h"

#include <stdexcept>

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  ConnectionString::ConnectionString() :
    driver(u8"sqlite", 6),
    hostnameOrPath(u8".", 1),
    port(),
    user(),
    password(),
    databaseName(std::u8string(u8"default.sqlite3.db", 18)),
    options() {}

  // ------------------------------------------------------------------------------------------- //

  ConnectionString ConnectionString::Parse(const std::u8string &properties) {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
    return ConnectionString();
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string ConnectionString::ToString() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> ConnectionString::GetOption(
    const std::u8string &name
  ) const {
    std::map<std::u8string, std::u8string>::const_iterator iterator = (
      this->options.find(name)
    );
    if(iterator == this->options.end()) {
      return std::optional<std::u8string>();
    } else {
      return iterator->second;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void ConnectionString::SetOption(
    const std::u8string &name,
    const std::optional<std::u8string> &value /* = std::optional<std::u8string>() */
  ) {
    if(value.has_value()) {
      this->options[name] = value.value();
    } else {
      this->options.erase(name);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
