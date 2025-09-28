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
#include "Nuclex/ThinOrm/Migrations/GlobalMigrationRepository.h"
#include "Nuclex/ThinOrm/Errors/AmbiguousSchemaVersionError.h"

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()

#include "../Utilities/QStringConverter.h" // for QStringConverter, U8Chars()

#include <algorithm> // for std::sort()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Comparison functor that checks if one migration's schema version is earlier (less)
  ///   than another's
  /// </summary>
  class MigrationSchemaVersionLess {

    //std::less<std::unique_ptr<Nuclex::ThinOrm::Migrations::Migration>>

    /// <summary>
    ///   Checks if the first migration's schema version is 'less' than the second's
    /// </summary>
    /// <param name="left">First migration step whose schema version to compare</param>
    /// <param name="right">Second migration step whose schema version to compare</param>
    /// <returns>True if the first migration's schema version is 'less'</returns>
    public: bool operator()(
      const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &left,
      const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &right
    ) const noexcept;

  };

  // ------------------------------------------------------------------------------------------- //

  bool MigrationSchemaVersionLess::operator()(
    const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &left,
    const std::shared_ptr<Nuclex::ThinOrm::Migrations::Migration> &right
  ) const noexcept {
    return left->GetTargetSchemaVersion() < right->GetTargetSchemaVersion();
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::MigrationRunner():
    migrations() {}

  // ------------------------------------------------------------------------------------------- //

  MigrationRunner::~MigrationRunner() = default;

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::UpgradeToLatestSchema() {
    sortMigrationsBySchemaVersion();
    requireDistinctSchemaVersions();

    // TODO: Implement schema migration to latest version
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::MoveToSchemaVersion(std::size_t schemaVersion) {
    sortMigrationsBySchemaVersion();
    requireDistinctSchemaVersions();

    // TODO: Implement schema migration to specific version
    (void)schemaVersion;
    throw std::runtime_error(U8CHARS(u8"Not implemented yet"));
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddMigration(const std::shared_ptr<Migration> &migration) {
    this->migrations.push_back(migration);
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations() {
    AddAllGlobalMigrations(nullptr);
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::AddAllGlobalMigrations(const std::type_info *dataContextType) {
    const MigrationVector &globalMigrations = (
      GlobalMigrationRepository::GetUnsortedMigrations(dataContextType)
    );

    std::size_t count = globalMigrations.size();
    for(std::size_t index = 0; index < count; ++index) {
      this->migrations.push_back(globalMigrations[index]);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::sortMigrationsBySchemaVersion() {
    std::sort(
      this->migrations.begin(),
      this->migrations.end(),
      MigrationSchemaVersionLess()
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void MigrationRunner::requireDistinctSchemaVersions() {
    std::size_t count = this->migrations.size();
    if(count == 0) {
      return;
    }

    std::size_t schemaVersion = this->migrations[0]->GetTargetSchemaVersion();
    for(std::size_t index = 1; index < count; ++index) {

      // Assuming migrations are sorted by increasing schema versions, if the same
      // schema version is repeated, it means this verification step has failed.
      std::size_t nextSchemaVersion = this->migrations[index]->GetTargetSchemaVersion();
      if(schemaVersion == nextSchemaVersion) {
        std::u8string message(u8"Schema version '", 16);
        Nuclex::Support::Text::lexical_append(message, schemaVersion);
        message.append(u8"' was declared by more than one migration and is ambiguous", 58);
        throw Errors::AmbiguousSchemaVersionError(message);
      }

      // If the schema versions are not sorted, our technique of linearly checking for
      // repeated schema version is useless, so we need to check for that, too.
      if(nextSchemaVersion < schemaVersion) {
        assert(
          (nextSchemaVersion < schemaVersion) &&
          u8"Migration steps must be sorted by schema version"
        );
        throw std::logic_error(
          reinterpret_cast<const char *>(
            u8"Internal error: migration steps were not sorted by their declared "
            u8"schema vesions by the time 'requireDistinctSchemaVersions() was called."
          )
        );
      }

    } // for each migration index
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations
