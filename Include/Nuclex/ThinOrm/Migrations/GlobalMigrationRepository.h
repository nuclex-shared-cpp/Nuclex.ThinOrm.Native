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

namespace Nuclex::ThinOrm::Migrations {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Provides global store to which individual migrations can register</summary>
  class NUCLEX_THINORM_TYPE GlobalMigrationRepository {

    //public: NUCLEX_THINORM_API static void AddMigration

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Migrations

#endif // NUCLEX_THINORM_MIGRATIONS_GLOBALMIGRATIONREPOSITORY_H
