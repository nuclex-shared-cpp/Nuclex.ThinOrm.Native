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
#include "../../../Source/Utilities/QStringConverter.h"
#include <Nuclex/Support/Text/LexicalAppend.h>

#include <QString>

namespace {

  // ------------------------------------------------------------------------------------------- //

  class TemporaryDatabase {
    public: inline TemporaryDatabase();
    public: inline ~TemporaryDatabase();

    public: inline void OpenMemoryDatabase();
    public: inline QSqlDatabase &GetDatabase();

    private: inline static QString makeUniqueDatabaseName(std::uintptr_t uniqueId);

    private: QString connectionName;
    private: QSqlDatabase qtDatabase;
  };

  // ------------------------------------------------------------------------------------------- //

  inline TemporaryDatabase::TemporaryDatabase() :
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

  inline TemporaryDatabase::~TemporaryDatabase() {
    if(this->qtDatabase.isOpen()) {
      this->qtDatabase.close();
    }
    QSqlDatabase::removeDatabase(this->connectionName);
  }

  // ------------------------------------------------------------------------------------------- //

  inline void TemporaryDatabase::OpenMemoryDatabase() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    this->qtDatabase.setDatabaseName(
      QStringConverter::FromU8(std::u8string(u8":memory:"))
    );
    if(!this->qtDatabase.open()) {
      throw std::runtime_error(U8CHARS(u8"Failed to open memory database"));
    }
  }

  // ------------------------------------------------------------------------------------------- //

  inline QSqlDatabase &TemporaryDatabase::GetDatabase() {
    return this->qtDatabase;
  }

  // ------------------------------------------------------------------------------------------- //

  inline QString TemporaryDatabase::makeUniqueDatabaseName(std::uintptr_t uniqueId) {
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
    TemporaryDatabase tempDb;
    tempDb.OpenMemoryDatabase();

    Query testQuery(u8"SELECT * FROM users");

    std::unique_ptr<QtSqlMaterializedQuery> materializedQuery = (
      QtSqlMaterializedQuery::Materialize(tempDb.GetDatabase(), testQuery)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  TEST(QtSqlMaterializedQueryTest, CanMaterializeQueryWithParameters) {
    TemporaryDatabase tempDb;
    tempDb.OpenMemoryDatabase();

    Query testQuery(u8"SELECT * FROM users WHERE (age >= {minAge}) AND (role = {role})");

    std::unique_ptr<QtSqlMaterializedQuery> materializedQuery = (
      QtSqlMaterializedQuery::Materialize(tempDb.GetDatabase(), testQuery)
    );
  }


  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql
