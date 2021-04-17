#ifndef CORE_GAME_GAME_HPP
#define CORE_GAME_GAME_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Util/NonCopyable.hpp>

int main();

/**
 * @addtogroup CoreGame
 * @ingroup Core
 * @brief Collection of core game classes shared between the game and the editor
 *
 */

namespace core
{
/// Namespace containing core game classes shared between the game and the editor
namespace game
{
/**
 * @brief Owns all primary systems and the engine
 *
 * @ingroup CoreGame
 *
 */
class Systems : private bl::util::NonCopyable {
public:
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

    // TODO - make systems accessible here

private:
    bl::engine::Engine& _engine;

    /**
     * @brief Creates the core game object and associates it with the engine
     *
     * @param engine The main game engine
     */
    Systems(bl::engine::Engine& engine);

    friend int ::main();
};

} // namespace game
} // namespace core

#endif
