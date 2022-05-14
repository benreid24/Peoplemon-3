target_sources(Core PRIVATE
    AI.cpp
    Cameras.cpp
    Clock.cpp
    Controllable.cpp
    Entity.cpp
    HUD.cpp
    Interaction.cpp
    Movement.cpp
    Player.cpp
    Position.cpp
    Render.cpp
    Scripts.cpp
    Systems.cpp
    Trainers.cpp
    World.cpp
)

add_subdirectory(Cameras)
add_subdirectory(HUD)
