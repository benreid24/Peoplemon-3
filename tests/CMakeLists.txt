add_executable(Peoplemon.t)

target_link_libraries(Peoplemon.t PRIVATE
    BLIB
    Core

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system

    gtest
    gmock
    gtest_main
)

target_include_directories(Peoplemon.t PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(Peoplemon.t PRIVATE ${PROJECT_SOURCE_DIR}/tests)
target_include_directories(Peoplemon.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/BLIB/lib/gtest/googletest/include)
target_include_directories(Peoplemon.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/BLIB/lib/gtest/googlemock/include)

add_subdirectory(Files)

set_target_properties(Peoplemon.t
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME UnitTests COMMAND Peoplemon.T)
