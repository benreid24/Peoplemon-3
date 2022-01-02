target_sources(Core PUBLIC
    AI.hpp
    Cameras.hpp
    Clock.hpp
    Controllable.hpp
    Entity.hpp
    HUD.hpp
    Interaction.hpp
    Movement.hpp
    Player.hpp
    Position.hpp
    Scripts.hpp
    Systems.hpp
    Render.hpp
    World.hpp
)

add_subdirectory(Cameras)
add_subdirectory(HUD)
