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

#include "Nuclex/ThinOrm/Errors/BadSqlStatementError.h"
#include "Nuclex/ThinOrm/Value.h"

#include <Nuclex/Support/Text/LexicalAppend.h> // for lexical_append<>()
#include <Nuclex/Support/ScopeGuard.h> // for ON_SCOPE_EXIT

#include <QSqlQuery> // for QSqlQuery
#include <QSqlError> // for QSqlError
#include <QSqlRecord> // for QSqlRecord
#include <QSqlField> // for QSqlField

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

  Value QtSqlMaterializedQuery::RunWithScalarResult() {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    // TODO: Bind any query parameters here or leave that up to the QtSqlConnection?

    // Simply execute the prepared query.
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
    }

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
          return EmptyValueFromType(record.field(0).type());
        }

        result = this->qtQuery.value(0);
      }

      // Make sure there aren't multiple rows of reulsts, otherwise, we again refuse to just
      // pick a row because the query clearly isn't one with a scalar result and we want to
      // fail early rather than attempt to wipe this discrepancy under the carpet.
      bool hasSecondResultRow = this->qtQuery.next();
      if(hasSecondResultRow) [[unlikely]] {
        std::u8string message(
          u8"Expected scalar (single value) result from SQL statement:\n", 58
        );
        message.append(Nuclex::ThinOrm::Utilities::QStringConverter::ToU8(this->qtSqlStatement));
        // CHECK: Is it safe to assume zero result column = can have no result rows either?
        message.append(u8"\nBut it returned multiple rows of results", 41);
        throw Nuclex::ThinOrm::Errors::BadSqlStatementError(message);
      }

      return ValueFromQVariant(result);
    } // scope guard for this->qtQuery.finish()
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlMaterializedQuery::ValueFromQVariant(const QVariant &variant) {
    using Nuclex::ThinOrm::Utilities::QStringConverter;

    switch(variant.type()) {
      case QVariant::Type::Bool: { return Value(std::optional<bool>(variant.toBool())); }
      case QVariant::Type::Int: { return Value(std::optional<std::int32_t>(variant.toInt())); }
      //case QVariant::Type::UInt: { return Value(std::optional<std::uint32_t>()); }
      case QVariant::Type::LongLong: {
        return Value(std::optional<std::int64_t>(variant.toLongLong()));
      }
      //case QVariant::Type::ULongLong: { return Value(std::optional<std::uint64_t>()); }
      case QVariant::Type::Double: { return Value(std::optional<double>(variant.toDouble())); }
      //case QVariant::Type::Char: { return Value(std::optional<std::int16_t>(variant.toChar())); }
      case QVariant::Type::String: {
        return Value(std::optional<std::u8string>(QStringConverter::ToU8(variant.toString())));
      }
      //case QVariant::Type::Date: { return Value(std::optional<DateTime>()); }
      //case QVariant::Type::Time: { return Value(std::optional<DateTime>()); }
      //case QVariant::Type::DateTime: { return Value(std::optional<DateTime>()); }
      default: {
        throw std::runtime_error(
          U8CHARS(
            u8"Attempt to convert Qt variant to Nuclex::ThinOrm::Value() failed because "
            u8"the Qt variant had an unsupported type."
          )
        );
      }
    } // switch
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlMaterializedQuery::EmptyValueFromType(QVariant::Type variantType) {
    switch(variantType) {
      case QVariant::Type::Bool: { return Value(std::optional<bool>()); }
      case QVariant::Type::Int: { return Value(std::optional<std::int32_t>()); }
      //case QVariant::Type::UInt: { return Value(std::optional<std::uint32_t>()); }
      case QVariant::Type::LongLong: { return Value(std::optional<std::int64_t>()); }
      //case QVariant::Type::ULongLong: { return Value(std::optional<std::uint64_t>()); }
      case QVariant::Type::Double: { return Value(std::optional<double>()); }
      //case QVariant::Type::Char: { return Value(std::optional<std::int16_t>()); }
      case QVariant::Type::String: { return Value(std::optional<std::u8string>()); }
      //case QVariant::Type::Date: { return Value(std::optional<DateTime>()); }
      //case QVariant::Type::Time: { return Value(std::optional<DateTime>()); }
      //case QVariant::Type::DateTime: { return Value(std::optional<DateTime>()); }
      default: {
        throw std::runtime_error(
          U8CHARS(
            u8"Attempt to convert Qt variant to Nuclex::ThinOrm::Value() failed because "
            u8"the Qt variant had an unsupported type."
          )
        );
      }
    } // switch
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

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
