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

#include "Nuclex/ThinOrm/Configuration/KnownOptions.h"

namespace Nuclex::ThinOrm::Configuration {

  // ------------------------------------------------------------------------------------------- //

  const std::u8string KnownOptions::ReadOnlyOptionName = (
    std::u8string(u8"ReadOnly")
  );

  const std::u8string KnownOptions::EnableCompressionOptionName = (
    std::u8string(u8"EnableCompression")
  );

  const std::u8string KnownOptions::ConnectionTimeoutSecondsOptionName = (
    std::u8string(u8"ConnectionTimeoutSeconds")
  );

  const std::u8string KnownOptions::ReadTimeoutSecondsOptionName = (
    std::u8string(u8"ReadTimeoutSeconds")
  );

  const std::u8string KnownOptions::WriteTimeoutSecondsOptionName = (
    std::u8string(u8"WriteTimeoutSeconds")
  );

  const std::u8string KnownOptions::EnableSslOptionName = (
    std::u8string(u8"EnableSsl")
  );

  const std::u8string KnownOptions::RequireSslOptionName = (
    std::u8string(u8"RequireSsl")
  );

  const std::u8string KnownOptions::VerifyServerSslCertificateOptionName = (
    std::u8string(u8"VerifyServerSslCertificate")
  );

  const std::u8string KnownOptions::ClientSslPrivateKeyPathOptionName = (
    std::u8string(u8"ClientSslPrivateKeyPath")
  );

  const std::u8string KnownOptions::ClientSslPublicKeyPathOptionName = (
    std::u8string(u8"ClientSslPublicKeyPath")
  );

  const std::u8string KnownOptions::SslCertificateAuthorityPathOptionName = (
    std::u8string(u8"SslCertificateAuthorityPath")
  );

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
