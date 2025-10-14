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

#ifndef NUCLEX_THINORM_DATACONTEXT_H
#define NUCLEX_THINORM_DATACONTEXT_H

#include "Nuclex/ThinOrm/Config.h"

#include <memory> // for std::shared_ptr<>

namespace Nuclex::ThinOrm::Connections {
  class Connection;
  class ConnectionPool;
}

namespace Nuclex::ThinOrm {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Represents a database and exposes all its tables</summary>
  /// <remarks>
  ///   <para>
  ///     The data context is used as a kind of key to identify a database with some
  ///     of the other classes (i.e. you can specialize a <see cref="StandardConnectionPool" />
  ///     on different data context so you can require the correct connection rool when your
  ///     application accesses multiple databases) and serves as the main access hub for
  ///     fluent queries.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE DataContext {

    /// <summary>Initializes a data context on an already established connection</summary>
    /// <param name="connection">Database connection the data context will be using</param>
    /// <remarks>
    ///   The data context will permanently hold onto the connection and use it exclusively
    ///   for all database queries. That means that parallel queries are not possible. This
    ///   can be an okay solution for small cases, i.e. simple web applications where each
    ///   request does data processing in an exclusive thread, or unit tests and migrations
    ///   where you only want a single connections to access the database at a time.
    /// </remarks>
    public: NUCLEX_THINORM_API DataContext(
      const std::shared_ptr<Connections::Connection> &connection
    );

    /// <summary>
    ///   Initializes a data context that is able to borrow connections on demand
    /// </summary>
    /// <param name="pool">Pool from which connections will be borrowed as needed</param>
    /// <remarks>
    ///   Prefer this constructor whenever you can. By using a connecton pool, you do not
    ///   &quot;hog&quot; a database connection right away and you can run multiple queries
    ///   at the same time. This is especially important if you use <see cref="RowReader" />
    ///   to read a multi-row query result while potentially peforming other queries.
    /// </remarks>
    public: NUCLEX_THINORM_API DataContext(
      const std::shared_ptr<Connections::ConnectionPool> &pool
    );

    /// <summary>Frees all resources owned by the context</summary>
    public: NUCLEX_THINORM_API virtual ~DataContext();

    /// <summary>Connection that should be exclusively used for all data access</summary>
    /// <remarks>
    ///   If this is set, only this one connection will be used. When a data context is
    ///   constructed using a connection pool, which is the recommended setup, it will
    ///   simply remain a nullptr.
    /// </remarks>
    private: std::shared_ptr<Connections::Connection> connection;
    /// <summary>Connection pool from which connections should be borrowed as needed</summary>
    private: std::shared_ptr<Connections::ConnectionPool> pool;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm

#endif // NUCLEX_THINORM_DATACONTEXT_H
