cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(Peoplemon C CXX)

# BLIB will inherit these settings as well
option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)
option(ON_CI "True when on CI, false when local" Off)
option(EDITOR_RELEASE "Builds release mode editor with debug logging" Off)

if (EDITOR_RELEASE)
    set(CMAKE_BUILD_TYPE Release)
    add_compile_definitions(PEOPLEMON_DEBUG)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (ON_CI)
    add_compile_definitions(ON_CI)
endif()

set(CMAKE_INSTALL_PREFIX .)

add_subdirectory(lib/BLIB)
include_directories(lib/BLIB/include)
include_directories(lib/BLIB/lib/SFML/include)

add_executable(Peoplemon)
add_executable(PeoplemonEditor)
add_library(Core STATIC)

include("lib/BLIB/cmake/blib_helpers.cmake")
configure_target(Peoplemon)
configure_target(PeoplemonEditor)
configure_target(Core)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Core PUBLIC PEOPLEMON_DEBUG)
    target_compile_definitions(PeoplemonEditor PUBLIC PEOPLEMON_DEBUG)
    target_compile_definitions(Peoplemon PUBLIC PEOPLEMON_DEBUG)
endif()

target_link_libraries(Core PRIVATE
    BLIB

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

target_link_libraries(Peoplemon PRIVATE
    BLIB
    Core

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

target_link_libraries(PeoplemonEditor PRIVATE
    BLIB
    Core

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

include_directories(include)
add_subdirectory(src)
add_subdirectory(include)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

set_target_properties(Peoplemon
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_NAME "Peoplemon${CMAKE_BUILD_TYPE}"
)

set_target_properties(PeoplemonEditor
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_NAME "PeoplemonEditor${CMAKE_BUILD_TYPE}"
)
