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

#ifndef NUCLEX_THINORM_CONNECTIONS_CONNECTION_H
#define NUCLEX_THINORM_CONNECTIONS_CONNECTION_H

#include "Nuclex/ThinOrm/Config.h"

#include <memory> // for std::unique_ptr<>
#include <string> // for std::u8string

namespace Nuclex::ThinOrm {
  class Value;
  class Query;
  class RowReader;
}

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Connection to a database on which queries can be run</summary>
  class NUCLEX_THINORM_TYPE Connection {

    /// <summary>Frees all resources owned by the command</summary>
    public: NUCLEX_THINORM_API virtual ~Connection() = default;

    /// <summary>Prepares the specified query for execution</summary>
    /// <param name="query">Query that will be prepared for execution</param>
    /// <remarks>
    ///   <para>
    ///     Database engines can prepare a statement (parsing it and creating an optimization
    ///     plan) before it is actually issues. Calling this with a query will prepare
    ///     the query for execution on this connection.
    ///   </para>
    ///   <para>
    ///     This happens automatically when the query is run, but if you want to improve
    ///     responsiveness and have some time for a warm-up, you can prepare your queries
    ///     before actually running them. A greater gain can be achieved by using stored
    ///     procedures, but that adds a whole new layer that needs to be kept in sync with
    ///     the application and generally adds even more differences between SQL dialect.
    ///   </para>
    ///   <para>
    ///     Implementations of the <see cref="Connection" /> are free to simply do nothing
    ///     in response to this call, so it is not safe to use for validating statements.
    ///   </para>
    /// </remarks>
    public: NUCLEX_THINORM_API inline virtual void Prepare(const Query &query) { (void)query; }

    /// <summary>Executes an SQL query that has no results on the database</summary>
    /// <param name="statement">Statement that will be executed</param>
    public: virtual void RunStatement(const Query &statement) = 0;
    
    /// <summary>Executes an SQL query that has a single result on the database</summary>
    /// <param name="scalarQuery">Query that will be executed</param>
    /// <returns>The result of the query</returns>
    public: virtual Value RunScalarQuery(const Query &scalarQuery) = 0;

    /// <summary>Executes an SQL query that updates (or deletes) rows in the database</summary>
    /// <param name="updateQuery">Query that will be executed</param>
    /// <returns>The number of affected rows</returns>
    public: virtual std::size_t RunUpdateQuery(const Query &updateQuery) = 0;

    /// <summary>Executes an SQL query that has result rows on the database</summary>
    /// <param name="rowQuery">Query that will be executed</param>
    /// <returns>A reader that can be used to fetch individual rows</returns>
    public: virtual std::unique_ptr<RowReader> RunRowQuery(const Query &rowQuery) = 0;

    /// <summary>Checks if the specified table exists</summary>
    /// <param name="tableName">Table or view whose existence will be checked</param>
    /// <returns>True if a table or view with the given exists</returns>
    /// <remarks>
    ///   This is exposed on the conenction level because there is no uniform way to check
    ///   whether a tabel exists. Ordinarily, database engines should expose a special
    ///   database named <code>INFORMATION_SCHEMA</code>, but in practice, this isn't
    ///   guaranteed, so this leaves it up to the driver to implement the correct way
    ///   for the database or to check via a dummy query.
    /// </remarks>
    public: virtual bool DoesTableOrViewExist(const std::u8string &tableName) = 0;

    // Dialect tags?
    //
    // Could be used by query formatters to decide what to do in a controlled way
    // without having to do completely separate formatters per SQL dialect or
    // ending up with a 'if(dialectName.contains(u8"...""))' sections everywhere.
    //
    //const SqlDialect &GetDialect() const;

    /*
    public: virtual const std::u8string &GetDatabaseProductName() = 0;

    public: virtual const std::u8string &GetDialectName() = 0;

    public: virtual const std::u8string &GetConnectorName() = 0;
    */

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_CONNECTION_H
