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

#ifndef NUCLEX_THINORM_MIGRATIONS_GLOBALMIGRATIONREPOSITORY_H
#define NUCLEX_THINORM_MIGRATIONS_GLOBALMIGRATIONREPOSITORY_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Migrations/Migration.h"

#include <typeinfo> // for std::type_info
#include <vector> // for std::vector<>
#include <memory> // for std::unique_ptr<>
#include <mutex> // for std::mutex
#include <unordered_map> // for std::unordered_map<>

namespace Nuclex::ThinOrm::Migrations {
  class Migration;
}
namespace Nuclex::ThinOrm {
  class DataContext;
}

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Global store to which individual migrations can register</summary>
  /// <remarks>
  ///   This is used in conjunction with the <see cref="MigrationRunner" />. You can either
  ///   register migrations individually to your <see cref="MigrationRunner" /> or you can
  ///   use 'initterm' classes to register your migrations to this repository and have
  ///   your <see cref="MigrationRunner" /> fetch them from this repository.
  /// </remarks>
  class NUCLEX_THINORM_TYPE GlobalMigrationRepository {

    /// <summary>Registers a migration for the specified data context</summary>
    /// <param name="dataContextType">
    ///   Data context for the database the migration applies to, can be nullptr (which will
    ///   not apply the migration to all but treats the nullptr data context as another
    ///   singular data context for cases where applications have a single, default database)
    /// </param>
    /// <param name="migration">Migration step that will be registered</param>
    public: NUCLEX_THINORM_API static void AddMigration(
      const std::type_info *dataContextType, const std::shared_ptr<Migration> &migration
    );

    /// <summary>Registers a migration for the specified data context</summary>
    /// <typeparam name="TDataContext">
    ///   Data context for the database the migration applies to
    /// </typeparam>
    /// <param name="migration">Migration step that will be registered</param>
    public: template<typename TDataContext>
    NUCLEX_THINORM_API inline static void AddMigration(
      const std::shared_ptr<Migration> &migration
    );

    /// <summary>Registers a migration for the specified data context</summary>
    /// <typeparam name="TDataContext">
    ///   Data context for the database the migration applies to
    /// </typeparam>
    /// <typeparam name="TMigration">
    ///   Class that implements the migration step that will be registered
    /// </param>
    public: template<typename TDataContext, typename TMigration>
    NUCLEX_THINORM_API inline static void AddMigration();

    /// <summary>Removes all migrations from the repository</summary>
    /// <remarks>
    ///   You can call this method after you're sure you don't need to migrate the database
    ///   anymore to free all memory used by the migrations. It will just be a few bytes
    ///   unless your migration classes are super complex, but it's the thought that counts :)
    /// </remarks>
    public: NUCLEX_THINORM_API static void ClearAllMigrations();

    /// <summary>
    ///   Fetches the migrations for the specifeid database (identified by its data context)
    /// </summary>
    /// <param name="dataContextType">
    ///   Data context for which the migrations will be returned
    /// </param>
    /// <returns>A list containing the migrations for the specified database</returns>
    public: NUCLEX_THINORM_API static const std::vector<
      std::shared_ptr<Migration>
    > &GetUnsortedMigrations(const std::type_info *dataContextType);

    /// <summary>List of database migration steps</summary>
    private: typedef std::vector<std::shared_ptr<Migration>> MigrationVector;

    /// <summary>Map that associates types with lists of migration steps</summary>
    private: typedef std::unordered_map<
      const std::type_info *, MigrationVector
    > TypeMigrationsMap;

    /// <summary>Must be held when accessing the migration list in any way</summary>
    private: static std::mutex migrationAccessMutex;
    /// <summary>List of migrations matched to target databases by data context</summary>
    private: static TypeMigrationsMap migrations;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void GlobalMigrationRepository::AddMigration(
    const std::shared_ptr<Migration> &migration
  ) {
    static_assert(
      std::is_base_of<DataContext, TDataContext>::value,
      //std::is_convertible<DataContext *, TDataContext *>::value,
      u8"The data context template argument on GlobalMigrationRepository::AddMigration() "
      u8"must refer to a class that inherits from the 'DataContext' class"
    );

    static const std::type_info &typeInfo = typeid(TDataContext);
    AddMigration(&typeInfo, std::move(migration));
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext, typename TMigration>
  inline void GlobalMigrationRepository::AddMigration() {
    static_assert(
      std::is_default_constructible<TMigration>::value,
      u8"The templated AddMigration() method can only be used on migrations that have "
      u8"a parameterless default constructor. If you are forwardig the schema version, "
      u8"don't do that. Instead, pass a hard-coded and schema version to the base class "
      u8"contructor and give your migration itself a parameterless default constructor."
    );
    static_assert(
      std::is_base_of<Migration, TMigration>::value,
      //std::is_convertible<Migration *, TMigration *>::value,
      u8"Migrations have to publicly inherit the 'Migration' base class in order to "
      u8"quality as migrations."
    );

    AddMigration<TDataContext>(std::make_shared<TMigration>());
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_GLOBALMIGRATIONREPOSITORY_H
