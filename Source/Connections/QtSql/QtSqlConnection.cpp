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

#include "./QtSqlConnection.h"

#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h"
#include "Nuclex/ThinOrm/Errors/MissingDriverError.h"

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>
#include <Nuclex/Support/ScopeGuard.h> // for ON_SCOPE_EXIT_TRANSACTION

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Name of the option by which the database name can be customized</summary>
  std::u8string DatabaseNameOptionName(u8"DatabaseName", 12);

  // ------------------------------------------------------------------------------------------- //
  
} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnection::QtSqlConnection(
    std::u8string databaseName, std::uint64_t uniqueId, QSqlDatabase database
  ) :
    databaseName(databaseName),
    uniqueId(uniqueId),
    database(database) {}

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnection::~QtSqlConnection() {
    QtSqlConnection::uniqueNameGenerator.ReturnUniqueId(this->databaseName);
  }

  // ------------------------------------------------------------------------------------------- //
#if 0
  std::shared_ptr<QtSqlConnection> QtSqlConnection::Connect(
    const Configuration::ConnectionProperties &properties
  ) {
    std::u8string driverName = properties.GetDriver();

    // Check if the driver exists explicitly. While this error would be caught when
    // attempting to open the connection, this way we can deliver a decent error message
    if(!QSqlDatabase::isDriverAvailable(QString::fromStdString(driverName))) [[unlikely]] {
      std::u8string message(
        u8"Unable to open database connection. Either the driver name is invalid outirght or "
        u8"the Qt library being used was built with enabling the '", 137
      );
      message.append(driverName);
      message.append(u8"' driver.", 9);
      throw Nuclex::ThinOrm::Errors::MissingDriverError(message);
    }

    // Figure out the name of the database to use for the QSqlDatabase (Qt SQL globally
    // manages connections, which is counter to our design
    std::u8string databaseName;
    {
      std::optional<std::u8string> customDatabaseName = (
        properties.GetOption(DatabaseNameOptionName)
      );
      if(customDatabaseName.has_value()) {
        databaseName = std::move(customDatabaseName.value());
      } else {
        customDatabaseName = properties.GetDatabaseName();
        if(customDatabaseName.has_value()) {
          databaseName = std::move(customDatabaseName.value());
        } else {
          databaseName = std::move(properties.GetHostnameOrPath());
        }
      }
    }

    std::uint64_t uniqueId = uniqueNameGenerator.BorrowUniqueId(databaseName);
    {
      auto returnUniqueIdScope = ON_SCOPE_EXIT_TRANSACTION {
        uniqueNameGenerator.ReturnUniqueId(databaseName);
      };

      std::u8string uniqueDatabaseName = databaseName;
      uniqueDatabaseName.push_back(u8'-');
      Nuclex::Support::Text::lexical_append(uniqueDatabaseName, uniqueId;

      QSqlDatabase database = QSqlDatabase::addDatabase(
        QString::fromStdString(driverName), QString::fromStdString(uniqueDatabaseName)
      );

      returnUniqueIdScope.Commit();
    }
    
  }
#endif
  // ------------------------------------------------------------------------------------------- //

  UniqueNameGenerator QtSqlConnection::uniqueNameGenerator;

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql
