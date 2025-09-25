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

// If the library is compiled as a DLL, this ensures symbols are exported
#define NUCLEX_THINORM_SOURCE 1

#include "./QtSqlMaterializedQuery.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "../../Utilities/QStringConverter.h"

#include <QSqlQuery> // for QSqlQuery
#include <QSqlError> // for QSqlError

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::QtSqlMaterializedQuery(
    QSqlDatabase &database, const Query &query
  ) :
    qtSqlStatement(transformSqlStatement(query.GetSqlStatement(), query.GetParameterInfo())),
    qtQuery(qtSqlStatement, database) {}

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::QtSqlMaterializedQuery(
    QSqlDatabase &database, const QtSqlMaterializedQuery &other
  ) :
    qtSqlStatement(other.qtSqlStatement),
    qtQuery(other.qtSqlStatement, database) {}

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::~QtSqlMaterializedQuery() = default;

  // ------------------------------------------------------------------------------------------- //
#if 0
  std::unique_ptr<QtSqlMaterializedQuery> QtSqlMaterializedQuery::Materialize(
    QSqlDatabase &database, const Query &query
  ) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    std::unique_ptr<QtSqlMaterializedQuery> materializedQuery = (
      std::make_unique<QtSqlMaterializedQuery>(database, query)
    );
    /*
    const std::vector<QueryParameterView> &parameters = query.GetParameterInfo();
    materializedQuery->qtSqlStatement = transformSqlStatement(
      query.GetSqlStatement(), parameters
    );

    materializedQuery->qtQuery = QSqlQuery(materializedQuery->qtSqlStatement, database);
    */

    return materializedQuery;
  }
#endif
  // ------------------------------------------------------------------------------------------- //

  QString QtSqlMaterializedQuery::transformSqlStatement(
    const std::u8string &sqlStatement, const std::vector<QueryParameterView> &parameters
  ) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    QString qtSqlStatement;

    // The resulting SQL statement will turn {parameter} into :parameter, so it will
    // be shorter by one character per each parameter in the statement
    std::u8string::size_type length = sqlStatement.length();
    qtSqlStatement.reserve(length - parameters.size());

    // Skip ahead to each parameter, then append only the parameter name prefixed by
    // a double colon and do it again until all parameters and the text inbetween
    // them are appended to the QString
    QString::size_type start = 0;
    for(std::size_t index = 0; index < parameters.size(); ++index) {
      std::u8string::size_type end = parameters[index].StartIndex;

      // TODO: scan for u8':' (Qt's parameter format) and escape it if needed
      std::u8string::size_type length = end - start;
      QStringConverter::AppendU8(qtSqlStatement, sqlStatement.data() + start, length);
      qtSqlStatement.push_back(QChar(':'));
      QStringConverter::AppendU8(
        qtSqlStatement, parameters[index].Name.data(), parameters[index].Name.length()
      );

      start = end + parameters[index].Length;
    }
    if(start < length) {
      QStringConverter::AppendU8(qtSqlStatement, sqlStatement.data() + start, length - start);
    }

    return qtSqlStatement;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
