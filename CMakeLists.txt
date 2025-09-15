#!/usr/bin/cmake
cmake_minimum_required (VERSION 3.15)

# -------------------------------------------------------------------------------------------------

project(
  NuclexThinOrmNative
  VERSION 1.0.0
  DESCRIPTION "Thin database abstraction layer for Windows and Linux"
)

option(
  BUILD_DOCS
  "Whether to generate documentation via Doxygen"
  OFF
)

option(
  NUCLEX_THINORM_SUPPORT_ASYNCPP
  "Whether methods should be exposed that use C++20 couroutines for \
  slow operations such as establishing database connections and running \
  queries. This will require the AsyncPP library."
  ON
)

option(
  BUILD_UNIT_TESTS
  "Whether to build the unit test executable. This will require an extra \
  compilation of the entire source tree as well as the GoogleTest library."
  OFF
)
option(
  BUILD_BENCHMARK
  "Whether to build the benchmark executable. This will require an extra \
  compilation of the entire source tree as well as the Celero library."
  OFF
)

# -------------------------------------------------------------------------------------------------

# This sets a bunch of compile flags and defined ${NUCLEX_COMPILER_TAG} to
# say something like linux-gcc-13.2-amd64-debug. You should have this directory
# if you do a full clone of a project that is using this third-party library build.
include("../build-system/cmake/cplusplus.cmake")

# The Unix build pipeline doesn't automatically include threading, so search for
# the pthreads library in order to link against it later on.
#   https://en.wikipedia.org/wiki/Pthreads
find_package(Threads REQUIRED)

# Locate Qt, the cross-platform User Interface and base API abstraction library
# we're using for the built-in SQL database clients
find_package(Qt5 COMPONENTS REQUIRED Sql Core)

# Add Nuclex.Support.Native as a sub-project, we link it for utility methods.
if(NOT (TARGET NuclexSupportNative))
  add_subdirectory(
    ${PROJECT_SOURCE_DIR}/../Nuclex.Support.Native
    ${CMAKE_BINARY_DIR}/NuclexSupportNative
  )
endif()

# Add AsyncPP as a sub-project, we use for the task-based coroutines.
if(NUCLEX_THINORM_SUPPORT_ASYNCPP)
  if(NOT (TARGET AsyncPP))
    add_subdirectory(
      ${PROJECT_SOURCE_DIR}/../third-party/nuclex-asyncpp
      ${CMAKE_BINARY_DIR}/asyncpp
    )
  endif()
endif()

message(STATUS "Enabled options for Nuclex.ThinOrm.Native:")
message(STATUS "  ⚫ Build core library")

if(BUILD_UNIT_TESTS)
  message(STATUS "  ⚫ Build unit tests")

  # Add GoogleTest as a sub-project so we can link our unit test executable
  if(NOT (TARGET GoogleTest))
    add_subdirectory(
      ${PROJECT_SOURCE_DIR}/../third-party/nuclex-googletest
      ${CMAKE_BINARY_DIR}/nuclex-googletest
   )
  endif()
endif()

if(BUILD_BENCHMARK)
  message(STATUS "  ⚫ Build benchmark")

  # Add Celero as a sub-project so we can link our benchmark executable
  if(NOT (TARGET Celero))
    add_subdirectory(
      ${PROJECT_SOURCE_DIR}/../third-party/nuclex-celero
      ${CMAKE_BINARY_DIR}/nuclex-celero
    )
  endif()
endif()

# Use CMake's own package for locating Doxygen on the system
if(BUILD_DOCS)
  find_package(Doxygen)
endif()

# -------------------------------------------------------------------------------------------------

# Project structure
#
#   ProjectName/
#     Source/                   All source files, using deeper directories as needed
#     Include/ProjectName/      All public headers, using deeper directories as needed
#     Tests/                    All unit tests, using deeper directories as needed
#     Benchmarks/               All benchmark files, using deeper directories as needed
#
# CMake documentation:
#   |  Note: We do not recommend using GLOB to collect a list of
#   |  source files from your source tree. If no CMakeLists.txt file
#   |  changes when a source is added or removed then the generated
#   |  build system cannot know when to ask CMake to regenerate.
#
# As so very often, CMake becomes a hurdle rather than helping.
# I'm not going to manually maintain a list of source files. Rebuilds
# where files are added, removed or renamed need to be from scratch.
#
file(
  GLOB_RECURSE sourceFiles
  CONFIGURE_DEPENDS
  "Source/*.cpp"
  "Source/*.c"
)
file(
  GLOB_RECURSE headerFiles
  CONFIGURE_DEPENDS
  "Include/Nuclex/ThinOrm/*.h"
)
file(
  GLOB_RECURSE unittestFiles
  CONFIGURE_DEPENDS
  "Tests/*.cpp"
)
file(
  GLOB_RECURSE benchmarkFiles
  CONFIGURE_DEPENDS
  "Benchmarks/*.cpp"
)

# -------------------------------------------------------------------------------------------------

function(add_third_party_libraries target_name)

  if(NUCLEX_THINORM_SUPPORT_ASYNCPP)
    target_link_libraries(
      ${target_name}
      PUBLIC AsyncPP
    )
  endif()

  target_link_libraries(
    ${target_name}
    PRIVATE Qt5::Sql
    PRIVATE Qt5::Core
  )

  # On Unix systems, the library and unit test executable should look for
  # dependencies in its own directory first.
  set_target_properties(
    ${target_name} PROPERTIES
    BUILD_RPATH_USE_ORIGIN ON
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH_USE_LINK_PATH OFF
    INSTALL_RPATH "\${ORIGIN}"
  )

endfunction()

# -------------------------------------------------------------------------------------------------

# Shared library that can be linked to other projects
add_library(NuclexThinOrmNative SHARED)

# Enable compiler warnings only if this library is compiled on its own.
# If it's used as a sub-project, the including project's developers aren't
# interested in seeing warnings from a project they're not maintaining.
if(${CMAKE_PROJECT_NAME} STREQUAL "NuclexThinOrmNative")
  enable_target_compiler_warnings(NuclexThinOrmNative)
else()
  disable_target_compiler_warnings(NuclexThinOrmNative)
endif()

# Add directory with public headers to include path
target_include_directories(
  NuclexThinOrmNative
  PUBLIC "Include"
)

# Add public headers and sources to compilation list
# (headers, too, in case CMake is used to generate an IDE project)
target_sources(
  NuclexThinOrmNative
  PUBLIC ${headerFiles}
  PRIVATE ${sourceFiles}
)

# Link against PThreads and Nuclex.Support.Native
target_link_libraries(
  NuclexThinOrmNative
  PRIVATE Threads::Threads
  PUBLIC NuclexSupportNative
)

# Add include directories and static libraries of all enabled image formats
add_third_party_libraries(NuclexThinOrmNative)

# On Windows, we want the shared library to be named Nuclex.ThinOrm.Native.dll
if(WIN32)
  set_target_properties(
    NuclexThinOrmNative
    PROPERTIES OUTPUT_NAME "Nuclex.ThinOrm.Native"
  )
endif()

# -------------------------------------------------------------------------------------------------

if(BUILD_UNIT_TESTS)

  # Executable that runs the unit tests (main() supplied by GoogleTest)
  add_executable(NuclexThinOrmNativeTests)

  # Enable compiler warnings only if this library is compiles on its own.
  # If it's used as a sub-project, the including project's developers aren't
  # interested in seeing warnings from a project they're not maintaining.
  if(${CMAKE_PROJECT_NAME} STREQUAL "NuclexThinOrmNative")
    enable_target_compiler_warnings(NuclexThinOrmNativeTests)
  else()
    disable_target_compiler_warnings(NuclexThinOrmNativeTests)
  endif()

  # Let the code know it's not being compiled into a shared library
  # (this disables visibility/exports, thus allowing the compiler detect
  # additional unused code and warn about it)
  target_compile_definitions(
    NuclexThinOrmNativeTests
    PRIVATE NUCLEX_THINORM_EXECUTABLE
  )

  # Add directory with public headers to include path
  target_include_directories(
    NuclexThinOrmNativeTests
    PUBLIC "Include"
  )

  # Add public headers and sources (normal + unit tests) to compilation list
  # (headers, too, in case CMake is used to generate an IDE project)
  target_sources(
    NuclexThinOrmNativeTests
    PRIVATE ${headerFiles}
    PRIVATE ${sourceFiles}
    PRIVATE ${unittestFiles}
  )

  # Link GoogleTest and the main() function supplied by GoogleTest
  # Also link against PThreads
  target_link_libraries(
    NuclexThinOrmNativeTests
    PRIVATE GoogleTest::Static
    PRIVATE GoogleTest::Main
    PRIVATE Threads::Threads
    PUBLIC NuclexSupportNative
  )

  add_third_party_libraries(NuclexThinOrmNativeTests)

  # On Windows, we want the executable to be named Nuclex.ThinOrm.Native.Tests.exe
  if(WIN32)
    set_target_properties(
      NuclexThinOrmNativeTests
      PROPERTIES OUTPUT_NAME "Nuclex.ThinOrm.Native.Tests"
    )
  endif()

endif() # if BUILD_UNIT_TESTS enabled

# -------------------------------------------------------------------------------------------------

if(BUILD_BENCHMARK)

  # Executable that runs the benchmark (main() supplied by Celero)
  add_executable(NuclexThinOrmNativeBenchmark)

  # Enable compiler warnings only if this library is compiled on its own.
  # If it's used as a sub-project, the including project's developers aren't
  # interested in seeing warnings from a project they're not maintaining.
  if(${CMAKE_PROJECT_NAME} STREQUAL "NuclexThinOrmNative")
    enable_target_compiler_warnings(NuclexThinOrmNativeBenchmark)
  else()
    disable_target_compiler_warnings(NuclexThinOrmNativeBenchmark)
  endif()

  # Let the code know it's not being compiled into a shared library
  # (this disables visibility/exports, thus allowing the compiler detect
  # additional unused code and warn about it)
  target_compile_definitions(
    NuclexThinOrmNativeBenchmark
    PRIVATE NUCLEX_THINORM_EXECUTABLE
  )

  # Add directory with public headers to include path
  target_include_directories(
    NuclexThinOrmNativeBenchmark
    PUBLIC "Include"
  )

  # Add public headers and sources (normal + benchmark) to compilation list
  # (headers, too, in case CMake is used to generate an IDE project)
  target_sources(
    NuclexThinOrmNativeBenchmark
    PRIVATE ${headerFiles}
    PRIVATE ${sourceFiles}
    PRIVATE ${benchmarkFiles}
  )

  # Link Celero
  # Also link against PThreads
  target_link_libraries(
    NuclexThinOrmNativeBenchmark
    PRIVATE Celero
    PRIVATE Threads::Threads
    PUBLIC NuclexSupportNative
  )

  add_third_party_libraries(NuclexThinOrmNativeBenchmark)

  # On Windows, we want the executable to be named Nuclex.ThinOrm.Native.Benchmark.exe
  if(WIN32)
    set_target_properties(
      NuclexThinOrmNativeBenchmark
      PROPERTIES OUTPUT_NAME "Nuclex.ThinOrm.Native.Benchmark"
    )
  endif()

endif() # if BUILD_BENCHMARK enabled

# -------------------------------------------------------------------------------------------------

# Install the shared library into a subdirectory of this CMakeLists.txt file
# under ./bin/linux-gcc9.3-amd64-debug/ (the second-level directory is called
# "compiler tag" and dynamically formed -- it ensures that when linking
# a pre-compiled shared library, the correct library is used).
install(
  TARGETS NuclexThinOrmNative
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
)

# Install .pdb files on Windows platforms for the main library.
install_debug_symbols(NuclexThinOrmNative)

# Do the same for Nuclex.Support.Native. Since we depend on this library
# and have set the rpath accordingly, it needs to be in the same directory
install(
  TARGETS NuclexSupportNative
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
)

# Install unit tests in same location as shared library.
if(BUILD_UNIT_TESTS)
  install(
    TARGETS NuclexThinOrmNativeTests
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  )

  # Install .pdb files on Windows platforms for the unit tests, too.
  install_debug_symbols(NuclexThinOrmNativeTests)
endif()

# Install benchmarks in same location as shared library.
if(BUILD_BENCHMARK)
  install(
    TARGETS NuclexThinOrmNativeBenchmark
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${NUCLEX_COMPILER_TAG}
  )

  # Install .pdb files on Windows platforms for the benchmark, too.
  install_debug_symbols(NuclexThinOrmNativeBenchmark)
endif()

# -------------------------------------------------------------------------------------------------

if(BUILD_DOCS)

  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Can't build documentation because Doxygen was not found")
  endif()

  add_custom_target(
    NuclexThinOrmNativeDocs ALL
    COMMAND ${DOXYGEN_EXECUTABLE} "Nuclex.ThinOrm.Native.doxygen.cfg"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  )

endif()

# -------------------------------------------------------------------------------------------------

file(
  WRITE "${PROJECT_SOURCE_DIR}/NuclexThinOrmNativeConfig.cmake"
  "#!/usr/bin/cmake

# Configuration to include Nuclex.ThinOrm.Native in a CMake-based project. If you want to
# reference Nuclex.ThinOrm.Native as an externally compiled static library, do this:
#
#    set(NuclexThinOrmNative_DIR \"../Nuclex.ThinOrm.Native\")
#    find_package(NuclexThinOrmNative REQUIRED CONFIG)
#
#    target_link_libraries(
#        MyAwesomeProject
#        PRIVATE NuclexThinOrmNative::Dynamic
#    )
#
# Alternatively, if you want to build Nuclex.ThinOrm.Native together with your project,
# use the normal CMakeLists.txt with CMake's add_subdirectory() command:
#
#    add_subdirectory(
#        \"\${PROJECT_SOURCE_DIR}/../Nuclex.ThinOrm.Native\"
#        \"\${CMAKE_BINARY_DIR}/nuclex.pixels.native\"
#    )
#
#    target_link_libraries(
#        MyAwesomeProject
#        PRIVATE NuclexThinOrmNative
#    )
#

# -------------------------------------------------------------------------------------------------

if(NOT DEFINED NUCLEX_COMPILER_TAG)
	message(
		FATAL_ERROR
		\"NUCLEX_COMPILER_TAG not defined! Include cplusplus.cmake before importing this package \\
		in order to generate a tag identifying the platform/compiler/architecture/variant!\"
	)
endif()

# -------------------------------------------------------------------------------------------------

if(NOT EXISTS \"\${CMAKE_CURRENT_LIST_DIR}/bin/\${NUCLEX_COMPILER_TAG}\")
	# TODO: Warn and link release build when compiling in debug mode
	# TODO: Warn and link build for older compiler version if found
	message(
		FATAL_ERROR
		\"Directory '\${CMAKE_CURRENT_LIST_DIR}/bin/\${NUCLEX_COMPILER_TAG}' not found. \\
		Please either build and install this project before importing it via \\
		find_package() or use this project's main CMakeFiles.txt via add_subdirectory()!\"
	)
endif()

# -------------------------------------------------------------------------------------------------

add_library(NuclexThinOrmNative::Dynamic SHARED IMPORTED)

# This may cause warnings on recent GCC versions (10.0.0+?) with LTO because GCC detects
# that the headers used during build (residing in build/) are not the same used when
# linking the library (copies resising in Include/).
#
# CMake doesn't run the install step during build, so the only way to get the headers
# in place before building would be by copying them rather than installing them.
set_target_properties(
	NuclexThinOrmNative::Dynamic PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES \"\${CMAKE_CURRENT_LIST_DIR}/Include\"
	IMPORTED_LINK_INTERFACE_LANGUAGES \"C\"
)

if(WIN32)
	set_target_properties(
		NuclexThinOrmNative::Dynamic PROPERTIES
		IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/bin/\${NUCLEX_COMPILER_TAG}/Nuclex.ThinOrm.Native.lib\"
	)
else()
	set_target_properties(
		NuclexThinOrmNative::Dynamic PROPERTIES
		IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/bin/\${NUCLEX_COMPILER_TAG}/libNuclexThinOrmNative.so\"
	)
endif()

message(STATUS \"Imported Nuclex.ThinOrm.Native targets with binaries in '\${CMAKE_CURRENT_LIST_DIR}'\")"
)

# -------------------------------------------------------------------------------------------------
