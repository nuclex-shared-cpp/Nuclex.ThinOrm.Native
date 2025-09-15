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
#include "Nuclex/ThinOrm/Value.h"
#include "Nuclex/ThinOrm/RowReader.h"

#include "../../Utilities/QStringConverter.h" // for QStringConverter

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>
#include <Nuclex/Support/ScopeGuard.h> // for ON_SCOPE_EXIT_TRANSACTION

#include <QSqlError> // for QSqlError
#include <stdexcept> // for std::runtime_error

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

  std::shared_ptr<QtSqlConnection> QtSqlConnection::Connect(
    const Configuration::ConnectionProperties &properties
  ) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    std::u8string driverName = properties.GetDriver();

    // Check if the driver exists explicitly. While this error would be caught when
    // attempting to open the connection, this way we can deliver a decent error message
    if(!QSqlDatabase::isDriverAvailable(QStringConverter::FromU8(driverName))) [[unlikely]] {
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
      Nuclex::Support::Text::lexical_append(uniqueDatabaseName, uniqueId);

      QSqlDatabase database = QSqlDatabase::addDatabase(
        QStringConverter::FromU8(driverName), QStringConverter::FromU8(uniqueDatabaseName)
      );
      {
        auto removeDatabaseScope = ON_SCOPE_EXIT_TRANSACTION {
          QSqlDatabase::removeDatabase(QStringConverter::FromU8(uniqueDatabaseName));
        };

        if(!database.isValid()) [[unlikely]] {
          std::u8string message(
            u8"Error configuring database via Qt SQL. Most likely, the specified driver, '", 75
          );
          message.append(driverName);
          message.append(u8"' could not be loaded due to missing dependencies", 49);
          throw std::runtime_error(
            std::string(reinterpret_cast<const char *>(message.data()), message.length())
          );
        }

        std::u8string hostname = properties.GetHostnameOrPath();
        database.setHostName(QStringConverter::FromU8(hostname));

        std::optional<std::u8string> databaseName2 = properties.GetDatabaseName();
        if(databaseName2.has_value()) {
          database.setDatabaseName(QStringConverter::FromU8(databaseName2.value()));
        }

        std::optional<std::uint16_t> port = properties.GetPort();
        if(port.has_value()) {
          database.setPort(port.value());
        }

        std::optional<std::u8string> userName = properties.GetUser();
        if(userName.has_value()) {
          database.setUserName(QStringConverter::FromU8(userName.value()));
        }

        std::optional<std::u8string> password = properties.GetPassword();
        if(password.has_value()) {
          database.setPassword(QStringConverter::FromU8(password.value()));
        }

        bool opened = database.open();
        if(!opened) {
          std::u8string message(u8"Failed to open database connection to '", 39);
          if(databaseName2.has_value()) {
            message.append(databaseName2.value());
            message.append(u8"' on '", 6);
          }
          message.append(hostname);
          message.push_back(u8'\'');
          if(userName.has_value()) {
            message.append(u8" as '", 5);
            message.append(userName.value());
            message.push_back(u8'\'');
          }
          message.push_back(u8':');
          message.append(QStringConverter::ToU8(database.lastError().text()));
          throw std::runtime_error(
            std::string(reinterpret_cast<const char *>(message.data()), message.length())
          );
        }

        std::shared_ptr<QtSqlConnection> result = std::make_shared<QtSqlConnection>(
          databaseName, uniqueId, database
        );
        removeDatabaseScope.Commit();
        returnUniqueIdScope.Commit();
        return result;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void QtSqlConnection::Prepare(const Query &query) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  void QtSqlConnection::RunStatement(const Query &statement) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlConnection::RunScalarQuery(const Query &scalarQuery) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  RowReader QtSqlConnection::RunRowQuery(const Query &rowQuery) {
    throw u8"Not implemented yet";
  }

  // ------------------------------------------------------------------------------------------- //

  UniqueNameGenerator QtSqlConnection::uniqueNameGenerator;

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql
