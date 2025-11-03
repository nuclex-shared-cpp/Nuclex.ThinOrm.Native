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

#ifndef NUCLEX_THINORM_UTILITIES_QVARIANTCONVERTER_H
#define NUCLEX_THINORM_UTILITIES_QVARIANTCONVERTER_H

#include "Nuclex/ThinOrm/Config.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "Nuclex/ThinOrm/Value.h" // for Value
#include <QVariant> // for Qt's QVariant

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Provides helper methods for dealing with Qt's QVariant class</summary>
  class QVariantConverter {

    /// <summary>Constructs a Qt variant taking over a value stored in the Valu class</summary>
    /// <param name="value">Value whose type and value will be stored in the Qt variant</param>
    /// <returns>A new Qt variant mirroring the type and value of the input Value</returns>
    public: static QVariant QVariantFromValue(const Value &value);

    /// <summary>Constructs a value taking over the value from a Qt variant</summary>
    /// <param name="variant">Qt variant whose type and value will be adopted</param>
    /// <returns>A new value mirroring the type and value of the specified Qtvariant</returns>
    public: static Value ValueFromQVariant(const QVariant &variant);

    /// <summary>Constructs an empty value with a type equivalent to specified Qt type</summary>
    /// <param name="variantType">Qt variant type the empty value's type will mirror</param>
    /// <returns>A new, empty value with a type equivalent to the specified Qt type</returns>
    public: static Value EmptyValueFromType(QVariant::Type variantType);

    /// <summary>Returns the ValueType equivalent to a Qt Variant type</summary>
    /// <param name="variantType">Qt variant type whose equivalent ValueType to look up</param>
    /// <returns>The ValueType equivalent to the specifeid Qt Variant type</returns>
    public: static ValueType ValueTypeFromType(QVariant::Type variantType);

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // defined(NUCLEX_THINORM_ENABLE_QT)

#endif // NUCLEX_THINORM_UTILITIES_QVARIANTCONVERTER_H
