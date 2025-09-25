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

#if 0
    /// <summary>Materializes the specified query for a Qt SQL database</summary>
    /// <param name="database">Database for which the query will be materialized</param>
    /// <param name="query">Query that will be materialized</param>
    /// <returns>A new Qt-specified prepared query ready for execution</returns>
    public: static std::unique_ptr<QtSqlMaterializedQuery> Materialize(
      QSqlDatabase &database, const Query &query
    );
#endif

    /// <summary>Transforms the SQL statement into the format expected by Qt SQL</summary>
    /// <param name="sqlStatement">SQL statement that will be transformed</param>
    /// <param name="parameters">Parameters contained in the SQL statement</param>
    /// <returns>
    ///   The SQL statement with its parameters in the format expeted by Qt SQL
    /// </returns>
    private: static QString transformSqlStatement(
      const std::u8string &sqlStatement, const std::vector<QueryParameterView> &parameters
    );

    /// <summary>
    ///   The SQL statement as it has been passed to the <see cref="QSqlQuery" />
    /// </summary>
    private: QString qtSqlStatement;
    /// <summary>Prepared Qt SQL query awaiting execution</summary>
    private: QSqlQuery qtQuery;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLMATERIALIZEDQUERY_H

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
