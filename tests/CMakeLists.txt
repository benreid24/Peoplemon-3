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

add_subdirectory(AI)
add_subdirectory(Files)

add_test(NAME UnitTests COMMAND Peoplemon.T)
