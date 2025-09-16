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

#ifndef NUCLEX_THINORM_CONNECTIONS_QTSQLCONNECTIONFACTORY_H
#define NUCLEX_THINORM_CONNECTIONS_QTSQLCONNECTIONFACTORY_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Connections/ConnectionFactory.h"

#include <string> // for std::u8string
#include <memory> // for std::unique_ptr<>
#include <optional> // for std::optional<>
#include <unordered_map> // for std::unordered_map<>
#include <type_traits> // for std::is_default_constructible<>, std::is_base_of<>

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   Establishes database connections through the Qt SQL module from Qt5 or Qt6
  /// </summary>
  class NUCLEX_THINORM_TYPE QtSqlConnectionFactory : public ConnectionFactory {

    #pragma region class DatabaseNameFormatter

    /// <summary>Generates the database name passed to Qt's SQL module</summary>
    /// <remarks>
    ///   Normally, you don't need to touch the name passed to Qt's
    ///   <see cref="QSqlDatabase.setDatabaseName()" /> method, but within this library,
    ///   connection properties are designed to split &quot;path/hostname&quot; and
    ///   &quot;database name&quot;,including for file-based databases. Qt doesn't do
    ///   that, so connecting to <code>QSQLITE:///tmp/test.dn</code> would assign
    ///   <code>/tmp</code> via <see cref="QSqlDatabase.setHostName" /> where it would
    ///   be ignored by Qt's QSQLITe driver. This transformer can rectify such situations
    ///   by path-concatenating the host/path into the database name.
    /// </remarks>
    public: class NUCLEX_THINORM_TYPE DatabaseNameFormatter {

      /// <summary>Frees all resources owned by the instance</summary>
      public: NUCLEX_THINORM_API virtual ~DatabaseNameFormatter();

      /// <summary>Forms the database name that will be passed to Qt</summary>
      /// <param name="connectionProperties">
      ///   Connection settings the database name can be constructed from
      /// </param>
      /// <returns>The database name that will be passed to Qt SQL module</returns>
      public: NUCLEX_THINORM_API virtual std::optional<std::u8string> operator()(
        const Configuration::ConnectionProperties &connectionProperties
      ); // default implementation returns connectionProperties.GetDatabaseName()

    };

    #pragma endregion // class DatabaseNameFormatter

    /// <summary>Name of the option through which the Qt connection name can be set</summary>
    public: NUCLEX_THINORM_API static const std::u8string ConnectionNameOptionName;

    /// <summary>Initializes a new connection factory</summary>
    public: NUCLEX_THINORM_API QtSqlConnectionFactory();
    /// <summary>Frees all resources owned by the connection factory</summary>
    public: NUCLEX_THINORM_API ~QtSqlConnectionFactory() override;

    /// <summary>Establishes a new connection to the specified database</summary>
    /// <param name="connectionProperties">
    ///   Specifies the driver, data source and other parameters to reach the database
    /// </param>
    /// <returns>A new database connection wrapper, if successful</returns>
    /// <remarks>
    ///   This factory passes the 'driver' setting directly to the 'QSqlDatabase' as
    ///   the type of the database, i.e. 'QSQLITE', so pick a valid Qt driver name. The other
    ///   properties are directly applied to the 'QSqlDatabase', whilst the 'connection name'
    ///   (which must be unique if Qt is to actually establish multiple connections) will be
    ///   auto-generated but can be prefixed with an identifier you can control by using
    ///   an options named <see cref="ConnectionNameOptionName" /> in the connection settings.
    /// </remarks>
    public: NUCLEX_THINORM_API [[nodiscard]] std::shared_ptr<Connection> Connect(
      const Configuration::ConnectionProperties &connectionProperties
    ) const override;

    /// <summary>
    ///   Assigns a formatter to produce the database name passed to Qt for the specified driver
    /// </summary>
    /// <param name="driverName">Qt SQL driver name, i.e. <code>QSQLITE</code></param>
    /// <param name="formatter">
    ///   Formatter that should be used to determine the database name passed to Qt for
    ///   connections using the associated Qt SQL driver
    /// </param>
    /// <remarks>
    ///   For a list of Qt's standard drivers (which you can enable or disable when compiling
    ///   the Qt libraries), see: https://doc.qt.io/qt-6/sql-driver.html
    /// </remarks>
    public: NUCLEX_THINORM_API void OverrideDatabaseNameFormatter(
      const std::u8string &driverName, std::unique_ptr<DatabaseNameFormatter> formatter
    );

    /// <summary>
    ///   Assigns a formatter to produce the database name passed to Qt for the specified driver
    /// </summary>
    /// <typeparam name="TFormatter">
    ///   Formatter that should be used to determine the database name passed to Qt for
    ///   connections using the associated Qt SQL driver
    /// </typeparam>
    /// <param name="driverName">Qt SQL driver name, i.e. <code>QSQLITE</code></param>
    /// <remarks>
    ///   For a list of Qt's standard drivers (which you can enable or disable when compiling
    ///   the Qt libraries), see: https://doc.qt.io/qt-6/sql-driver.html
    /// </remarks>
    public: template<typename TFormatter>
    NUCLEX_THINORM_API inline void OverrideDatabaseNameFormatter(const std::u8string &driverName);

    /// <summary>Throws an exception if the specified driver isn't available</summary>
    /// <param name="driver">Driver that will be checked for availability</param>
    private: static void requireQtSqlDriver(const std::u8string &driver);

    /// <summary>Maps strings to database name formatters</summary>
    private: typedef std::unordered_map<
      std::u8string, std::unique_ptr<DatabaseNameFormatter>
    > DriverFormatterMap;

    /// <summary>Registered database name formatters by their Qt SQL driver names</summary>
    private: DriverFormatterMap formatters;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TFormatter>
  inline void QtSqlConnectionFactory::OverrideDatabaseNameFormatter(
    const std::u8string &driverName
  ) {
    static_assert(
      std::is_default_constructible<TFormatter>::value,
      u8"The templated OverrideDatabaseNameFormatter() method can only be used on "
      u8"formatters that have a parameterless default constructor."
    );
    static_assert(
      std::is_base_of<DatabaseNameFormatter, TFormatter>::value,
      //std::is_convertible<DatabaseNameFormatter *, TFormatter *>::value,
      u8"Formatters have to publicly inherit the 'DatabaseNameFormatter' base class "
      u8"in order to quality as formatters."
    );

    OverrideDatabaseNameFormatter(driverName, std::make_unique<TFormatter>());
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQLCONNECTIONFACTORY_H

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
