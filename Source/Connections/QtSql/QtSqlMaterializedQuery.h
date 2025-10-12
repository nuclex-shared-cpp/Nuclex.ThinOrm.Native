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

namespace Nuclex::ThinOrm {
  class RowReader;
  enum class ValueType;
}

namespace Nuclex::ThinOrm::Connections::QtSql {
  class QtSqlRowReader;
}

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Materialization of a generic Query, prepared for a Qt SQL database</summary>
  class QtSqlMaterializedQuery {
    friend QtSqlRowReader;

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

    /// <summary>Binds the parameter values from the specified query</summary>
    /// <param name="query">Query whose parameter values will be bound</param>
    /// <remarks>
    ///   The query must have the same number of names of parameters as the query that was
    ///   passed to the constructor of the materialized query instance.
    /// </remarks>
    public: void BindParameters(const Query &query);

    /// <summary>Executes the query, assuming it is a statement that returns nothing</summary>
    public: void RunWithoutResult();

    /// <summary>Executes the query, assuming it returns a scalar value as result</summary>
    /// <returns>The result of the query</returns>
    public: Value RunWithScalarResult();

    /// <summary>Executes the query, assuming it affects zero or more rows</summary>
    /// <returns>The result of the query</returns>
    /// <remarks>
    ///   SQL drivers usually provide a separate path to obtaining the number of rows
    ///   a query has affected. This method will thus not return any result generated
    ///   by the query, but the number of rows the SQL driver reported to be affected.
    /// </remarks>
    public: std::size_t RunWithRowCountResult();

    /// <summary>Executes the query, assuming it returns zero or more result rows</summary>
    /// <param name="self">
    ///   Essentially the this pointer, provided by the connection rather than through
    ///   <code>std::shared_from_this&lt;&gt;</code> in order for
    ///   the <see cref="RowReader" /> to take temporary ownership of the materialized query.
    /// </param>
    /// <returns>A row reader which acts as an enumerator and row metadata provider</returns>
    public: std::unique_ptr<RowReader> RunWithMultiRowResult(
      // TODO: This will also needs a materialized query cache return callback
      const std::shared_ptr<QtSqlMaterializedQuery> &self
    );

    /// <summary>Accesses the Qt SQL query managed by the materialized query wrapper</summary>
    /// <returns>The Qt SQL query the wrapper is managing</returns>
    protected: inline QSqlQuery &GetQtQuery();

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

    /// <summary>Executes the SQL statement via Qt's QSqlQuery</summary>
    /// <remarks>
    ///   This method is intended to be used from the various <code>Run...()</code> methods
    ///   and merely wraps the <see cref="QSqlQuery.exec()" /> method so the error handling
    ///   code doesn't have to be repeated.
    /// </remarks>
    private: void executeQuery();

    /// <summary>
    ///   The SQL statement as it has been passed to the <see cref="QSqlQuery" />
    /// </summary>
    private: QString qtSqlStatement;
    /// <summary>Prepared Qt SQL query awaiting execution</summary>
    private: QSqlQuery qtQuery;

  };

  // ------------------------------------------------------------------------------------------- //

  inline QSqlQuery &QtSqlMaterializedQuery::GetQtQuery() {
    return this->qtQuery;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLMATERIALIZEDQUERY_H
