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

#include "./QtSqlRowReader.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "./QtSqlMaterializedQuery.h" // for QtSqlMaterializedQuery

#include "../../Utilities/QStringConverter.h"
#include "../../Utilities/QVariantConverter.h"

#include <QSqlQuery> // for QSqlQuery
#include <QSqlRecord> // for QSqlRecord
#include <QSqlError> // for QSqlError
#include <QSqlField> // for QSqlField

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //
  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  QtSqlRowReader::QtSqlRowReader(
    const std::shared_ptr<QtSqlMaterializedQuery> &materializedQuery
  ) : materializedQuery(materializedQuery) {}

  // ------------------------------------------------------------------------------------------- //

  QtSqlRowReader::~QtSqlRowReader() = default;
  // TODO: Return ownership of the QtSqlMaterializedQuery to the connection

  // ------------------------------------------------------------------------------------------- //

  bool QtSqlRowReader::MoveToNext() {
    return this->materializedQuery->GetQtQuery().next(); //Result();
  }

  // ------------------------------------------------------------------------------------------- //

  std::size_t QtSqlRowReader::CountColumns() const {
    return static_cast<std::size_t>(
      this->materializedQuery->GetQtQuery().record().count()
    );
  }

  // ------------------------------------------------------------------------------------------- //

  const std::u8string QtSqlRowReader::GetColumnName(std::size_t columnIndex) const {
    return Utilities::QStringConverter::ToU8(
      this->materializedQuery->GetQtQuery().record().fieldName(columnIndex)
    );
  }

  // ------------------------------------------------------------------------------------------- //

  ValueType QtSqlRowReader::GetColumnType(std::size_t columnIndex) const {
    using Nuclex::ThinOrm::Utilities::QVariantConverter;

    const QSqlRecord &record = this->materializedQuery->GetQtQuery().record();
    QVariant::Type fieldType = record.field(static_cast<int>(columnIndex)).type();
    return QVariantConverter::ValueTypeFromType(fieldType);
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlRowReader::GetColumnValue(std::size_t columnIndex) const {
    using Nuclex::ThinOrm::Utilities::QVariantConverter;

    QVariant value = this->materializedQuery->GetQtQuery().value(static_cast<int>(columnIndex));
    return QVariantConverter::ValueFromQVariant(value);
  }

  // ------------------------------------------------------------------------------------------- //

  Value QtSqlRowReader::GetColumnValue(const std::u8string &columnName) const {
    using Nuclex::ThinOrm::Utilities::QVariantConverter;

    QVariant value = this->materializedQuery->GetQtQuery().value(
      Utilities::QStringConverter::FromU8(columnName)
    );
    return QVariantConverter::ValueFromQVariant(value);
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
