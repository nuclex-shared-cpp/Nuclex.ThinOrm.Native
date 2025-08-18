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

#include <stdexcept>

namespace {

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

    // Parse the driver name from the URL
    std::u8string::size_type driverEndIndex = properties.find(u8"://");
    if(driverEndIndex == std::u8string::npos) {
      throw std::invalid_argument(reinterpret_cast<const char *>(u8"URL missing protocol part"));
    }

    // Look for the protocol part in the url:
    //
    //   mariadb://localhost/mydatabase
    //   ^^^^^^^
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

    // Extract the port. This is a bit icky because we want to treat everything up to
    // the element behind a potential last slash as the host or path name (so an instance
    // name for SQL server becomes part of the host name and an actual path for SQLite is
    // stored in the path property as well).
    //
    //   mssql://db.local:1433/testinstance/mydatabase    sqlite://mydata.db
    //           '''''''' ^^^^                                <nothing>
    //
    // That's why we'll move the 'hostStartPath' and cache the cut-off host name part in
    // the following variable:
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
    //           ^^^^^^^^      ^^^^^^^^^^^^                         ^^^^^^^^^^^^^
    //
    std::string::size_type databaseStartIndex;
    {
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
        }
        databaseStartIndex = hostStartIndex;
      }
    }

    if(optionsStartIndex == std::u8string::npos) {
      hostnameOrPath.append(properties.substr(databaseStartIndex));
    } else { // ^^ no options specified ^^ // vv path ends at options vv
      hostnameOrPath.append(
        properties.substr(databaseStartIndex, optionsStartIndex - databaseStartIndex)
      );
    }
    result.SetDatabaseName(hostnameOrPath);

    return result;
  }

  // ------------------------------------------------------------------------------------------- //

  std::u8string ConnectionUrl::ToString() const {
    throw std::runtime_error(reinterpret_cast<const char *>(u8"Not implemented yet"));
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
