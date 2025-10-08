#pragma region GNU GPL 3.0 License
/*
Nuclex Motion Smoother
Copyright (C) 2024-2025 Markus Ewald / Nuclex Development Labs

Licensed under the GNU GPL 3.0 license (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.gnu.org/licenses/gpl-3.0.en.html

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma endregion // GNU GPL 3.0 License

#define NUCLEX_THINORM_SOURCE 1

#include "./QVariantConverter.h"

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include "./QStringConverter.h" // for QStringConverter

#include <QDateTime> // for QDateTime
#include <QDate> // for QDate
#include <QTime> // for QTime

#include <stdexcept> // for std::runtime_error
#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Number of 1/10th microseconds that elapse every millisecond</summary>
  constexpr const std::int64_t TicksPerMillisecond = 1000 * 10;

  /// <summary>Number of 1/10th microseconds that elapse in a single day</summary>
  constexpr const std::int64_t TicksPerDay = 86400ll * 1000 * TicksPerMillisecond;

  /// <summary>Number of 1/10th microseconds on midnight of January the 1st in 1970</summary>
  constexpr const std::int64_t TicksAtStartOfUnixEpoch = 719162ll * TicksPerDay;

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  QVariant QVariantConverter::QVariantFromValue(const Value &value) {
    if(value.IsEmpty()) {
      return QVariant();
    }

    switch(value.GetType()) {
      case ValueType::Boolean: {
        return QVariant(static_cast<bool>(value));
      }
      case ValueType::Int16: {
        return QVariant(static_cast<int>(static_cast<std::int16_t>(value)));
      }
      case ValueType::Int32: {
        return QVariant(static_cast<int>(static_cast<std::int32_t>(value)));
      }
      case ValueType::Int64: {
        return QVariant(static_cast<qlonglong>(static_cast<std::int64_t>(value)));
      }
      case ValueType::Float: {
        return QVariant(static_cast<double>(static_cast<float>(value)));
      }
      case ValueType::Double: {
        return QVariant(static_cast<double>(value));
      }
      case ValueType::String: {
        return QVariant(
          Utilities::QStringConverter::FromU8(static_cast<std::u8string>(value))
        );
      }
      case ValueType::Date: {
        std::int64_t ticks = static_cast<DateTime>(value).GetTicks();
        qint64 daysSinceYearOne = ticks / TicksPerDay;
        return QVariant(QDate(1, 1, 1).addDays(daysSinceYearOne));
      }
      case ValueType::Time: {
        std::int64_t ticks = static_cast<DateTime>(value).GetTicks();
        qint64 millisecondsTimeOfDay = (
          (ticks % TicksPerDay) / TicksPerMillisecond
        );
        return QVariant(QTime::fromMSecsSinceStartOfDay(millisecondsTimeOfDay));
      }
      case ValueType::DateTime: {
        std::int64_t ticks = static_cast<DateTime>(value).GetTicks();
        qint64 millisecondsSinceUnixEpoch = (
          (ticks - TicksAtStartOfUnixEpoch) / TicksPerMillisecond
        );
        return QVariant(
          QDateTime::fromMSecsSinceEpoch(millisecondsSinceUnixEpoch, Qt::TimeSpec::UTC)
        );
      }
      case ValueType::Blob: {
        const std::vector<std::byte> &bytes = static_cast<std::vector<std::byte>>(value);
        return QVariant(
          QByteArray(
            reinterpret_cast<const char *>(bytes.data()),
            static_cast<int>(bytes.size())
          )
        );
      }
      default: {
        throw std::runtime_error(
          U8CHARS(
            u8"Attempted to convert a Nuclex::ThinOrm::Value() into a Qt variant, "
            u8"but the Nuclex::ThinOrm::Value's type cannot be represented in a Qt variant."
          )
        );
      }
    } // switch on variant type
  }

  // ------------------------------------------------------------------------------------------- //

  Value QVariantConverter::ValueFromQVariant(const QVariant &variant) {
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

  Value QVariantConverter::EmptyValueFromType(QVariant::Type variantType) {
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
            u8"Attempt to construct Nuclex::ThinOrm::Value() with equivalent type to "
            u8"Qt variant failed because the Variant had an unsupported type."
          )
        );
      }
    } // switch
  }

  // ------------------------------------------------------------------------------------------- //

  ValueType QVariantConverter::ValueTypeFromType(QVariant::Type variantType) {
    switch(variantType) {
      case QVariant::Type::Bool: { return ValueType::Boolean; }
      case QVariant::Type::Int: { return ValueType::Int32; }
      //case QVariant::Type::UInt: { return ValueType::Int32; }
      case QVariant::Type::LongLong: { return ValueType::Int64; }
      //case QVariant::Type::ULongLong: { return ValueType::Int64; }
      case QVariant::Type::Double: { return ValueType::Double; }
      //case QVariant::Type::Char: { return ValueType::Int16; }
      case QVariant::Type::String: { return ValueType::String; }
      //case QVariant::Type::Date: { return ValueType::Date; }
      //case QVariant::Type::Time: { return ValueType::Time; }
      //case QVariant::Type::DateTime: { return ValueType::DateTime; }
      default: { return ValueType(-1); } // Invalid
    } // switch
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
