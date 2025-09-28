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

#include "Nuclex/ThinOrm/Migrations/GlobalMigrationRepository.h"
#include "Nuclex/ThinOrm/Migrations/Migration.h"

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  void GlobalMigrationRepository::AddMigration(
    const std::type_info *dataContextType, const std::shared_ptr<Migration> &migration
  ) {
    std::unique_lock<std::mutex> migrationMutexScope(migrationAccessMutex);

    TypeMigrationsMap::iterator iterator = migrations.find(dataContextType);

    if(iterator == migrations.end()) {
      MigrationVector newMigrations;
      newMigrations.push_back(migration);
      migrations.emplace(dataContextType, newMigrations);
    } else {
      MigrationVector &migrations = iterator->second;
      migrations.push_back(migration);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void GlobalMigrationRepository::ClearAllMigrations() {
    std::unique_lock<std::mutex> migrationMutexScope(migrationAccessMutex);
    migrations.clear();
  }

  // ------------------------------------------------------------------------------------------- //

  const std::vector<std::shared_ptr<Migration>> &GlobalMigrationRepository::GetUnsortedMigrations(
    const std::type_info *dataContextType
  ) {
    std::unique_lock<std::mutex> migrationMutexScope(migrationAccessMutex);

    // I am a bit torn between throwing an exception when the specified data context does
    // not exist or changing the method signature to return a pointer so it can deliver
    // a null pointer. But I think having a database with zero migrations is just a sign
    // that something isn't set up correctly, so the exception is most likely a better
    // choice than silently doing nothing...
    //
    TypeMigrationsMap::iterator iterator = migrations.find(dataContextType);
    if(iterator == migrations.end()) {
      throw std::runtime_error(
        reinterpret_cast<const char *>(
          u8"No migrations registered for the specified database"
        )
      );
    }

    return iterator->second;
  }

  // ------------------------------------------------------------------------------------------- //

  std::mutex GlobalMigrationRepository::migrationAccessMutex;

  GlobalMigrationRepository::TypeMigrationsMap GlobalMigrationRepository::migrations;

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
