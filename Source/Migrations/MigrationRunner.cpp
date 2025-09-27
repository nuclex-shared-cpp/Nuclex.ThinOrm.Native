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

#include "Nuclex/ThinOrm/Migrations/MigrationRunner.h"
#include "Nuclex/ThinOrm/Migrations/Migration.h"

#include "../Utilities/QStringConverter.h" // for QStringConverter, U8Chars()

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::MigrationRunner():
    migrations() {}

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::~MigrationRunner() = default;

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::UpgradeToLatestSchema() {
    // TODO: Implement schema migration to latest version
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::MoveToSchemaVersion(std::size_t schemaVersion) {
    // TODO: Implement schema migration to specific version
    (void)schemaVersion;
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddMigration(std::unique_ptr<Migration> migration) {
    this->migrations.push_back(std::move(migration));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations() {
    // TODO: Implement option to register all migrations in the global repository
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations(const std::type_info *dataContextType) {
    // TODO: Implement option to register matching migrations in the global repository
    (void)dataContextType;
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
