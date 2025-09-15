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

#ifndef NUCLEX_THINORM_UTILITIES_QSTRINGCONVERTER_H
#define NUCLEX_THINORM_UTILITIES_QSTRINGCONVERTER_H

#include "Nuclex/ThinOrm/Config.h"

#include <QString> // for Qt's QString
#include <string> // for C++' std::u8string

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  // Turns a C++20 UTF-8 char8_t array (u8"") into a plain char array
  //
  // Why? Because using u8 expresses that the text should be stored as UTF-8,
  // regardless of how the source file is encoded.
  //
  // Safe? Yes because any type in C++ can be aliased as a char sequence. In fact, this
  // reinterpret_cast is shown as a correct example in the char8_t addition to
  // the C++ standard. The opposite way (char * to char8_t *) invokes UB, though.
  #define U8CHARS(x) (reinterpret_cast<const char *>(x))

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Provides helper methods for dealing with Qt's QString class</summary>
  class QStringConverter {

    /// <summary>Forms a Qt QString from a C++ UTF-8 string</summary>
    /// <param name="utf8String">UTF-8 string that will be converted into a QString</param>
    /// <returns>A Qt QString with equivalent contents to the C++ UTF-8 string</returns>
    public: inline static QString FromU8(const std::u8string &utf8String) {
      return QString::fromUtf8(
        U8CHARS(utf8String.data()),
        static_cast<int>(utf8String.size())
      );
    }

    /// <summary>Turns a Qt QString into a C++ UTF-8 string</summary>
    /// <param name="qtString">Qt QString that will be converted into a UTF-8 striong</param>
    /// <returns>A standard UTF-8 string equivalent contents to the Qt QString</returns>
    public: static std::u8string ToU8(const QString &qtString);

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities

#endif // NUCLEX_THINORM_UTILITIES_QSTRINGCONVERTER_H
