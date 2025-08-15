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

#ifndef NUCLEX_DATABASE_ISOLATIONLEVEL_H
#define NUCLEX_DATABASE_ISOLATIONLEVEL_H

#include "Nuclex/Database/Config.h"

namespace Nuclex { namespace Database {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>
  ///   How SQL statements inside transactions will be isolated from other transactions
  /// </summary>
  /// <remarks>
  ///   <para>
  ///     While care has been taken to find common denominators for isolation levels supported
  ///     by all popular databases, some of the behavior is still implementation specific
  ///     (though you can develop database access patterns that work pretty universally, such
  ///     as full isolation with an application-enforced single writer).
  ///   </para>
  ///   <para>
  ///     If a database does not support such granular control over isolation levels,
  ///     the next higher isolation level that makes at least the same guaranteed and is
  ///     supported by the database is used. So this, unfortunately, is one of those situations
  ///     where you either have to cargo-cult the aforementioned usage pattern, accept chaos
  ///     or delve into the transaction minutiae of the database engines you plan to support.
  ///   </para>
  ///   <para>
  ///     For SQLite, check https://www.sqlite.org/lang_transaction.html.
  ///     For MariaDB, check https://mariadb.com/docs/server/reference/sql-statements/transactions/set-transaction.
  ///     For PostgreSQL, check https://www.postgresql.org/docs/current/transaction-iso.html.
  ///     For SQL Server, check https://learn.microsoft.com/en-us/sql/t-sql/statements/set-transaction-isolation-level-transact-sql.
  ///   </para>
  /// </remarks>
  enum class NUCLEX_DATABASE_TYPE IsolationLevel {

    /// <summary>The default isolation level of the database is used</summary>
    /// <remarks>
    ///   What this means is unspecified, but in general, the default will "just work"
    ///   and should not be a problem for single-threaded access from one process only.
    /// </remarks>
    Default = 0,

    /// <summary>Always fetch latest committed data, fail on any conflict</summary>
    /// <remarks>
    ///   <para>
    ///     Reads will always fetch the latest committed data. If changes intersect in
    ///     any way with pending changes from other transactions (usually that means touching
    ///     the same table), this transaction fails.
    ///   </para>
    ///   <para>
    ///     A good choice if you have any kind of unique id generation routine that cannot
    ///     make use of auto-generated columns, where you'd fetch the current highest id and
    ///     try to insert the next higher one  until you succeed.
    ///   </para>
    /// </summary>>
    CommittedReads_FailConflictingWrites,

    /// <summary>Pin any fetched rows, fail on any conflict</summary>
    /// <remarks>
    ///   <para>
    ///     Any individual rows have been touched by the query are pinned to their state
    ///     for the duration of the transaction, but if any transactions commit that insert
    ///     new rows or delete existing rows, these are still reflected in the queries.
    ///   </para>
    /// </remarks>
    RepeatableReads_FailConflictingWrites,

    /// <summary>Pin queried row ranges, fail on any conflict</summary>
    /// <remarks>
    ///   <para>
    ///     This will pin queried ranges of rows and guarantees that the same rows will be
    ///     returned if a query is repeated. Other transactions wanting to touch the same
    ///     rows will be suspended and vice versa until a row range can be locked.
    ///   </para>
    ///   <para>
    ///     Notice that this still doesn't guarantee a whole-table lock. If you query rows
    ///     10-19 from a table, then another transaction commits that deletes entry 5,
    ///     if you successively query rows 20-29, you will have skipped over one row
    ///     because the table's non-locked rows shifted up by one row.
    ///   </para>
    /// </remarks>
    PinnedReads_BlockConcurrentWrites,

    /// <summary>Snapshot whole database for reads, allow only one write at a time</summary>
    /// <remarks>
    ///   <para>
    ///     This makes the database behaves similar to a classic reader/writer lock - many
    ///     readers can query the database at the same time and will see the state of
    ///     the database at the point when either the transaction was started or when they
    ///     issued the first SQL statement.
    ///   </para>
    ///   <para>
    ///     Writers are exclusive, meaning other connections attempting to write will be
    ///     blocked until any other potential write transaction completes. Since transactions
    ///     aren't explicitly qualified as &quot;read-only&quot; or &quot;write&quot;
    ///     transactions, this can still result in two transactions being opened, each
    ///     successively becoming write transactions but issuing conflicting SQL statements
    ///     because both operate based on the state of the database when they were started.
    ///   </para>
    ///   <para>
    ///     This will cause the transaction to fail and roll back during commit.
    ///   </para>
    /// </remarks>
    Isolated_BlockConcurrentWrites

  };

  // ------------------------------------------------------------------------------------------- //

}} // namespace Nuclex::Database

#endif // NUCLEX_DATABASE_ISOLATIONLEVEL_H
