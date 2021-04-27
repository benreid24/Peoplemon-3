target_sources(Core PRIVATE
    Properties.cpp
    Resources.cpp
)

add_subdirectory(Entities)
add_subdirectory(Maps)
add_subdirectory(Systems)
