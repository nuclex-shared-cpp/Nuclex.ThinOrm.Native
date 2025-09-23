#pragma region Apache License 2.0
/*
Nuclex Native Framework
Copyright (C) 2002-2025 Markus Ewald / Nuclex Development Labs

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

#include <gtest/gtest.h>

#include "../../../Source/Connections/QtSql/QtSqlMaterializedQuery.h"
#include "../../../Source/Utilities/QStringConverter.h" // for QStringConverter
#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>

#include <QString> // for QString

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Sets up a temporary in-memory Qt database for testing</summary>
  class TemporaryDatabaseScope {

    /// <summary>Prepares a new in-memory database</summary>
    public: inline TemporaryDatabaseScope();
    /// <summary>Destroys the in-memory database, invalidating all open queries</summary>
    public: inline ~TemporaryDatabaseScope();

    /// <summary>Opens the in-memory database. Must be called before use</summary>
    public: inline void OpenMemoryDatabase();

    /// <summary>Fetches the Qt database instance (unique to the scope)</summary>
    /// <returns>The Qt database instance</returns>
    public: inline QSqlDatabase &GetDatabase();

    /// <summary>Builds a unique name for the database</summary>
    /// <param name="uniqueId">
    ///   A unique value that should only exist once per active database
    /// </param>
    /// <returns>A Qt string containing a unique but descriptive name</returns>
    private: inline static QString makeUniqueDatabaseName(std::uintptr_t uniqueId);

    /// <summary>Name of the unique database instance, needed for cleanup</summary>
    private: QString connectionName;
    /// <summary>The temporary in-memory database created for this scope</summary>
    private: QSqlDatabase qtDatabase;

  };

  // ------------------------------------------------------------------------------------------- //

  inline TemporaryDatabaseScope::TemporaryDatabaseScope() :
    connectionName(),
    qtDatabase() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    std::intptr_t uniqueId = reinterpret_cast<std::intptr_t>(this);
    this->connectionName = makeUniqueDatabaseName(uniqueId);

    this->qtDatabase = QSqlDatabase::addDatabase(
      QStringConverter::FromU8(std::u8string(u8"QSQLITE")), this->connectionName
    );
  }

  // ------------------------------------------------------------------------------------------- //

  inline TemporaryDatabaseScope::~TemporaryDatabaseScope() {
    if(this->qtDatabase.isOpen()) {
      this->qtDatabase.close();
    }
    QSqlDatabase::removeDatabase(this->connectionName);
  }

  // ------------------------------------------------------------------------------------------- //

  inline void TemporaryDatabaseScope::OpenMemoryDatabase() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    this->qtDatabase.setDatabaseName(
      QStringConverter::FromU8(std::u8string(u8":memory:"))
    );
    if(!this->qtDatabase.open()) {
      throw std::runtime_error(U8CHARS(u8"Failed to open memory database"));
    }
  }

  // ------------------------------------------------------------------------------------------- //

  inline QSqlDatabase &TemporaryDatabaseScope::GetDatabase() {
    return this->qtDatabase;
  }

  // ------------------------------------------------------------------------------------------- //

  inline QString TemporaryDatabaseScope::makeUniqueDatabaseName(std::uintptr_t uniqueId) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    std::u8string name(u8"temp-", 5);
    Nuclex::Support::Text::lexical_append(name, uniqueId);

    return QStringConverter::FromU8(name);
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  TEST(QtSqlMaterializedQueryTest, CanMaterializeParameterlessQuery) {
    TemporaryDatabaseScope tempDb;
    tempDb.OpenMemoryDatabase();

    Query testQuery(u8"SELECT * FROM users");

    QtSqlMaterializedQuery materializedQuery(tempDb.GetDatabase(), testQuery);
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QtSqlMaterializedQueryTest, CanMaterializeQueryWithParameters) {
    TemporaryDatabaseScope tempDb;
    tempDb.OpenMemoryDatabase();

    Query testQuery(u8"SELECT * FROM users WHERE (age >= {minAge}) AND (role = {role})");

    QtSqlMaterializedQuery materializedQuery(tempDb.GetDatabase(), testQuery);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql
