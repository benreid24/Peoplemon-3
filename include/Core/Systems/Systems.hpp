#ifndef CORE_GAME_GAME_HPP
#define CORE_GAME_GAME_HPP

#include <Core/Systems/AI.hpp>
#include <Core/Systems/Cameras.hpp>
#include <Core/Systems/Clock.hpp>
#include <Core/Systems/Controllable.hpp>
#include <Core/Systems/Entity.hpp>
#include <Core/Systems/HUD.hpp>
#include <Core/Systems/Interaction.hpp>
#include <Core/Systems/Movement.hpp>
#include <Core/Systems/Player.hpp>
#include <Core/Systems/Position.hpp>
#include <Core/Systems/Render.hpp>
#include <Core/Systems/World.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Util/NonCopyable.hpp>

int main();

namespace tests
{
class SystemsHelper;
}

/**
 * @addtogroup Systems
 * @ingroup Core
 * @brief Collection of core game systems shared between the game and the editor
 *
 */

namespace core
{
/// Namespace containing core game systems shared between the game and the editor
namespace system
{
/**
 * @brief Owns all primary systems and a reference to the engine
 *
 * @ingroup Systems
 *
 */
class Systems : private bl::util::NonCopyable {
public:
    /**
     * @brief Helper function to update all game systems
     *
     * @param dt Elapsed time in seconds since last call to update()
     */
    void update(float dt);

    /**
     * @brief Const accessor for the Engine
     *
     */
    const bl::engine::Engine& engine() const;

    /**
     * @brief Modifiable accessor for the engine
     *
     */
    bl::engine::Engine& engine();

    /**
     * @brief Const accessor for the in game clock
     *
     */
    const Clock& clock() const;

    /**
     * @brief Modifiable accessor for the world system
     *
     */
    World& world();

    /**
     * @brief Const accessor for the world system
     *
     */
    const World& world() const;

    /**
     * @brief Returns a modifiable accessor for the camera system
     *
     */
    Cameras& cameras();

    /**
     * @brief Returns an immutable accessor for the camera system
     *
     */
    const Cameras& cameras() const;

    /**
     * @brief Returns a reference to the position system
     *
     */
    Position& position();

    /**
     * @brief Returns a const reference to the position system
     *
     */
    const Position& position() const;

    /**
     * @brief Returns a reference to the movement system
     *
     */
    Movement& movement();

    /**
     * @brief Returns a const reference to the movement system
     *
     */
    const Movement& movement() const;

    /**
     * @brief Returns a reference to the rendering system
     *
     */
    Render& render();

    /**
     * @brief Returns a const reference to the rendering system
     *
     */
    const Render& render() const;

    /**
     * @brief Returns the entity system
     *
     */
    Entity& entity();

    /**
     * @brief Returns a const reference to the entity system
     *
     */
    const Entity& entity() const;

    /**
     * @brief Returns the player system
     *
     */
    Player& player();

    /**
     * @brief Returns the player system
     *
     */
    const Player& player() const;

    /**
     * @brief Returns the controllable entity system
     *
     */
    Controllable& controllable();

    /**
     * @brief Returns the AI system
     *
     */
    AI& ai();

    /**
     * @brief Returns the interaction system
     *
     */
    Interaction& interaction();

    /**
     * @brief Returns the HUD
     *
     */
    HUD& hud();

private:
    bl::engine::Engine& _engine;
    Cameras _cameras;
    Clock _clock;
    AI _ai;
    Controllable _controllable;
    Entity _entity;
    Player _player;
    World _world;
    Position _position;
    Movement _movement;
    Render _render;
    Interaction _interaction;
    HUD _hud;

    /**
     * @brief Creates the core game object and associates it with the engine
     *
     * @param engine The main game engine
     */
    Systems(bl::engine::Engine& engine);

    friend int ::main();
    friend class ::tests::SystemsHelper;
};

} // namespace system
} // namespace core

#endif
