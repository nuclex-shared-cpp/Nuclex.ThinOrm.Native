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

#include "../../Utilities/QStringConverter.h" // for QStringConverter
#include "../../Utilities/QVariantConverter.h" // for QVariantConverter

#include "Nuclex/ThinOrm/Errors/BadSqlStatementError.h" // for BadSqlStatementError
#include "Nuclex/ThinOrm/Value.h" // for Value
#include "./QtSqlRowReader.h" // for QtSqlRowReader

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()
#include <Nuclex/Support/ScopeGuard.h> // for ON_SCOPE_EXIT

#include <QSqlQuery> // for QSqlQuery
#include <QSqlError> // for QSqlError
#include <QSqlRecord> // for QSqlRecord
#include <QSqlField> // for QSqlField
#include <QDate> // for QDate
#include <QTime> // for QTime
#include <QDateTime> // for QDateTime

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Throws an exception if a query has not returned exactly one result</summary>
  /// <param name="record">Record that reports the number of result columns</param>
  /// <param name="qtSqlStatement">
  ///   SQL statement that has been executed, will be provided together with the error
  ///   message to aid the user in debugging (including in production)
  /// </param>
  void requireSingleColumnForScalarQuery(QSqlRecord record, const QString &qtSqlStatement) {
    int columnCount = record.count();
    if(columnCount < 1) [[unlikely]] {
      std::u8string message(
        u8"Expected scalar (single value) result from SQL statement:\n", 58
      );
      message.append(Nuclex::ThinOrm::Utilities::QStringConverter::ToU8(qtSqlStatement));
      // CHECK: Is it safe to assume zero result column = can have no result rows either?
      message.append(u8"\nBut it returned no results at all", 34);
      throw Nuclex::ThinOrm::Errors::BadSqlStatementError(message);
    }
    if(columnCount >= 2) [[unlikely]] { // Separate error message for too many columns
      std::u8string message(
        u8"Expected scalar (single value) result from SQL statement:\n", 58
      );
      message.append(Nuclex::ThinOrm::Utilities::QStringConverter::ToU8(qtSqlStatement));
      message.append(u8"\nBut it returned ", 17);
      Nuclex::Support::Text::lexical_append(message, columnCount);
      message.append(u8" columns instead.", 17);
      throw Nuclex::ThinOrm::Errors::BadSqlStatementError(message);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::QtSqlMaterializedQuery(
    QSqlDatabase &database, const Query &query
  ) :
    qtSqlStatement(transformSqlStatement(query.GetSqlStatement(), query.GetParameterInfo())),
    qtQuery(database) {
    prepareSqlStatement();
  }

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::QtSqlMaterializedQuery(
    QSqlDatabase &database, const QtSqlMaterializedQuery &other
  ) :
    qtSqlStatement(other.qtSqlStatement),
    qtQuery(database) {
    prepareSqlStatement();
  }

  // ------------------------------------------------------------------------------------------- //

  QtSqlMaterializedQuery::~QtSqlMaterializedQuery() = default;

  // ------------------------------------------------------------------------------------------- //

  void QtSqlMaterializedQuery::BindParameters(const Query &query) {
    using Nuclex::ThinOrm::Utilities::QVariantConverter;

    const std::vector<QueryParameterView> &parameterInfo = query.GetParameterInfo();
    for(std::size_t index = 0; index < parameterInfo.size(); ++index) {
      this->qtQuery.bindValue(
        index, QVariantConverter::QVariantFromValue(query.GetParameterValue(index))
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void QtSqlMaterializedQuery::RunWithoutResult() {
    executeQuery();
    this->qtQuery.finish();
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlMaterializedQuery::RunWithScalarResult() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;
    using Nuclex::ThinOrm::Utilities::QVariantConverter;

    executeQuery();
    {
      ON_SCOPE_EXIT { this->qtQuery.finish(); };

      // Extract the resulting value
      QVariant result;
      {
        QSqlRecord record = this->qtQuery.record();

        // The result should only have one column. If not, we refuse to just pick a column
        // because the query is clearly incorrect and it's better to fail early than to
        // go around and try guessing things in hopes that they may end up being correct.
        requireSingleColumnForScalarQuery(record, this->qtSqlStatement);

        // Fetch the first result row of the query. If there is not result, the query is
        // still considered valid and it is assumpted to have returned an empty result
        // CHECK: Is this sane? Would Qt SQL rather return 'NULL' in some other way?
        bool hasFirstResultRow = this->qtQuery.next();
        if(!hasFirstResultRow) [[unlikely]] {
#if 0 // Treat zero rows as empty result (bad idea, I think)
          return EmptyValueFromType(record.field(0).type());
#else // vv zero rows means error vv // ^^ zero rows means empty result ^^
          std::u8string message(
            u8"Expected scalar (single value) result from SQL statement:\n", 58
          );
          message.append(QStringConverter::ToU8(this->qtSqlStatement));
          message.append(u8"\nBut it returned no results", 27);
          throw Nuclex::ThinOrm::Errors::BadSqlStatementError(message);
#endif
        }

        result = this->qtQuery.value(0);
      }

      // Make sure there aren't multiple rows of results, otherwise, we again refuse to just
      // pick a row because the query clearly isn't one with a scalar result and we want to
      // fail early rather than attempt to wipe this discrepancy under the carpet.
      bool hasSecondResultRow = this->qtQuery.next();
      if(hasSecondResultRow) [[unlikely]] {
        std::u8string message(
          u8"Expected scalar (single value) result from SQL statement:\n", 58
        );
        message.append(QStringConverter::ToU8(this->qtSqlStatement));
        message.append(u8"\nBut it returned multiple rows of results", 41);
        throw Nuclex::ThinOrm::Errors::BadSqlStatementError(message);
      }

      return QVariantConverter::ValueFromQVariant(result);
    } // scope guard for this->qtQuery.finish()
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t QtSqlMaterializedQuery::RunWithRowCountResult() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    executeQuery();
    {
      ON_SCOPE_EXIT { this->qtQuery.finish(); };
      return static_cast<std::size_t>(this->qtQuery.numRowsAffected());
    }
  }

  // ------------------------------------------------------------------------------------------- //

  std::unique_ptr<RowReader> QtSqlMaterializedQuery::RunWithMultiRowResult(
    const std::shared_ptr<QtSqlMaterializedQuery> &materializedQuery
  ) {
    return std::make_unique<QtSqlRowReader>(materializedQuery);
  }

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

  void QtSqlMaterializedQuery::prepareSqlStatement() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    bool successfullyPrepared = this->qtQuery.prepare(this->qtSqlStatement);
    if(!successfullyPrepared) [[unlikely]] {
      std::u8string message(u8"Error preparing SQL statement:\n", 31);
      message.append(QStringConverter::ToU8(this->qtSqlStatement));
      message.append(u8"\nReason provided by Qt SQL:\n", 28);

      QSqlError lastError = this->qtQuery.lastError();
      if(lastError.type() == QSqlError::ErrorType::NoError) [[unlikely]] {
        message.append(u8"unknown QtSql error ", 20);
        message.append(u8"(.prepare() returned false, yet .lastError() was 'NoError')", 59);
      } else {
        message.append(QStringConverter::ToU8(lastError.text()));
      }

      throw Errors::BadSqlStatementError(message);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void QtSqlMaterializedQuery::executeQuery() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    bool successfullyExecuted = this->qtQuery.exec();
    if(!successfullyExecuted) [[unlikely]] {
      std::u8string message(u8"Error executing SQL statement:\n", 31);
      message.append(QStringConverter::ToU8(this->qtSqlStatement));
      message.append(u8"\nReason provided by Qt SQL:\n", 28);

      QSqlError lastError = this->qtQuery.lastError();
      if(lastError.type() == QSqlError::ErrorType::NoError) [[unlikely]] {
        message.append(u8"unknown QtSql error ", 20);
        message.append(u8"(.prepare() returned false, yet .lastError() was 'NoError')", 59);
      } else {
        message.append(QStringConverter::ToU8(lastError.text()));
      }

      throw Errors::BadSqlStatementError(message);
    } // if query failed
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
