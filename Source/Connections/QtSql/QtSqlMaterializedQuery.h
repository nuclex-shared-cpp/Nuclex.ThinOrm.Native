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

#ifndef NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLMATERIALIZEDQUERY_H
#define NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLMATERIALIZEDQUERY_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Query.h" // for Query

#include <QSqlDatabase> // for QSqlDatabase
#include <QSqlQuery> // for QSqlQuery
#include <QVariant> // for QVariant
#include <QString> // for QString

#include <memory> // for std::unique_ptr

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Materialization of a generic Query, prepared for a Qt SQL database</summary>
  class QtSqlMaterializedQuery {

    /// <summary>Initializes a new materialized query</summary>
    /// <param name="database">Qt database the materialization will be bound to</summarize>
    /// <param name="query">Query describing the SQL statement and parameters</param>
    public: QtSqlMaterializedQuery(QSqlDatabase &database, const Query &query);
    /// <summary>Initializes a new materialized query as clone of an existing query</summary>
    /// <param name="database">Qt database the materialization will be bound to</summarize>
    /// <param name="query">Query describing the SQL statement and parameters</param>
    public: QtSqlMaterializedQuery(QSqlDatabase &database, const QtSqlMaterializedQuery &other);
    /// <summary>Destroys the materialized query and frees all resources</summary>
    public: ~QtSqlMaterializedQuery();

    /// <summary>Executes the query, assuming it returns a scalar value as result</summary>
    /// <returns>The result of the query</returns>
    public: Value RunWithScalarResult();

    /// <summary>Constructs a value taking over the value from q Qt variant</summary>
    /// <param name="variant">Qt variant whose type and value will be adopted</param>
    /// <returns>A new value mirroring the type and value of the specified Qtvariant</returns>
    public: static Value ValueFromQVariant(const QVariant &variant);

    /// <summary>Constructs an empty value with a type equivalent to specified Qt type</summary>
    /// <param name="variantType">Qt variant type the empty value's type will mirror</param>
    /// <returns>A new, empty value with a type equivalent to the specified Qt type</returns>
    public: static Value EmptyValueFromType(QVariant::Type variantType);

    /// <summary>Transforms the SQL statement into the format expected by Qt SQL</summary>
    /// <param name="sqlStatement">SQL statement that will be transformed</param>
    /// <param name="parameters">Parameters contained in the SQL statement</param>
    /// <returns>
    ///   The SQL statement with its parameters in the format expeted by Qt SQL
    /// </returns>
    private: static QString transformSqlStatement(
      const std::u8string &sqlStatement, const std::vector<QueryParameterView> &parameters
    );

    /// <summary>Initializes the QSqlQuery and prepares the statement for execution</summary>
    /// <remarks>
    ///   This essentially just calls <see cref="QSqlQuery.prepare()" />, which will allow
    ///   the database driver to pre-compile the SQL statement on the database server, if
    ///   supported. This will very likely already catch query syntax errors and also when
    ///   the query references table names or other things that do not exist in the database.
    /// </remarks>
    private: void prepareSqlStatement();

    /// <summary>
    ///   The SQL statement as it has been passed to the <see cref="QSqlQuery" />
    /// </summary>
    private: QString qtSqlStatement;
    /// <summary>Prepared Qt SQL query awaiting execution</summary>
    private: QSqlQuery qtQuery;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLMATERIALIZEDQUERY_H
