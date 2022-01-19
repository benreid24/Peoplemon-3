cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(Peoplemon C CXX)

# BLIB will inherit these settings as well
option(STATIC_LINK "On to link statically" On)
option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)
option(ON_CI "True when on CI, false when local" On)
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

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(Core PUBLIC /W4 /WX)
    target_compile_options(Peoplemon PUBLIC /W4 /WX)
    target_compile_options(PeoplemonEditor PUBLIC /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(Core PUBLIC -Wall -Wextra -pedantic)
    target_compile_options(Peoplemon PUBLIC -Wall -Wextra -pedantic)
    target_compile_options(PeoplemonEditor PUBLIC -Wall -Wextra -pedantic)
    target_compile_options(PeoplemonEditor PUBLIC -Wno-switch)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Core PUBLIC PEOPLEMON_DEBUG)
    target_compile_definitions(PeoplemonEditor PUBLIC PEOPLEMON_DEBUG)
    target_compile_definitions(Peoplemon PUBLIC PEOPLEMON_DEBUG)
endif()

if (STATIC_LINK)
    if (WIN32)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set(BUILD_SHARED_LIBRARIES OFF)
        set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    endif()
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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_NAME "Peoplemon${CMAKE_BUILD_TYPE}"
)

set_target_properties(PeoplemonEditor
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_NAME "PeoplemonEditor${CMAKE_BUILD_TYPE}"
)
