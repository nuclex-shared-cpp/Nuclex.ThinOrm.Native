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

#ifndef NUCLEX_THINORM_DIALECTS_DATETIMEDIALECT_H
#define NUCLEX_THINORM_DIALECTS_DATETIMEDIALECT_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string

namespace Nuclex::ThinOrm::Dialects {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Describes the specified of an SQL database engine's date/time support</summary>
  /// <remarks>
  ///   <para>
  ///     Unfortunately, storing date and time is where the major database engines diverge
  ///     from each other quite a bit. MariaDB and MySQL do not enforce UTC but translate
  ///     into the time zone specified by the client establishing a connection. The column
  ///     types have different names, different resolutions and different minimum an maximum
  ///     representable dates for each dataabse.
  ///   </para>
  ///   <para>
  ///     Via <see cref="DateTimeDialect" />, you can look up the types you should ideally be
  ///     using to store pure dates, pure times and combined date + time values. These will
  ///     be column types specific to the database engine and you should assume that their
  ///     accuracy may be as coarse as one second intervals in the worst case. Representable
  ///     minimum and maximum dates between the year 1900 and the year 2100 should be safe in
  ///     all cases, usually the ranger is much further.
  ///   </para>
  ///   <para>
  ///     When you directly bind <see cref="DateTime" /> values via the <see cref="Value" />
  ///     class as query parameters, the type used will also be the one specified here. If
  ///     you need guaranteed microsecond precision or guaranteed readability inside an SQL
  ///     editor, use 64-bit integers with tick counts or text columns with ISO-8601 dates.
  ///   </para>
  /// </remarks>
  class DateTimeDialect {

    /// <summary>The database engine's native column type for expressing date/time</summary>
    /// <remarks>
    ///   Surprisingly, storage formats for date/time fluctuate wildly. This type guarantees
    ///   one-second accuracy and is generally sortable and less-than/greater-than comparison
    ///   for queries.
    /// </remarks>
    public: std::u8string NativeDateTimeColumnType;

    /// <summary>The database engine's native column type for expressing dates</summary>
    public: std::u8string NativeDateOnlyColumnType;

    /// <summary>The database engine's native column type for expressing time of day</summary>
    public: std::u8string NativeTimeOnlyColumnType;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Dialects

#endif // NUCLEX_THINORM_DIALECTS_DATETIMEDIALECT_H
