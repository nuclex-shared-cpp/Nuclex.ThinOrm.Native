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

  const std::u8string_view KnownOptions::ReadOnlyOptionName = (
    std::u8string_view(u8"ReadOnly", 8)
  );

  const std::u8string_view KnownOptions::AllowCreateNewOptionName = (
    std::u8string_view(u8"AllowCreateNew", 14)
  );

  const std::u8string_view KnownOptions::EnableCompressionOptionName = (
    std::u8string_view(u8"EnableCompression", 17)
  );

  const std::u8string_view KnownOptions::ConnectionTimeoutSecondsOptionName = (
    std::u8string_view(u8"ConnectionTimeoutSeconds", 24)
  );

  const std::u8string_view KnownOptions::ReadTimeoutSecondsOptionName = (
    std::u8string_view(u8"ReadTimeoutSeconds", 18)
  );

  const std::u8string_view KnownOptions::WriteTimeoutSecondsOptionName = (
    std::u8string_view(u8"WriteTimeoutSeconds", 19)
  );

  const std::u8string_view KnownOptions::EnableSslOptionName = (
    std::u8string_view(u8"EnableSsl", 9)
  );

  const std::u8string_view KnownOptions::RequireSslOptionName = (
    std::u8string_view(u8"RequireSsl", 10)
  );

  const std::u8string_view KnownOptions::VerifyServerSslCertificateOptionName = (
    std::u8string_view(u8"VerifyServerSslCertificate", 26)
  );

  const std::u8string_view KnownOptions::ClientSslPrivateKeyPathOptionName = (
    std::u8string_view(u8"ClientSslPrivateKeyPath", 23)
  );

  const std::u8string_view KnownOptions::ClientSslPublicKeyPathOptionName = (
    std::u8string_view(u8"ClientSslPublicKeyPath", 22)
  );

  const std::u8string_view KnownOptions::SslCertificateAuthorityPathOptionName = (
    std::u8string_view(u8"SslCertificateAuthorityPath", 27)
  );

  // ------------------------------------------------------------------------------------------- //

} // namespace Nuclex::ThinOrm::Configuration
