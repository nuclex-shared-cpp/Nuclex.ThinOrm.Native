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

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h"
#include "Nuclex/ThinOrm/Value.h"
#include "Nuclex/ThinOrm/RowReader.h"

#include "../../Utilities/QStringConverter.h" // for QStringConverter

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>
#include <Nuclex/Support/ScopeGuard.h> // for ON_SCOPE_EXIT_TRANSACTION

#include <QSqlError> // for QSqlError
#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Forms the unique connection name that is passed to QSqlDatabase</summary>
  /// <parma name="connectionBaseName">
  ///   Semi-unique name of the connection configuration, either specified by the user or
  ///   taken from the connection configuration on a best-effort basis
  /// </param>
  /// <param name="uniqueId">Unique id assigned to the connection instance</param>
  /// <returns>The unqiue connection name that gets passed to QSqlDatabase</returns>
  QString makeUniqueConnectionName(
    const std::u8string &connectionBaseName, std::uint64_t uniqueId
  ) {
    std::u8string uniqueConnectionName = connectionBaseName;
    uniqueConnectionName.push_back(u8'-');
    Nuclex::Support::Text::lexical_append(uniqueConnectionName, uniqueId);

    return Nuclex::ThinOrm::Utilities::QStringConverter::FromU8(uniqueConnectionName);
  }

  // ------------------------------------------------------------------------------------------- //
  
} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnection::QtSqlConnection(
    std::u8string connectionBaseName, std::uint64_t uniqueId, QSqlDatabase database
  ) :
    connectionBaseName(connectionBaseName),
    uniqueId(uniqueId),
    database(database),
    uniqueConnectionName(makeUniqueConnectionName(connectionBaseName, uniqueId)) {}

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnection::~QtSqlConnection() {
    QtSqlConnection::uniqueNameGenerator.ReturnUniqueId(this->connectionBaseName);
  }

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<QtSqlConnection> QtSqlConnection::Connect(
    const Configuration::ConnectionProperties &properties,
    const std::u8string connectionBaseName,
    const std::optional<std::u8string> databaseName
  ) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    // Figure out the name of the database to use for the QSqlDatabase (Qt SQL globally
    // manages connections, which is counter to our design
    std::uint64_t uniqueId = uniqueNameGenerator.BorrowUniqueId(connectionBaseName);
    {
      auto returnUniqueIdScope = ON_SCOPE_EXIT_TRANSACTION {
        uniqueNameGenerator.ReturnUniqueId(connectionBaseName);
      };

      std::u8string driverName = properties.GetDriver();
      QString uniqueConnectionName = makeUniqueConnectionName(
        connectionBaseName, uniqueId
      );

      QSqlDatabase database = QSqlDatabase::addDatabase(
        QStringConverter::FromU8(driverName), uniqueConnectionName
      );
      {
        auto removeDatabaseScope = ON_SCOPE_EXIT_TRANSACTION {
          QSqlDatabase::removeDatabase(uniqueConnectionName);
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

        // databaseName is processed in the QtSqlConnectionFactory
        if(databaseName.has_value()) {
          database.setDatabaseName(QStringConverter::FromU8(databaseName.value()));
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
          std::optional<std::u8string> rawDatabaseName = properties.GetDatabaseName();
          if(rawDatabaseName.has_value()) {
            message.append(rawDatabaseName.value());
            message.append(u8"' on or in '", 6);
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
          connectionBaseName, uniqueId, database//, uniqueConnectionName
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

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
