add_executable(ProjectName.t
    UnitTest.t.cpp  # You may wish to remove this in favor of directories
)

target_link_libraries(ProjectName.t PRIVATE
    BLIB

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
    
    gtest
    gmock
    gtest_main
)

target_include_directories(ProjectName.t PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(ProjectName.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/BLIB/lib/gtest/googletest/include)
target_include_directories(ProjectName.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/BLIB/lib/gtest/googlemock/include)

add_test(NAME UnitTests COMMAND ProjectName.T)
