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

#ifndef NUCLEX_THINORM_MIGRATIONS_ENTITIES_MIGRATIONRECORD_H
#define NUCLEX_THINORM_MIGRATIONS_ENTITIES_MIGRATIONRECORD_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/DateTime.h" // for DateTime

#include <cstddef> // for std::size_t
#include <string> // for std::u8string
#include <optional> // for std::optional<>

namespace Nuclex::ThinOrm::Migrations::Entities {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Records an applied database schema migration in the database</summary>
  class MigrationRecord {

    /// <summary>Initializes a new schema migration record</summary>
    /// <param name="schemaVersion">Database schema version whose migration was applied</param>
    /// <param name="appliedOn">Date and time the migration was applied on</param>
    /// <param name="name">Name given by the user to the 
    public: MigrationRecord(
      std::size_t schemaVersion,
      const DateTime &appliedOn,
      const std::optional<std::u8string> &name
    );

    /// <summary>Schema version of the migration</summary>
    public: std::size_t SchemaVersion;

    /// <summary>Date and time the migration was applied on</summary>
    /// <remarks>
    ///   This field isn't actually used by the library, but if storing a time stamp is
    ///   cheap and if anything goes wrong the user may be able to figure one when
    ///   the mess-up happened and either identify the root cause or revert to the nearest
    ///   backup without having to guess.
    /// </remarks>
    public: DateTime AppliedOn;

    /// <summary>Name that describes the changes the migration made to the schema</summary>
    /// <remarks>
    ///   That is, if the user has actually done their diligence and named the migration.
    /// </remarks>
    public: std::optional<std::u8string> Name;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations::Entities

#endif // NUCLEX_THINORM_MIGRATIONS_ENTITIES_MIGRATIONRECORD_H
