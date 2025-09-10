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

#include "Nuclex/ThinOrm/Configuration/ConnectionString.h"
#include "Nuclex/Support/Text/StringHelper.h" // for StringHelper::GetTrimmed()
#include "Nuclex/Support/Text/LexicalCast.h" // lexical_cast<>
#include "Nuclex/Support/Text/UnicodeHelper.h" // for UnicodeHelper::Read/WriteCodePoint()

#include <stdexcept> // for std::invalid_argument
#include <cassert> // for assert()

namespace {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Constructs a string_view from a string segment specified via indices</summary>
  /// <param name="text">Text in which a string_view for a substring will be created</param>
  /// <param name="start">Start index within the full string</param>
  /// <param name="count">End index within the full string or 'npos' for everything</param>
  /// <returns>
  ///   A string_view covering the region of the string like std::u8string::substr() would
  /// </returns>
  std::u8string_view getSubstringView(
    const std::u8string &text,
    std::u8string::size_type start, std::u8string::size_type count = std::u8string::npos
  ) {
    if(count == std::u8string::npos) {
      std::u8string::size_type length = text.length();
      assert((length >= start) && u8"Start index lies within the string");

      return std::u8string_view(text.data() + start, length - start);
    } else {
      return std::u8string_view(text.data() + start, count - start);
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Extracts the key name of a key/value assignment (with optional value)</summary>
  /// <param name="property">String segment that contains the name of the key</param>
  /// <param name="assignmentIndex">Index of the equals sign in the assignment</param>
  /// <returns>A string segment containing only the trimmed key name</returns>
  std::u8string_view getKeyName(
    const std::u8string_view &property,
    std::u8string_view::size_type assignmentIndex = std::u8string_view::npos
  ) {
    if(assignmentIndex == std::u8string_view::npos) {
      return Nuclex::Support::Text::StringHelper::GetTrimmed(property);
    } else {
      return Nuclex::Support::Text::StringHelper::GetTrimmed(
        property.substr(0, assignmentIndex)
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Extracts the value of a key/value assignment</summary>
  /// <param name="property">String segment that contains the assigned value</param>
  /// <param name="assignmentIndex">Index of the equals sign in the assignment</param>
  /// <returns>A string segment containing only the trimmed value being assigned</returns>
  std::u8string_view getAssignedValue(
    const std::u8string_view &property,
    std::u8string_view::size_type assignmentIndex = std::u8string_view::npos
  ) {
    if(assignmentIndex == std::u8string_view::npos) {
      return std::u8string_view();
    } else {
      return Nuclex::Support::Text::StringHelper::GetTrimmed(
        property.substr(assignmentIndex + 1)
      );
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Checks if a key matches the expected value</summary>
  /// <param name="key">Key that will be compared</param>
  /// <param name="expected">Value against which the key will be compared</param>
  /// <returns>True if the key matches the expected value</returns>
  bool keyEquals(const std::u8string_view &key, const std::u8string &expected) {
    std::u8string_view::size_type keyLength = key.length();
    std::u8string::size_type expectedLength = expected.length();
    if(keyLength != expectedLength) [[likely]] {
      return false;
    }

    const char8_t *keyStart = key.data();
    const char8_t *keyEnd = keyStart + keyLength;
    const char8_t *expectedStart = expected.data();
    const char8_t *expectedEnd = expectedStart + expectedLength;

    for(;;) {
      using Nuclex::Support::Text::UnicodeHelper;

      if(keyStart >= keyEnd) [[unlikely]] {
        return (expectedStart>= expectedEnd); // Both must end at the same time
      }
      if(expectedStart >= expectedEnd) [[unlikely]] {
        return false; // If the haystack was shorter, the needle wasn't found
      }

      // Fetch the next code points from both strings so we can compare them
      char32_t keyCodePoint = UnicodeHelper::ReadCodePoint(keyStart, keyEnd);
      if(!UnicodeHelper::IsValidCodePoint(keyCodePoint)) [[unlikely]] {
        throw std::invalid_argument(reinterpret_cast<const char *>(u8"Corrupt UTF-8 string"));
      }
      char32_t expectedCodePoint = UnicodeHelper::ReadCodePoint(expectedStart, expectedEnd);
      if(!UnicodeHelper::IsValidCodePoint(expectedCodePoint)) [[unlikely]] {
        throw std::invalid_argument(reinterpret_cast<const char *>(u8"Corrupt UTF-8 string"));
      }

      // Convert them to folded lowercase for a case insensitive comparison
      expectedCodePoint = UnicodeHelper::ToFoldedLowercase(expectedCodePoint);
      keyCodePoint = UnicodeHelper::ToFoldedLowercase(keyCodePoint);
      if(expectedCodePoint != keyCodePoint) [[likely]] {
        return false;
      }
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Throws an exception if a key is a duplicate</summary>
  /// <param name="isDuplicate">True if the key is a duplicate</param>
  /// <param name="exceptionMessage">Exception that will be thrown</param>
  void requireUniqueness(bool isDuplicate, const char8_t *exceptionMessage) {
    if(isDuplicate) [[unlikely]] {
      throw std::invalid_argument(reinterpret_cast<const char *>(exceptionMessage));
    }
  }

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Throws an exception if a key is missing</summary>
  /// <param name="isPresent">True if the key is present</param>
  /// <param name="exceptionMessage">Exception that will be thrown</param>
  void requirePresence(bool isPresent, const char8_t *exceptionMessage) {
    if(!isPresent) [[unlikely]] {
      throw std::invalid_argument(reinterpret_cast<const char *>(exceptionMessage));
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // anonymous namespace

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  const std::u8string ConnectionString::DriverPropertyName(u8"Driver", 6);

  const std::u8string ConnectionString::HostPropertyName(u8"Host", 4);

  const std::u8string ConnectionString::PathPropertyName(u8"Path", 4);

  const std::u8string ConnectionString::UserPropertName(u8"User", 4);

  const std::u8string ConnectionString::PasswordPropertName(u8"Password", 8);

  const std::u8string ConnectionString::PortPropertyName(u8"Port", 4);

  const std::u8string ConnectionString::DatabasePropertName(u8"Database", 8);

  // ------------------------------------------------------------------------------------------- //

  ConnectionString::ConnectionString() :
    driver(u8"sqlite", 6),
    hostnameOrPath(u8".", 1),
    port(),
    user(),
    password(),
    databaseName(std::u8string(u8"default.sqlite3.db", 18)),
    options() {}

  // ------------------------------------------------------------------------------------------- //

  ConnectionString::ConnectionString(const ConnectionProperties& other) :
    driver(other.GetDriver()),
    hostnameOrPath(other.GetHostnameOrPath()),
    port(other.GetPort()),
    user(other.GetUser()),
    password(other.GetPassword()),
    databaseName(other.GetDatabaseName()),
    options() {

    std::vector<std::u8string> names = other.ListOptions();
    for(const std::u8string &name : names) {
      this->options[name] = other.GetOption(name).value(); // safe, we query only existing ones
    }
  }

  // ------------------------------------------------------------------------------------------- //

  ConnectionString ConnectionString::Parse(const std::u8string &properties) {
    ConnectionString result;

    // To keep track of non std::optional<> properties so we can tell if they're missing
    bool haveDriver = false;
    bool haveHostnameOrPath = false;
    bool haveDatabaseName = false;

    // Scan through the string to locate semicolon-delimited key/value pairs
    std::u8string::size_type start = 0;
    std::u8string::size_type end = properties.find(u8';');
    for(;;) {
      std::u8string_view property = getSubstringView(properties, start, end);

      if(!property.empty()) {
        std::u8string_view::size_type assignmentIndex = property.find(u8'=');
        std::u8string_view keyName = getKeyName(property, assignmentIndex);
        std::u8string_view value = getAssignedValue(property, assignmentIndex);

        using Nuclex::Support::Text::StringMatcher;
        using Nuclex::Support::Text::lexical_cast;
        if(keyEquals(keyName, DriverPropertyName)) {
          requireUniqueness(haveDriver, u8"Driver must not be specified multiple times");
          result.SetDriver(std::u8string(value));
          haveDriver = true;
        } else if(keyEquals(keyName, HostPropertyName) || keyEquals(keyName, PathPropertyName)) {
          requireUniqueness(
            haveHostnameOrPath, u8"Host or path must not be specified multiple times"
          );
          result.SetHostnameOrPath(std::u8string(value));
          haveHostnameOrPath = true;
        } else if(keyEquals(keyName, PortPropertyName)) {
          requireUniqueness(
            result.port.has_value(), u8"Port must not be specified multiple times"
          );
          result.SetPort(lexical_cast<std::uint16_t>(std::u8string(value)));
        } else if(keyEquals(keyName, UserPropertName)) {
          requireUniqueness(
            result.user.has_value(), u8"User must not be specified multiple times"
          );
          result.SetUser(std::u8string(value));
        } else if(keyEquals(keyName, PasswordPropertName)) {
          requireUniqueness(
            result.password.has_value(), u8"Password must not be specified multiple times"
          );
          result.SetPassword(std::u8string(value));
        } else if(keyEquals(keyName, PasswordPropertName)) {
          requireUniqueness(
            haveDatabaseName, u8"Database/file name must not be specified multiple times"
          );
          result.SetDatabaseName(std::u8string(value));
          haveDatabaseName = true;
        } else {
          std::u8string keyNameString(keyName);
          OptionsMapType::const_iterator iterator = result.options.find(keyNameString);
          if(iterator != result.options.end()) [[unlikely]] {
            std::u8string message(keyName);
            message.append(u8" must not be specified multiple times");
            requireUniqueness(true, message.data());
          }

          result.options[keyNameString] = std::u8string(value);
        }
      } // if property string has contents

      // If we already hit the end with this segment, stop
      if(end == std::u8string::npos) {
        break;
      }

      // Look for the next semicolon-delimited key/value pair
      start = end + 1;
      end = properties.find(u8';', start);
    } // for ever

    // Require at least the driver name (without it no connection can be made) and either
    // a lone hostname (valid if the connection is to the database engine itself) or
    // both or a database name only (for file-based databases in the current directory).
    requirePresence(haveDriver, u8"Driver must be specified");
    requirePresence(
      haveHostnameOrPath || haveDatabaseName,
      u8"Hostname/path or database name (or both) must be specified"
    );

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string ConnectionString::ToString() const {
    std::optional<std::u8string> portString;

    // Calculate the length of the final string. This is a bit of micro-optimization
    // but growing a string five times in a row would suck.
    std::u8string::size_type length = (
      DriverPropertyName.length() + 1 + this->driver.length() +
      2 + HostPropertyName.length() + 1 + this->hostnameOrPath.length()
    );
    if(this->port.has_value()) {
      portString = Nuclex::Support::Text::lexical_cast<std::u8string>(this->port.value());
      length += 2 + PortPropertyName.length() + 1 + portString.value().length();
    }
    if(this->user.has_value()) {
      length += 2 + UserPropertName.length() + 1 + this->user.value().length();
    }
    if(this->password.has_value()) {
      length += 2 + PasswordPropertName.length() + 1 + this->password.value().length();
    }
    if(this->databaseName.has_value()) {
      length += 2 + DatabasePropertName.length() + 1 + this->databaseName.value().length();
    }
    for(
      OptionsMapType::const_iterator iterator = this->options.begin();
      iterator != this->options.end();
      ++iterator
    ) {
      length += 2 + iterator->first.length();
      if(!iterator->second.empty()) {
        length += 1 + iterator->second.length();
      }
    }

    // Now that we know the ultimate lenght of the string, begin forming it
    std::u8string result;
    result.reserve(length);

    // Driver
    result.append(DriverPropertyName);
    result.push_back(u8'=');
    result.append(this->driver);

    // Hostname or path
    result.append(u8"; ");
    result.append(HostPropertyName);
    result.push_back(u8'=');
    result.append(this->hostnameOrPath);

    if(portString.has_value()) {
      result.append(u8"; ");
      result.append(PortPropertyName);
      result.push_back(u8'=');
      result.append(portString.value());
    }
    if(this->user.has_value()) {
      result.append(u8"; ");
      result.append(UserPropertName);
      result.push_back(u8'=');
      result.append(this->user.value());
    }
    if(this->password.has_value()) {
      result.append(u8"; ");
      result.append(PasswordPropertName);
      result.push_back(u8'=');
      result.append(this->password.value());
    }
    if(this->databaseName.has_value()) {
      result.append(u8"; ");
      result.append(DatabasePropertName);
      result.push_back(u8'=');
      result.append(this->databaseName.value());
    }
    for(
      OptionsMapType::const_iterator iterator = this->options.begin();
      iterator != this->options.end();
      ++iterator
    ) {
      result.append(u8"; ");
      result.append(iterator->first);
      if(!iterator->second.empty()) {
        result.push_back(u8'=');
        result.append(iterator->second);
      }
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> ConnectionString::GetOption(
    const std::u8string &name
  ) const {
    OptionsMapType::const_iterator iterator = this->options.find(name);
    if(iterator == this->options.end()) {
      return std::optional<std::u8string>();
    } else {
      return iterator->second;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void ConnectionString::SetOption(
    const std::u8string &name,
    const std::optional<std::u8string> &value /* = std::optional<std::u8string>() */
  ) {
    if(value.has_value()) {
      this->options[name] = value.value();
    } else {
      this->options.erase(name);
    }
  }

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
