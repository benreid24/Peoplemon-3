add_executable(Converter main.cpp)

target_link_libraries(Converter PRIVATE
    BLIB
    Core

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

set_target_properties(Converter
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_NAME "Converter"
)
