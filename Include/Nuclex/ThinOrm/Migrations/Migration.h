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

#include <typeinfo> // for std::type_info

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Applies (and optional, reverts) changes to a database</summary>
  class NUCLEX_THINORM_TYPE Migration {

    /// <summary>Frees all resources owned by the migration</summary>
    public: NUCLEX_THINORM_API virtual ~Migration();

    /// <summary>Upgrades the database schema</summary>
    public: NUCLEX_THINORM_API virtual void Up() = 0;

    /// <summary>Downgrades the database schema</summary>
    /// <remarks>
    ///   The default implementation throws an exception stating that downgrading is
    ///   not supported. Override this (and do not call the base implementation) in order
    ///   to support downgrading the database schema to the version before the migration.
    /// </remarks>
    public: NUCLEX_THINORM_API virtual void Down();

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

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_MIGRATION_H
