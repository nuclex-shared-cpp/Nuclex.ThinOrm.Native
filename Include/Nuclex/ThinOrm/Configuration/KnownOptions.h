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

#ifndef NUCLEX_THINORM_CONFIGURATION_KNOWNOPTIONS_H
#define NUCLEX_THINORM_CONFIGURATION_KNOWNOPTIONS_H

#include "Nuclex/ThinOrm/Config.h"

#include <string> // for std::u8string

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  /// <summary>Standard options that database drivers use for matching features</summary>
  class NUCLEX_THINORM_TYPE KnownOptions {

    /// <summary>If supported, connects to the database in read-only mode</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view ReadOnlyOptionName;

    /// <summary>For file-backed databases, whether a new database file can be created</summary>
    /// <remarks>
    ///   The default value for this is 'true', meaning that file-backed databases, such
    ///   as SQLite, will create new database file if the specified path does not exist.
    /// </remarks>
    public: NUCLEX_THINORM_API static const std::u8string_view AllowCreateNewOptionName;

    /// <summary>If supported, enables compression of queried results</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view EnableCompressionOptionName;

    /// <summary>Timeout after which a connection attempt should fail byt itself</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view ConnectionTimeoutSecondsOptionName;

    /// <summary>Timeout after which a query should fail if no result is sent</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view ReadTimeoutSecondsOptionName;

    /// <summary>Timeout after which a query should fail if rows remain unconmfirmed</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view WriteTimeoutSecondsOptionName;

    /// <summary>Whether to enable SSL encryption if available and compatible</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view EnableSslOptionName;

    /// <summary>Whether to fail a connection attempt if SSL encryption is unavailable</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view RequireSslOptionName;

    /// <summary>Whether validity of the server-provided SSL certificate is checked</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view VerifyServerSslCertificateOptionName;

    /// <summary>Path to the private key of the client SSL certificate</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view ClientSslPrivateKeyPathOptionName;

    /// <summary>Path to the public key of the client SSL certificate</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view ClientSslPublicKeyPathOptionName;

    /// <summary>Path to the certificate authority certificate the use for verification</summary>
    public: NUCLEX_THINORM_API static const std::u8string_view SslCertificateAuthorityPathOptionName;

  };

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration

#endif // NUCLEX_THINORM_CONFIGURATION_KNOWNOPTIONS_H


