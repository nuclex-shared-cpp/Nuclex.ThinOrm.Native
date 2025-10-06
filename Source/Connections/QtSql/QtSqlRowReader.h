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

#ifndef NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLROWREADER_H
#define NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLROWREADER_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/RowReader.h" // for RowReader

#include <memory> // for std::shared_ptr<>

namespace Nuclex::ThinOrm::Connections::QtSql {
  class QtSqlMaterializedQuery;
}

namespace Nuclex::ThinOrm::Connections::QtSql {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads rows from a query that results in multiple rows</summary>
  class QtSqlRowReader : public RowReader {

    /// <summary>Initializes a new row reader based on a Qt SQL query</summary>
    /// <param name="materializedQuery">
    ///   Query the row reader will take temporary ownership of to prevent sharing of
    ///   its QSqlQuery whilst rows are still being enumerated
    /// </param>
    public: QtSqlRowReader(const std::shared_ptr<QtSqlMaterializedQuery> &materializedQuery);

    /// <summary>Frees all resources owned by the reader and closes the query</summary>
    public: ~QtSqlRowReader() override;

    /// <summary>Tries to move to the next row in the result</summary>
    /// <returns>True if there was a next row, false if the end was reached</returns>
    public: bool MoveToNext() override;

    /// <summary>Counts the number of columns the query result returns</summary>
    /// <returns>The number of columns in the result</returns>
    public: std::size_t CountColumns() const override;

    /// <summary>Retrieves the name of the specified column</summary>
    /// <param name="columnIndex">Index of the column whose name will be returned</param>
    /// <returns>The name of the column with the specified index</returns>
    public: const std::u8string GetColumnName(std::size_t columnIndex) const override;

    /// <summary>Looks up the data type of the specified column</summary>
    /// <param name="columnIndex">Index of the column whose data type will be looked up</param>
    /// <returns>The data type of the specified column</returns>
    public: ValueType GetColumnType(std::size_t columnIndex) const override;

    /// <summary>Retrieves the value of the specified column in the current row</summary>
    /// <param name="columnIndex">Index of the column whose value will be retrieved</param>
    /// <returns>The value of the specified column in the current row</returns>
    public: Value GetColumnValue(std::size_t columnIndex) const override;

    /// <summary>Retrieves the value of the specified column in the current row</summary>
    /// <param name="columnName">Name of the column whose value will be retrieved</param>
    /// <returns>The value of the specified column in the current row</returns>
    public: Value GetColumnValue(const std::u8string &columnName) const override;

    //private: struct ParameterInfo {
    //  public: std::u8string Name;
    //  public: ValueType Type;
    //};

    /// <summary>Materialized query the row reader has temporary ownership of</summary>
    private: std::shared_ptr<QtSqlMaterializedQuery> materializedQuery;
    //private: std::vector<ParameterInfo> parameters;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Connections::QtSql

#endif // defined(NUCLEX_THINORM_ENABLE_QT)

#endif // NUCLEX_THINORM_CONNECTIONS_QTSQL_QTSQLROWREADER_H
