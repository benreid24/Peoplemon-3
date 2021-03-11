cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(ProjectName C CXX)

# BLIB will inherit these settings as well
option(STATIC_LINK "On to link statically" On)
option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

add_subdirectory(lib/BLIB)
include_directories(lib/BLIB/include)
include_directories(lib/BLIB/lib/SFML/include)

add_executable(ProjectName)
set(STATIC_PROJECT_LINK_FLAG "")
if (STATIC_LINK)
    set(STATIC_PROJECT_LINK_FLAG "-static")
endif()

target_link_libraries(ProjectName PRIVATE
    BLIB

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system

    ${STATIC_PROJECT_LINK_FLAG}
)

include_directories(include)
add_subdirectory(src)
add_subdirectory(include)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

set_target_properties(ProjectName
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
