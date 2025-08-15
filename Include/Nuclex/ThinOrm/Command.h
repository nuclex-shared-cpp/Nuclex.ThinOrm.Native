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

#ifndef NUCLEX_THINORM_COMMAND_H
#define NUCLEX_THINORM_COMMAND_H

#include "Nuclex/ThinOrm/Config.h"

#include <stop_token> // for std::stop_token

namespace Nuclex { namespace ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Command in the form of an SQL statement that can be issued to a database</summary>
  class NUCLEX_THINORM_TYPE Command {

    /// <summary>Frees all resources owned by the command</summary>
    public: NUCLEX_THINORM_API virtual ~Command() = default;

  };

  // ------------------------------------------------------------------------------------------- //

}} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_COMMAND_H
