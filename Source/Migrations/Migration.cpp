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

#include "Nuclex/ThinOrm/Migrations/Migration.h"
#include "Nuclex/ThinOrm/Errors/DowngradeUnsupportedError.h"

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  Migration::Migration(
    std::size_t schemaVersion, const std::u8string &name /* = std::u8string() */
  ) :
    schemaVersion(schemaVersion),
    name(name) {}

  // ------------------------------------------------------------------------------------------- //

  Migration::~Migration() = default;

  // ------------------------------------------------------------------------------------------- //

  void Migration::Down(Connections::Connection &connection) {
    throw Errors::DowngradeUnsupportedError(
      u8"Migration does not implement the 'Down()' method, thus reverting it is impossible"
    );
  }

  // ------------------------------------------------------------------------------------------- //

  const std::type_info *Migration::GetDataContextType() const {
    return nullptr;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
