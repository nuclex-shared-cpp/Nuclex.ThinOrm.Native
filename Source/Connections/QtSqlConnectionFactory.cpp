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

#include "Nuclex/ThinOrm/Connections/QtSqlConnectionFactory.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Configuration/ConnectionProperties.h"
#include "Nuclex/ThinOrm/Errors/MissingDriverError.h"

#include "../Utilities/QStringConverter.h" // for QStringConverter
#include "./QtSql/QtSqlConnection.h" // for QtSqlConnection

#include <filesystem> // for std::filesystem::path

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Places the full path of the database in the database name field for file-based
  ///   databases accessed through Qt's SQL module (which ignores the host name then)
  /// </summary>
  class FullpathInDatabaseNameFormatter :
    public Nuclex::ThinOrm::Connections::QtSqlConnectionFactory::DatabaseNameFormatter {

    /// <summary>Forms the database name that will be passed to Qt</summary>
    /// <param name="connectionProperties">
    ///   Connection settings the database name can be constructed from
    /// </param>
    /// <returns>The database name that will be passed to Qt SQL module</returns>
    public: std::optional<std::u8string> operator()(
      const Nuclex::ThinOrm::Configuration::ConnectionProperties &connectionProperties
    ) override;

  };

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> FullpathInDatabaseNameFormatter::operator()(
    const Nuclex::ThinOrm::Configuration::ConnectionProperties &connectionProperties
  ) {
    std::u8string path = connectionProperties.GetHostnameOrPath();
    std::optional<std::u8string> databaseName = connectionProperties.GetDatabaseName();
    if(databaseName.has_value()) {
      return (
        std::filesystem::path(path) / std::filesystem::path(databaseName.value())
      ).u8string();
    } else {
      return path;
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnectionFactory::DatabaseNameFormatter::~DatabaseNameFormatter() = default;

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> QtSqlConnectionFactory::DatabaseNameFormatter::operator()(
    const Configuration::ConnectionProperties &connectionProperties
  ) {
    return connectionProperties.GetDatabaseName(); // default implementation
  }

  // ------------------------------------------------------------------------------------------- //

  const std::u8string QtSqlConnectionFactory::ConnectionNameOptionName(u8"QtConnectionName", 16);

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnectionFactory::QtSqlConnectionFactory() :
    formatters() {
    OverrideDatabaseNameFormatter<FullpathInDatabaseNameFormatter>(u8"QSQLITE");
  }

  // ------------------------------------------------------------------------------------------- //

  QtSqlConnectionFactory::~QtSqlConnectionFactory() = default;

  // ------------------------------------------------------------------------------------------- //

  std::shared_ptr<Connection> QtSqlConnectionFactory::Connect(
    const Configuration::ConnectionProperties &connectionProperties
  ) const {
    std::u8string driverName = connectionProperties.GetDriver();
    requireQtSqlDriver(driverName);

    // The connection name is passed to QSqlDatabase::addDatabase(), though we will
    // append a unique number to it. The user can customize the string part of this name
    // such that possible error message will give better hints at which database is
    // involved in an error message, but this is purely optional.
    std::optional<std::u8string> connectionName = connectionProperties.GetOption(
      ConnectionNameOptionName
    );
    if(!connectionName.has_value()) [[likely]] {
      connectionName = connectionProperties.GetDatabaseName();
      if(!connectionName.has_value()) {
        connectionName = connectionProperties.GetHostnameOrPath();
      }
    }

    // The database name is the target database that will be opened when connecting to
    // a database server. For file-based databases, Qt expects both the directory and
    // the filename of the database in here (whilst we have those separated). That can
    // be overriden via a custom database name formatter, for which we query below:
    std::optional<std::u8string> qtDatabaseName;
    {
      DriverFormatterMap::const_iterator iterator = this->formatters.find(driverName);
      if(iterator == this->formatters.end()) [[likely]] {
        qtDatabaseName = connectionProperties.GetDatabaseName();
      } else {
        qtDatabaseName = iterator->second->operator()(connectionProperties);
      }
    }

    // We finally have the string on which the unique connection name should be based
    // and optionally a database name to pass on to Qt, so now leave the actual work
    // of opening the connection to the connection class itself.
    return QtSql::QtSqlConnection::Connect(
      connectionProperties, connectionName.value(), qtDatabaseName
    );
  }

  // ------------------------------------------------------------------------------------------- //

  void QtSqlConnectionFactory::OverrideDatabaseNameFormatter(
    const std::u8string &driverName, std::unique_ptr<DatabaseNameFormatter> formatter
  ) {
    DriverFormatterMap::iterator iterator = this->formatters.find(driverName);
    if(iterator == this->formatters.end()) {
      this->formatters.emplace(driverName, std::move(formatter));
    } else {
      iterator->second = std::move(formatter);
    }
  }

  void QtSqlConnectionFactory::requireQtSqlDriver(const std::u8string &driver) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    // Check if the driver exists explicitly. While this error would be caught when
    // attempting to open the connection, this way we can deliver a decent error message
    if(!QSqlDatabase::isDriverAvailable(QStringConverter::FromU8(driver))) [[unlikely]] {
      std::u8string message(
        u8"Unable to open database connection. Either the driver name is invalid outright or "
        u8"the Qt library being used was built without enabling the '", 137
      );
      message.append(driver);
      message.append(u8"' driver.", 9);
      throw Nuclex::ThinOrm::Errors::MissingDriverError(message);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
