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

#ifndef NUCLEX_THINORM_CONNECTIONS_CONNECTIONPOOL_H
#define NUCLEX_THINORM_CONNECTIONS_CONNECTIONPOOL_H

#include "Nuclex/ThinOrm/Config.h"

#include <memory> // for std::shared_ptr

namespace Nuclex::ThinOrm::Connections {
  class Connection;
}

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Establishes database connections and allows them to be reused</summary>
  /// <remarks>
  ///   <para>
  ///     Data contexts can either use an explicit user-provided connections or
  ///     &quot;borrow&quot; a connection from a connection pool. This is just the interface
  ///     through which that happens. Typically, you'd set up a connection factory in
  ///     your application, then the <see cref="StandardConnectionPool" /> and pass that
  ///     through your data context.
  ///   </para>
  ///   <para>
  ///     Using a connection pool interface as the basic building block instead of just
  ///     a connection factory makes it siple and obvious to use pooling rather than hide
  ///     it behind a level of indirection tha returns an internal connection when some
  ///     exposed connection object gets destroyed or similar.
  ///   </para>
  /// </remarks>
  class NUCLEX_THINORM_TYPE ConnectionPool {

    /// <summary>Frees all resources owned by the connection pool</summary>
    public: NUCLEX_THINORM_API virtual ~ConnectionPool() = default;

    /// <summary>Borrows a connection from the connection pool</summary>
    /// <returns>A connection to the database the connection pool has been set up for</returns>
    /// <remarks>
    ///   If there is a reusable connection sitting in the connection pool, it will be
    ///   returned for the exclusive use by the caller. Otherwise, a new connection will
    ///   be established.
    /// </remarks>
    public: virtual std::shared_ptr<Connection> BorrowConnection() = 0;

    /// <summary>Returns a borrowed connection to the connection pool</summary>
    /// <param name="connection">Connection to put back into the connection pool</param>
    /// <remarks>
    ///   Only return connections that are in a valid state and have no active queries,
    ///   otherwise you'll prime the next borrower for a nasty surprise that is hard to
    ///   trace back to the incorrect code that returned a connection in a bad state.
    /// </remarks>
    public: virtual void ReturnConnection(const std::shared_ptr<Connection> &connection) = 0;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_CONNECTIONPOOL_H
