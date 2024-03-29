#ifndef CORE_GAME_GAME_HPP
#define CORE_GAME_GAME_HPP

#include <Core/Systems/AI.hpp>
#include <Core/Systems/Clock.hpp>
#include <Core/Systems/Controllable.hpp>
#include <Core/Systems/Entity.hpp>
#include <Core/Systems/Flight.hpp>
#include <Core/Systems/HUD.hpp>
#include <Core/Systems/Interaction.hpp>
#include <Core/Systems/Movement.hpp>
#include <Core/Systems/Player.hpp>
#include <Core/Systems/Position.hpp>
#include <Core/Systems/Render.hpp>
#include <Core/Systems/Scripts.hpp>
#include <Core/Systems/Trainers.hpp>
#include <Core/Systems/WildPeoplemon.hpp>
#include <Core/Systems/World.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Util/NonCopyable.hpp>

int main(int, char**);

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
 */
class Systems : private bl::util::NonCopyable {
public:
    /**
     * @brief Const accessor for the Engine
     */
    const bl::engine::Engine& engine() const;

    /**
     * @brief Modifiable accessor for the engine
     */
    bl::engine::Engine& engine();

    /**
     * @brief Accessor for the in game clock
     */
    Clock& clock();

    /**
     * @brief Const accessor for the in game clock
     */
    const Clock& clock() const;

    /**
     * @brief Modifiable accessor for the world system
     */
    World& world();

    /**
     * @brief Const accessor for the world system
     */
    const World& world() const;

    /**
     * @brief Returns a reference to the position system
     */
    Position& position();

    /**
     * @brief Returns a const reference to the position system
     */
    const Position& position() const;

    /**
     * @brief Returns a reference to the movement system
     */
    Movement& movement();

    /**
     * @brief Returns a const reference to the movement system
     */
    const Movement& movement() const;

    /**
     * @brief Returns the entity system
     */
    Entity& entity();

    /**
     * @brief Returns a const reference to the entity system
     */
    const Entity& entity() const;

    /**
     * @brief Returns the player system
     */
    Player& player();

    /**
     * @brief Returns the player system
     */
    const Player& player() const;

    /**
     * @brief Returns the controllable entity system
     */
    Controllable& controllable();

    /**
     * @brief Returns the AI system
     */
    AI& ai();

    /**
     * @brief Returns the interaction system
     */
    Interaction& interaction();

    /**
     * @brief Returns the HUD
     */
    HUD& hud();

    /**
     * @brief Returns the scripting system
     */
    Scripts& scripts();

    /**
     * @brief Returns the scripting system
     */
    const Scripts& scripts() const;

    /**
     * @brief Returns the trainer system
     */
    Trainers& trainers();

    /**
     * @brief Returns the trainer system
     */
    const Trainers& trainers() const;

    /**
     * @brief Returns the wild peoplemon system
     */
    WildPeoplemon& wildPeoplemon();

    /**
     * @brief Returns the wild peoplemon system
     */
    const WildPeoplemon& wildPeoplemon() const;

    /**
     * @brief Returns the flight system
     */
    Flight& flight();

    /**
     * @brief Returns the flight system
     */
    const Flight& flight() const;

    /**
     * @brief Returns the render system
     */
    Render& render();

private:
    bl::engine::Engine& _engine;
    Clock& _clock;
    AI& _ai;
    Controllable _controllable;
    Entity _entity;
    Player& _player;
    World& _world;
    Position& _position;
    Movement& _movement;
    Interaction _interaction;
    HUD& _hud;
    Scripts _scripts;
    Trainers& _trainers;
    WildPeoplemon _wildPeoplemon;
    Flight& _flight;
    Render& _render;

    /**
     * @brief Creates the core game object and associates it with the engine
     *
     * @param engine The main game engine
     */
    Systems(bl::engine::Engine& engine);

    friend int ::main(int, char**);
    friend class ::tests::SystemsHelper;
};

} // namespace system
} // namespace core

#endif
