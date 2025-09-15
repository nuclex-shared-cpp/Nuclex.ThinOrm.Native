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

#include "./UniqueNameGenerator.h"

#include <cassert> // for assert()

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  std::uint64_t UniqueNameGenerator::BorrowUniqueId(const std::u8string &databaseName) {
    std::unique_lock<std::mutex> stateAccessScope(this->stateMutex);

    StringIdMap::iterator iterator = this->nextDatabaseIds.find(databaseName);
    if(iterator == this->nextDatabaseIds.end()) {
      DatabaseIdGenerator newGenerator;
      newGenerator.ConnectionCount = 1;
      newGenerator.NextUniqueId = 2;
      this->nextDatabaseIds.emplace(databaseName, newGenerator);
      return 1;
    } else {
      DatabaseIdGenerator &generator = iterator->second;
      ++generator.ConnectionCount;
      return generator.NextUniqueId++;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void UniqueNameGenerator::ReturnUniqueId(const std::u8string &databaseName) {
    std::unique_lock<std::mutex> stateAccessScope(this->stateMutex);

    StringIdMap::iterator iterator = this->nextDatabaseIds.find(databaseName);
    if(iterator == this->nextDatabaseIds.end()) {
      assert(false && u8"Any ReturnUniqueName() should be paired with a BorrowUniqueName()");
    } else {
      DatabaseIdGenerator &generator = iterator->second;
      --generator.ConnectionCount;
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql
