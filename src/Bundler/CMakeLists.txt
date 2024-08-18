target_sources(Bundler PRIVATE main.cpp)

target_link_libraries(Bundler PRIVATE
    BLIB
    Core

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

set_target_properties(Bundler
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_NAME "Bundler"
)
