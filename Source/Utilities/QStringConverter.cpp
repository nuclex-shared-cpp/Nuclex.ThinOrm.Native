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

#if defined(NUCLEX_THINORM_ENABLE_QT)

#include <Nuclex/Support/Errors/CorruptStringError.h> // for CorruptStringError
#include <Nuclex/Support/Text/UnicodeHelper.h> // for UnicodeHelper

#include <cassert> // for assert()

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

  /// <summary>Encodes the specified code point into UTF-16 characters</summary>
  /// <param name="codePoint">Code point that will be encoded as UTF-16</param>
  /// <param name="target">
  ///   Address at which the UTF-16 characters will be deposited. Needs to have at
  ///   least 4 bytes of usable space and will be moved to after the encoded characters
  /// </param>
  /// <returns>
  ///   The number of characters that have been encoded or std::size_t(-1) if
  ///   you specified an invalid code point.
  /// </returns>
  inline std::size_t writeCodePoint(QChar *&target, char32_t codePoint) {
    if(codePoint < 65536) {
      assert(
        ((codePoint < 0xDC00) || (codePoint >= 0xE000)) &&
        u8"Unicode code point has to be outside surrogate range (0xDC00-0xDFFF)"
      );
      // If Q_COMPILER_UNICODE_STRINGS is set, this should construct a QChar implicitly
      *target = static_cast<char16_t>(codePoint);
      ++target;
      return 1;
    } else if(codePoint < 1114111) {
      codePoint -= char32_t(65536);
      *(target) = 0xD800 | static_cast<char16_t>(codePoint >> 10);
      *(target + 1) = 0xDC00 | static_cast<char16_t>(codePoint & 0x03FF);
      target += 2;
      return 2;
    } else {
      return std::size_t(-1);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Utilities {

  // ------------------------------------------------------------------------------------------- //

  void QStringConverter::AppendU8(
    QString &target, const char8_t *source, std::size_t length
  ) {
    QString::size_type actualLength = target.length();
    QString::size_type guessedNewLength = actualLength;
    if(length >= 4) {
      guessedNewLength += length; // on the assumpton we're mostly dealing with ASCII
    } else {
      guessedNewLength += 4; // to not risk a buffer overflow, we need at least 4 bytes
    }

    target.resize(guessedNewLength);
    QChar *write = target.data() + actualLength;

    const char8_t *sourceEnd = source + length;
    while(source < sourceEnd) {

      // Read the next code point from the UTF-8 string. This will automatically update
      // the source pointer, ensuring the while() eventually terminates
      char32_t codePoint = (
        Nuclex::Support::Text::UnicodeHelper::ReadCodePoint(source, sourceEnd)
      );
      if(codePoint == char32_t(-1)) [[unlikely]] {
        throw Nuclex::Support::Errors::CorruptStringError(
          reinterpret_cast<const char *>(u8"UTF-8 char array contains invalid characters")
        );
      }

      // Write the code point into the QString (which is UTF-16 encoded). Unless we're
      // dealing with code points outside the unicode table, this should always succeeed.
      std::size_t writtenCharCount = writeCodePoint(write, codePoint);
      if(writtenCharCount == std::size_t(-1)) [[unlikely]] {
        throw Nuclex::Support::Errors::CorruptStringError(
          reinterpret_cast<const char *>(u8"UTF-8 char array encodes invalid code points")
        );
      }

      // While we expect to be dealing with ASCII mostly (table and column names are
      // rarely localized), we still have to make sure not to overrun the available sapce
      // in the QString, so resize it when needed.
      actualLength += writtenCharCount;
      if(actualLength + 4 >= guessedNewLength) [[unlikely]] {
        target.resize(actualLength + length + 4);
        write = target.data() + actualLength;
      }

    } // while source characters remaining

    target.resize(actualLength);
  }

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
        {
          char8_t *out = &result[outIndex];
          outIndex += Nuclex::Support::Text::UnicodeHelper::WriteCodePoint(out, codePoint);
        }

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

#endif // defined(NUCLEX_THINORM_ENABLE_QT)
