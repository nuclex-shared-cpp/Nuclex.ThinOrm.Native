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

#ifndef NUCLEX_THINORM_CONNECTIONS_STANDARDCONNECTIONPOOL_H
#define NUCLEX_THINORM_CONNECTIONS_STANDARDCONNECTIONPOOL_H

#include "Nuclex/ThinOrm/Config.h"
#include "Nuclex/ThinOrm/Configuration/ConnectionString.h"
#include "Nuclex/ThinOrm/Connections/ContextualConnectionPool.h"
#include "Nuclex/ThinOrm/Connections/ConnectionFactory.h"

#include <queue> // for std::queue
#include <mutex> // for std::mutex

namespace Nuclex::ThinOrm::Connections {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Manages database connections and allows them to be reused</summary>
  /// <typeparam name="TDataContext">
  ///   Specialization to distinguish the types in C++ dependency injectors.
  ///   Ignroe this if you do not use a dependency injector or if you only
  ///   access a single database in your application.
  /// </typeparam>
  /// <remarks>
  ///   Default implementation that uses a connection factory as well as a stored
  ///   <see cref="ConnectionProperties" /> instance to establish new connections as needed,
  ///   with a simple pool and a limit on how many active connections to keep ready.
  ///   connection
  /// </remarks>
  template<typename TDataContext = void>
  class NUCLEX_THINORM_TYPE StandardConnectionPool :
    public ContextualConnectionPool<TDataContext> {

    /// <summary>Initializes a new connection pool with the specified settings</summary>
    /// <param name="connectionFactory">
    ///   Factory that should be used when a new connection needs to be established
    /// </param>
    /// <param name="connectionProperties">
    ///   Settings that should be passed to the connection factory when establishing
    ///   new connections, contains driver name, database hostname or path, etc.
    /// </param>
    /// <param name="maximumRetainedConnectionCount">
    ///   Maximum number of connections to keep ready in the connection pool.
    /// </param>
    /// <remarks>
    ///   The maximum retained connection count is not a limit on the total number of
    ///   connections that might exists if there are many borrowers, just a limit after
    ///   which the connection pool will close returned connections if the pool already
    ///   contains this many active connections waiting to be borrowed.
    /// </remarks>
    public: NUCLEX_THINORM_API inline StandardConnectionPool(
      const std::shared_ptr<ConnectionFactory> &connectionFactory,
      const Configuration::ConnectionProperties &connectionProperties,
      const std::size_t maximumRetainedConnectionCount = 3
    );

    /// <summary>Frees all resources owned by the connection pool</summary>
    public: NUCLEX_THINORM_API inline ~StandardConnectionPool() override = default;

    /// <summary>Retrieves the current number of connections that the pool will retain</summary>
    /// <returns>The maximum number of connections the pool currently retains</summary>
    public: NUCLEX_THINORM_API inline std::size_t GetMaximumRetainedConnectionCount() const;

    /// <summary<Updates the number of connections the pool should retain</summary>
    /// <param name="newMaximumRetainedConnectionCount">
    ///   The new number of connections the pool should retain at most
    /// </param>
    /// <remarks>
    ///   This will immediately evict (close) currently retained connections if set to
    ///   a value below the number of connections the pool is holding on to. You can also set
    ///   this number to 0 in order to never pool any connections. This is useful for debugging
    ///   (i.e. integration testing where you want a new mock connection being created) or to
    ///   work with classes that, for simplicity, work via the <see cref="ConnectionPool" />
    ///   interface only without providing a <see cref="ConnectionFactory" /> based variant.
    /// </remarks>
    public: NUCLEX_THINORM_API inline void SetMaximumRetainedConnectionCount(
      std::size_t newMaximumRetainedConnectionCount
    );

    /// <summary>
    ///   Establishes the specified number of connections and puts them into the pool
    /// </summary>
    /// <param name="connectionCount">
    ///   Number of connections that should be established and made available through
    ///   the pool immediately
    /// </param>
    /// <remarks>
    ///   This method normally isn't needed, but if you want to reducethe warm-up time of
    ///   your application when it accesses the database for the first time after launching,
    ///   you can opt to create one or more prepared connections early.
    /// </remarks>
    public: NUCLEX_THINORM_API inline void Ready(std::size_t connectionCount);

    /// <summary>Evicts and, thus, closes all pooled connections</summary>
    public: NUCLEX_THINORM_API inline void EvictAll();

    /// <summary>Borrows a connection from the connection pool</summary>
    /// <returns>A connection to the database the connection pool has been set up for</returns>
    /// <remarks>
    ///   If there is a reusable connection sitting in the connection pool, it will be
    ///   returned for the exclusive use by the caller. Otherwise, a new connection will
    ///   be established.
    /// </remarks>
    public: NUCLEX_THINORM_API inline std::shared_ptr<Connection> BorrowConnection() override;

    /// <summary>Returns a borrowed connection to the connection pool</summary>
    /// <param name="connection">Connection to put back into the connection pool</param>
    /// <remarks>
    ///   Only return connections that are in a valid state and have no active queries,
    ///   otherwise you'll prime the next borrower for a nasty surprise that is hard to
    ///   trace back to the incorrect code that returned a connection in a bad state.
    /// </remarks>
    public: NUCLEX_THINORM_API inline void ReturnConnection(
      const std::shared_ptr<Connection> &connection
    ) override;

    /// <summary>Connection factory through which new connections are established</summary>
    private: std::shared_ptr<ConnectionFactory> connectionFactory;
    /// <summary>>Settings to use when establishing a new connection</summary>
    private: Configuration::ConnectionString connectionProperties;
    /// <summary>Mutex that must be held to access the connections</summary>
    private: std::mutex connectionsAccessMutex;
    /// <summary>Maximum number of connections the pool should retain</summary>
    private: std::size_t maximumRetainedConnectionCount;
    /// <summary>Connections currently retained in the connection pool</summary>
    private: std::queue<std::shared_ptr<Connection>> connections;

  };

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline StandardConnectionPool<TDataContext>::StandardConnectionPool(
    const std::shared_ptr<ConnectionFactory> &connectionFactory,
    const Configuration::ConnectionProperties &connectionProperties,
    const std::size_t maximumRetainedConnectionCount /* = 3 */
  ) :
    connectionFactory(connectionFactory),
    connectionProperties(connectionProperties),
    connectionsAccessMutex(),
    maximumRetainedConnectionCount(maximumRetainedConnectionCount),
    connections() {}

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline std::size_t StandardConnectionPool<
    TDataContext
  >::GetMaximumRetainedConnectionCount() const {
    return this->maximumRetainedConnectionCount;
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void StandardConnectionPool<TDataContext>::SetMaximumRetainedConnectionCount(
    std::size_t maximumRetainedConnectionCount
  ) {
    std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);

    this->maximumRetainedConnectionCount = maximumRetainedConnectionCount;
    while(maximumRetainedConnectionCount < this->connections.size()) {
      this->connections.pop();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void StandardConnectionPool<TDataContext>::Ready(std::size_t connectionCount) {
    std::size_t actualConnectionCount;
    {
      std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);
      actualConnectionCount = this->connections.size();
    }

    while(actualConnectionCount < connectionCount) {
      std::shared_ptr<Connection> newConnection = (
        this->connectionFactory->Connect(this->connectionProperties)
      );

      // Small optimization, we don't keep the locked while establishing a connection
      // so that potential borrowers aren't blocked for the entire duration it takes to
      // add a new connection to the pool.
      {
        std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);
        actualConnectionCount = this->connections.size();
        if(actualConnectionCount < connectionCount) {
          this->connections.push(std::move(newConnection));
        }
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void StandardConnectionPool<TDataContext>::EvictAll() {
    std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);
    while(!this->connections.empty()) {
      this->connections.pop();
    }
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline std::shared_ptr<Connection> StandardConnectionPool<TDataContext>::BorrowConnection() {
    {
      std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);
      if(!this->connections.empty()) {
        std::shared_ptr<Connection> result = this->connections.front();
        this->connections.pop();
        return result;
      }
    }

    return this->connectionFactory->Connect(this->connectionProperties);
  }

  // ------------------------------------------------------------------------------------------- //

  template<typename TDataContext>
  inline void StandardConnectionPool<TDataContext>::ReturnConnection(
    const std::shared_ptr<Connection> &connection
  ) {
    std::unique_lock<std::mutex> connectionsAccessScope(this->connectionsAccessMutex);
    if(this->connections.size() < this->maximumRetainedConnectionCount) {
      this->connections.push(std::move(connection));
    }
  }
  
  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections

#endif // NUCLEX_THINORM_CONNECTIONS_STANDARDCONNECTIONPOOL_H
