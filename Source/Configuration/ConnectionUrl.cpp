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

#include "Nuclex/ThinOrm/Configuration/ConnectionUrl.h"
#include "Nuclex/Support/Text/StringHelper.h" // for StringHelper::GetTrimmed()
#include "Nuclex/Support/Text/LexicalCast.h" // lexical_cast<>

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

  /// <summary>Checks whether the specified character is a number</summary>
  /// <param name="character">Character that will be checked for being a number</param>
  /// <returns>True if the specified character was a number</returns>
  bool isNumber(char8_t character) {
    return (
      (character == u8'0') ||
      (character == u8'1') ||
      (character == u8'2') ||
      (character == u8'3') ||
      (character == u8'4') ||
      (character == u8'5') ||
      (character == u8'6') ||
      (character == u8'7') ||
      (character == u8'8') ||
      (character == u8'9')
    );
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

} // anonymous namespace

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  ConnectionUrl::ConnectionUrl() :
    driver(u8"sqlite", 6),
    hostnameOrPath(u8".", 1),
    port(),
    user(),
    password(),
    databaseName(std::u8string(u8"default.sqlite3.db", 18)),
    options() {}

  // ------------------------------------------------------------------------------------------- //

  ConnectionUrl ConnectionUrl::Parse(const std::u8string &properties) {
    ConnectionUrl result;

    // Look for the protocol part in the url:
    //
    //   mariadb://localhost/mydatabase          postgres://user:password@localhost/mydatabase
    //   ^^^^^^^                                 ^^^^^^^^
    //
    // This will become the driver name
    //
    std::u8string::size_type driverEndIndex = properties.find(u8"://");
    if(driverEndIndex == std::u8string::npos) {
      throw std::invalid_argument(reinterpret_cast<const char *>(u8"URL missing protocol part"));
    }

    // Now see if there is a user name and/or password embedded in the url:
    //
    //   mariadb://localhost/mydatabase          postgres://user:password@localhost/mydatabase
    //         <nothing>                                    ^^^^ ^^^^^^^^
    //
    std::u8string::size_type hostStartIndex;
    {
      result.SetDriver(properties.substr(0, driverEndIndex));
      driverEndIndex += 3;

      // See if login parameters (username / password) are contained in the URL
      std::u8string::size_type loginEndIndex = properties.find(u8'@', driverEndIndex);
      if(loginEndIndex == std::u8string::npos) {
        hostStartIndex = driverEndIndex;
      } else { // ^^ no login ^^ // vv login present vv
        std::u8string::size_type userEndIndex = properties.find(u8':', driverEndIndex);
        if((userEndIndex == std::u8string::npos) || (userEndIndex >= loginEndIndex)) {
          result.SetUser(properties.substr(driverEndIndex, loginEndIndex - driverEndIndex));
        } else {
          result.SetUser(properties.substr(driverEndIndex, userEndIndex - driverEndIndex));
          ++userEndIndex;
          result.SetPassword(properties.substr(userEndIndex, loginEndIndex - userEndIndex));
        }

        hostStartIndex = loginEndIndex + 1;
      }
    } // beauty scope

    // Extract the port. We skip over the hostname and stash it in a temporary string here
    // because, depending on whether a port is specified or not, either the hostname is
    // part of the path (file-based database engine) or an actual host name.
    //
    //   mssql://db.local:1433/testinstance/mydatabase    sqlite://mydata.db
    //           '''''''' ^^^^                                <nothing>
    //
    // If there is a port, we stash it and adjust 'hostStartIndex', otherwise, we do
    // absolutely nothing here and let the next section parse the whole URL host + path.
    //
    std::u8string hostnameOrPath;
    std::u8string::size_type optionsStartIndex = properties.find(u8'?', hostStartIndex);
    {
      // Look for a port number. We require it to be before the first slash and to start
      // with a number, otherwise we'll not treat it as a port number at all.
      std::u8string::size_type portIndex = properties.find(u8':', hostStartIndex);
      if(portIndex != std::u8string::npos) {
        ++portIndex;

        if(portIndex < properties.length()) {
          std::u8string::size_type portEndIndex = properties.find(u8'/', hostStartIndex);
          if(portEndIndex == std::u8string::npos) {
            portEndIndex = optionsStartIndex;
          }
          if((portEndIndex == std::u8string::npos) || (portIndex < portEndIndex)) {
            if(isNumber(properties[portIndex])) {
              std::u8string port;
              if(portEndIndex == std::u8string::npos) {
                port = properties.substr(portIndex);
              } else {
                port = properties.substr(portIndex, portEndIndex - portIndex);
              }

              result.SetPort(Nuclex::Support::Text::lexical_cast<std::int16_t>(port));
              hostnameOrPath = properties.substr(hostStartIndex, portIndex - hostStartIndex - 1);
              hostStartIndex = portIndex + port.length();
            } // if first port character is numeric
          } // if port separator lies before first path slash
        } // if characters following port separator
      } // if port separator present
    } // beauty scope

    // Look for the database name. If the URL contains no directory separators, then assume
    // that the URL contains *only* the database name (relative path for file based database),
    // unless a port was specified (dear NTFS peeps, no we don't consider alternate streams).
    //
    //   mssql://db.local:1433/testinstance/mydatabase    mariadb://ambiguousname:3306
    //           """"""""      ^^^^^^^^^^^^                         """""""""""""
    //
    {
      std::string::size_type databaseStartIndex;

      std::u8string::size_type lastSlashIndex = properties.find_last_of(u8'/', optionsStartIndex);
      if((lastSlashIndex != std::u8string::npos) && (lastSlashIndex >= hostStartIndex)) {
        hostnameOrPath.append(properties.substr(hostStartIndex, lastSlashIndex - hostStartIndex));
        result.SetHostnameOrPath(hostnameOrPath);

        hostnameOrPath.clear();
        databaseStartIndex = lastSlashIndex + 1;
      } else { // ^^ slash present ^^ // vv no slash vv
        if(result.port.has_value()) {
          result.SetHostnameOrPath(hostnameOrPath);
          hostnameOrPath.clear();
        } else {
          result.SetHostnameOrPath(std::u8string());
        }
        databaseStartIndex = hostStartIndex;
      }

      if(optionsStartIndex == std::u8string::npos) {
        hostnameOrPath.append(properties.substr(databaseStartIndex));
      } else { // ^^ no options specified ^^ // vv path ends at options vv
        hostnameOrPath.append(
          properties.substr(databaseStartIndex, optionsStartIndex - databaseStartIndex)
        );
      }
      if(hostnameOrPath.empty()) {
        result.SetDatabaseName(std::optional<std::u8string>());
      } else {
        result.SetDatabaseName(hostnameOrPath);
      }
    } // beauty scope

    // If there are URL parameters, scan through the string to isolate each one
    //
    //   mariadb://localhost/db?timeout=30&ssl=no     sqlite://./my.sqlite3.db?journalMode=off
    //                         ^^^^^^^^^^^^^^^^^^                             ^^^^^^^^^^^^^^^^
    //
    // In 'optionsStartIndex' we'll have the index of the question mark that initiates
    // the URL parameter list, if it is present.
    //
    if(optionsStartIndex != std::u8string::npos) {
      std::u8string::size_type start = optionsStartIndex + 1;
      std::u8string::size_type end = properties.find(u8'&', start);
      for(;;) {
        std::u8string_view property = getSubstringView(properties, start, end);

        std::u8string_view::size_type assignmentIndex = property.find(u8'=');
        std::u8string_view keyName = getKeyName(property, assignmentIndex);
        std::u8string_view value = getAssignedValue(property, assignmentIndex);

        std::u8string keyNameString(keyName);
        OptionsMapType::const_iterator iterator = result.options.find(keyNameString);
        if(iterator != result.options.end()) [[unlikely]] {
          std::u8string message(keyName);
          message.append(u8" must not be specified multiple times");
          requireUniqueness(true, message.data());
        }

        result.options[keyNameString] = std::u8string(value);

        // If we already hit the end with this segment, stop
        if(end == std::u8string::npos) {
          break;
        }

        // Look for the next semicolon-delimited key/value pair
        start = end + 1;
        end = properties.find(u8'&', start);
      } // for ever
    } // if URL parameters present

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string ConnectionUrl::ToString() const {
    std::optional<std::u8string> portString;

    // Calculate the length of the final string. This is a bit of micro-optimization
    // but growing a string five times in a row would suck.
    std::u8string::size_type length = (
      this->driver.length() + 3 + this->hostnameOrPath.length()
    );
    if(this->user.has_value() || this->password.has_value()) {
      ++length;
      if(this->user.has_value()) {
        length += this->user.value().length();
      }
      if(this->password.has_value()) {
        length += 1 + this->password.value().length();
      }
    }
    if(this->databaseName.has_value()) {
      if(!this->hostnameOrPath.empty()) {
        ++length;
      }
      length += this->databaseName.value().length();
    }
    if(this->port.has_value()) {
      portString = Nuclex::Support::Text::lexical_cast<std::u8string>(this->port.value());
      length += 2 + portString.value().length();
    }
    for(
      OptionsMapType::const_iterator iterator = this->options.begin();
      iterator != this->options.end();
      ++iterator
    ) {
      length += 1 + iterator->first.length();
      if(!iterator->second.empty()) {
        length += 1 + iterator->second.length();
      }
    }

    // Now that we know the ultimate lenght of the string, begin forming it
    std::u8string result;
    result.reserve(length);

    result.append(this->driver);
    result.append(u8"://");

    if(this->user.has_value() || this->password.has_value()) {
      if(this->user.has_value()) {
        result.append(this->user.value());
      }
      if(this->password.has_value()) {
        result.push_back(u8':');
        result.append(this->password.value());
      }
      result.push_back(u8'@');
    }

    if(portString.has_value()) {
      std::u8string::size_type slashIndex = this->hostnameOrPath.find(u8'/');
      if(slashIndex == std::u8string::npos) {
        result.append(this->hostnameOrPath);
        result.push_back(u8':');
        result.append(portString.value());
      } else {
        result.append(this->hostnameOrPath.substr(0, slashIndex));
        result.push_back(u8':');
        result.append(portString.value());
        result.append(this->hostnameOrPath.substr(slashIndex + 1));
      }
    } else {
      result.append(this->hostnameOrPath);
    }

    if(this->databaseName.has_value()) {
      if(!this->hostnameOrPath.empty()) {
        result.push_back(u8'/');
      }

      result.append(this->databaseName.value());
    }

    bool first = true;
    for(
      OptionsMapType::const_iterator iterator = this->options.begin();
      iterator != this->options.end();
      ++iterator
    ) {
      if(first) {
        result.push_back(u8'?');
        first = false;
      } else {
        result.push_back(u8'&');
      }
      result.append(iterator->first);
      if(!iterator->second.empty()) {
        result.push_back(u8'=');
        result.append(iterator->second);
      }
    }

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::optional<std::u8string> ConnectionUrl::GetOption(
    const std::u8string &name
  ) const {
    std::map<std::u8string, std::u8string>::const_iterator iterator = (
      this->options.find(name)
    );
    if(iterator == this->options.end()) {
      return std::optional<std::u8string>();
    } else {
      return iterator->second;
    }
  }

  // ------------------------------------------------------------------------------------------- //

  void ConnectionUrl::SetOption(
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
