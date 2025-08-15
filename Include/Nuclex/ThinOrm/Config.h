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

#ifndef NUCLEX_THINORM_CONFIG_H
#define NUCLEX_THINORM_CONFIG_H

// --------------------------------------------------------------------------------------------- //

// ThinOrm recognition
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
  #error The Nuclex.ThinOrm.Native library does not support WinRT
#elif defined(WIN32) || defined(_WIN32)
  #define NUCLEX_THINORM_WINDOWS 1
#else
  #define NUCLEX_THINORM_LINUX 1
#endif

// --------------------------------------------------------------------------------------------- //

// Compiler support checking
#if defined(_MSC_VER)
  #if (_MSC_VER < 1920) // Visual Studio 2019 has the C++20 features we use
    #error At least Visual Studio 2019 is required to compile Nuclex.ThinOrm.Native
  #elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L)
    #define NUCLEX_THINORM_CXX20 1
  #endif
#elif defined(__clang__) && defined(__clang_major__)
  #if (__clang_major__ < 12) // clang 12.0 has the C++20 features we use
    #error At least clang 12.0 is required to compile Nuclex.ThinOrm.Native
  #elif defined(__cplusplus) && (__cplusplus >= 202002L)
    #define NUCLEX_THINORM_CXX20 1
  #endif
#elif defined(__GNUC__)
  #if (__GNUC__ < 11) // GCC 11.0 has the C++20 features we use
    #error At least GCC 11.0 is required to compile Nuclex.ThinOrm.Native
  #elif defined(__cplusplus) && (__cplusplus >= 202002L)
    #define NUCLEX_THINORM_CXX20 1
  #endif
#else
  #error Unknown compiler. Nuclex.ThinOrm.Native is tested with GCC, clang and MSVC only
#endif

// This library uses char8_t, std::u8string and other new C++20 features,
// so anything earlier than C++ 20 would only result in compilation errors.
#if !defined(NUCLEX_THINORM_CXX20)
  #error The Nuclex.ThinOrm.Native library must be compiled in at least C++20 mode
#endif

// --------------------------------------------------------------------------------------------- //

// Endianness detection
#if defined(_MSC_VER) // MSVC is always little endian, including Windows on ARM
  #define NUCLEX_THINORM_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) // GCC
  #define NUCLEX_THINORM_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) // GCC
  #define NUCLEX_THINORM_BIG_ENDIAN 1
#else
  #error Could not determine whether platform is big or little endian
#endif

// --------------------------------------------------------------------------------------------- //

// Decides whether symbols are imported from a dll (client app) or exported to
// a dll (Nuclex.ThinOrm.Native library). The NUCLEX_THINORM_SOURCE symbol is defined by
// all source files of the library, so you don't have to worry about a thing.
#if defined(_MSC_VER)

  #if defined(NUCLEX_THINORM_STATICLIB) || defined(NUCLEX_THINORM_EXECUTABLE)
    #define NUCLEX_THINORM_API
  #else
    #if defined(NUCLEX_THINORM_SOURCE)
      // If we are building the DLL, export the symbols tagged like this
      #define NUCLEX_THINORM_API __declspec(dllexport)
    #else
      // If we are consuming the DLL, import the symbols tagged like this
      #define NUCLEX_THINORM_API __declspec(dllimport)
    #endif
  #endif
  // MSVC doesn't have to import/export the whole type, it includes the vtable
  // and RTTI stuff automatically when at least one member is exported. Exporting
  // the whole type would only cause useless things to be exported as well.
  #define NUCLEX_THINORM_TYPE

#elif defined(__GNUC__) || defined(__clang__)

  #if defined(NUCLEX_THINORM_STATICLIB) || defined(NUCLEX_THINORM_EXECUTABLE)
    #define NUCLEX_THINORM_API
    #define NUCLEX_THINORM_TYPE
  #else
    // If you use -fvisibility=hidden in GCC, exception handling and RTTI would break
    // if visibility wasn't set during export _and_ import because GCC would immediately
    // forget all type infos encountered. See http://gcc.gnu.org/wiki/Visibility
    #define NUCLEX_THINORM_API __attribute__ ((visibility ("default")))
    #define NUCLEX_THINORM_TYPE __attribute__ ((visibility ("default")))
  #endif

#else

  #error Unknown compiler, please implement shared library macros for your system

#endif

// --------------------------------------------------------------------------------------------- //

// Silences unused variable warning, but only in release builds
// (NDEBUG is also what decides whether the assert() macro does anything)
#if defined(NDEBUG)
  #define NUCLEX_THINORM_NDEBUG_UNUSED(x) (void)x
#else
  #define NUCLEX_THINORM_NDEBUG_UNUSED(x) {}
#endif

// --------------------------------------------------------------------------------------------- //

#if defined(_MSC_VER)
  #define NUCLEX_THINORM_CPU_YIELD _mm_pause()
#elif defined(__arm__)
  #define NUCLEX_THINORM_CPU_YIELD asm volatile("yield")
#else
  #define NUCLEX_THINORM_CPU_YIELD __builtin_ia32_pause()
#endif

// --------------------------------------------------------------------------------------------- //

#endif // NUCLEX_THINORM_CONFIG_H
