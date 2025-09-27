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

#ifndef NUCLEX_THINORM_MIGRATIONS_MIGRATION_H
#define NUCLEX_THINORM_MIGRATIONS_MIGRATION_H

#include "Nuclex/ThinOrm/Config.h"

#include <cstddef> // for std::size_t
#include <typeinfo> // for std::type_info
#include <string> // for std::u8string

namespace Nuclex::ThinOrm::Connections {
  class Connection;
}

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Applies (and optional, reverts) changes to a database</summary>
  class NUCLEX_THINORM_TYPE Migration {

    /// <summary>Initializes a new migration to the specified version</summary>
    /// <param name="schemaVersion">Schema version this migration will upgrade to</param>
    /// <param name="name">
    ///   Name of the migration for documentation purposes, i.e. 'add user management'
    /// </param>
    /// <remarks>
    ///   <para>
    ///     Each migration needs a unique schema version number. Schema migrations are applied
    ///     in order of incrementing schema version numbers, so once you've let a migration
    ///     out to production, its version number must never change.
    ///   </para>
    /// </remarks>
    public: NUCLEX_THINORM_API Migration(
      std::size_t schemaVersion, const std::u8string &name = std::u8string()
    );
#if 0
    /// <summary>Initializes a new migration to the specified version</summary>
    /// <param name="schemaDate">Schema date this migration will upgrade to</param>
    public: NUCLEX_THINORM_API Migration(std::chrono::year_month_day schemaDate);
#endif
    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API virtual ~Migration();

    /// <summary>Retrieves the schema version number this migration upgrades to</summary>
    /// <returns>The database schema version this migration upgrades to</returns>
    public: NUCLEX_THINORM_API inline std::size_t GetTargetSchemaVersion() const;

    /// <summary>Retrieves the name of the migration for documentation purposes</summary>
    /// <returns>The name of the migration</returns>
    public: NUCLEX_THINORM_API inline const std::u8string &GetName() const;

    /// <summary>Upgrades the database schema</summary>
    /// <param name="connection">
    ///   Database connection through which the schema can be upgraded
    /// </param>
    public: NUCLEX_THINORM_API virtual void Up(Connections::Connection &connection) = 0;

    /// <summary>Downgrades the database schema</summary>
    /// <remarks>
    ///   The default implementation throws an exception stating that downgrading is
    ///   not supported. Override this (and do not call the base implementation) in order
    ///   to support downgrading the database schema to the version before the migration.
    /// </remarks>
    /// <param name="connection">
    ///   Database connection through which the schema can be downgraded
    /// </param>
    public: NUCLEX_THINORM_API virtual void Down(Connections::Connection &connection);

    /// <summary>
    ///   RTTI type id of the data context for the database the migration applies to
    /// </summary>
    /// <returns>The RTTI type id of the data context for the target database</returns>
    /// <remarks>
    ///   If you have just one database, you do not need to bother with this. However, if
    ///   your application accesses multiple databases and migrations are avavailable for
    ///   all of them, the migrations somehow need to be matched to the correct database.
    ///   If you inherit from <see cref="ContextualMigration" /> and specify your own
    ///   database's data context class as its template argument, this method will be taken
    ///   care of for you.
    /// </remarks>
    public: NUCLEX_THINORM_API virtual const std::type_info *GetDataContextType() const;

    /// <summary>Database schema version this migration is upgrading to</summary>
    private: std::size_t schemaVersion;
    /// <summary>Name of the migration for documentation purposes</summary>
    private: std::u8string name;

  };

  // ------------------------------------------------------------------------------------------- //

  inline std::size_t Migration::GetTargetSchemaVersion() const {
    return this->schemaVersion;
  }

  // ------------------------------------------------------------------------------------------- //

  const std::u8string &Migration::GetName() const {
    return this->name;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_MIGRATION_H
