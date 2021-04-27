#ifndef CORE_GAME_GAME_HPP
#define CORE_GAME_GAME_HPP

#include <Core/Systems/Clock.hpp>
#include <Core/Systems/World.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Util/NonCopyable.hpp>

int main();

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

private:
    bl::engine::Engine& _engine;
    Clock _clock;
    World _world;

    /**
     * @brief Creates the core game object and associates it with the engine
     *
     * @param engine The main game engine
     */
    Systems(bl::engine::Engine& engine);

    friend int ::main();
};

} // namespace system
} // namespace core

#endif
