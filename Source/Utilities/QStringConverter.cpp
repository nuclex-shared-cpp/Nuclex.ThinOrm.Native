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

#include "./QStringConverter.h"

#include <Nuclex/Support/Errors/CorruptStringError.h> // for CorruptStringError

#include <stdexcept> // for std::runtime_error

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Reads a code point from one or two UTF-16 characters</summary>
  /// <param name="current">Pointer from which to start reading</param>
  /// <param name="end">Pointer one past the end of the string</param>
  /// <returns>The code point that has been read or -1 on corruption or string end</returns>
  inline char32_t readCodePoint(const QChar *&current, const QChar *end) {
    assert((current < end) && u8"At least one UTF-16 character of input must be available");

    QChar leadCharacter = *current;
    if(leadCharacter < QChar(0xD800)) {
      ++current;
      return static_cast<char32_t>(leadCharacter.unicode());
    } else if(leadCharacter >= char16_t(0xE000)) {
      ++current;
      return static_cast<char32_t>(leadCharacter.unicode());
    } else if(leadCharacter < char16_t(0xDC00)) {
      if(current + 1 < end) {
        QChar trailCharacter = *(current + 1);
        bool allCharactersValid = (
          (static_cast<char16_t>(leadCharacter.unicode() & 0xFC00) == 0xD800) &&
          (static_cast<char16_t>(trailCharacter.unicode() & 0xFC00) == 0xDC00)
        );
        if(allCharactersValid) {
          current += 2;
          return char32_t(65536) + (
            (static_cast<char32_t>(leadCharacter.unicode() & 0x03FF) << 10) |
            (static_cast<char32_t>(trailCharacter.unicode() & 0x03FF))
          );
        }
      }
    }

    // Invalid character encountered or ran out of input
    return char32_t(-1);
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Encodes the specified code point into UTF-8 characters</summary>
  /// <param name="codePoint">Code point that will be encoded as UTF-8</param>
  /// <param name="target">
  ///   Address at which the UTF-8 characters will be deposited. Needs to have at
  ///   least 4 bytes of usable space and will be moved to after the encoded characters
  /// </param>
  /// <returns>
  ///   The number of characters that have been encoded or std::size_t(-1) if
  ///   you specified an invalid code point.
  /// </returns>
  inline std::size_t writeCodePoint(char8_t *target, char32_t codePoint) {
    if(codePoint < 128) [[likely]] {
      *target = static_cast<char8_t>(codePoint);
      ++target;
      return 1;
    } else if(codePoint < 2048) {
      *target = char8_t(0xC0) | static_cast<char8_t>(codePoint >> 6);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>(codePoint & 0x3F);
      ++target;
      return 2;
    } else if(codePoint < 65536) {
      *target = char8_t(0xE0) | static_cast<char8_t>(codePoint >> 12);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>((codePoint >> 6) & 0x3F);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>(codePoint & 0x3F);
      ++target;
      return 3;
    } else if(codePoint < 1114111) {
      *target = char8_t(0xF0) | static_cast<char8_t>(codePoint >> 18);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>((codePoint >> 12) & 0x3F);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>((codePoint >> 6) & 0x3F);
      ++target;
      *target = char8_t(0x80) | static_cast<char8_t>(codePoint & 0x3F);
      ++target;
      return 4;
    } else {
      assert(
        (codePoint < 1114111) && u8"Code point must be in valid unicode range"
      );
      return std::size_t(-1);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  std::u8string QStringConverter::ToU8(const QString &qtString) {
    QString::size_type length = qtString.length();
    if(length == 0) {
      return std::u8string();
    }

    // Prepare a string to receive the result. We assume that text is all English with
    // only a tiny number of code points that require multiple code units
    // (i.e. a single emoticon or special glyph from the symbol plane).
    std::u8string result(length + 8, u8'\0');
    {
      const QChar *current = qtString.constData();
      const QChar *end = current + length;

      std::u8string::size_type outIndex = 0;
      for(;;) { // No initial loop check needed because we verified that (length >= 1)
        char32_t codePoint = readCodePoint(current, end);
        if(codePoint == char32_t(-1)) [[unlikely]] {
          throw Nuclex::Support::Errors::CorruptStringError(
            reinterpret_cast<const char *>(u8"QString contains invalid UTF-16 characters")
          );
        }

        // At this point we always have at least 4 characters of space in the string,
        // so we can blindly write and check if we need to reallocate later
        outIndex += writeCodePoint(&result[outIndex], codePoint);

        // If this was the final character, exit the loop to avoid needlessly
        // enlarging the string in case we were close to the length limit
        if(current >= end) [[unlikely]] {
          break;
        }

        // If the output string is nearing its capacity, increase it in size
        if(outIndex + 4 >= result.size()) [[unlikely]] {
          result.resize(result.size() * 2);
        }
      }

      // Shrink the output string to its actual size and return it
      result.resize(outIndex);
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Utilities
