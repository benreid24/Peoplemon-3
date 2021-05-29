target_sources(Core PRIVATE
    Properties.cpp
    Resources.cpp
)

add_subdirectory(AI)
add_subdirectory(Components)
add_subdirectory(Files)
add_subdirectory(Items)
add_subdirectory(Maps)
add_subdirectory(Player)
add_subdirectory(Scripts)
add_subdirectory(Systems)
