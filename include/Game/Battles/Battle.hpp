#ifndef GAME_BATTLES_BATTLE_HPP
#define GAME_BATTLES_BATTLE_HPP

#include <Core/Systems/Player.hpp>
#include <Game/Battles/BattleControllers/BattleController.hpp>
#include <Game/Battles/BattleState.hpp>
#include <memory>

namespace game
{
/// Collection of classes used for peoplemon battling
namespace battle
{
/**
 * @brief Convenience struct that owns all the components required for a battle and resolves some
 *        boilerplate setup. This is the top level object
 *
 * @ingroup Battles
 *
 */
struct Battle {
    /**
     * @brief Creates the battle struct and initializes the player battler
     *
     * @param player The player data
     * @return std::unique_ptr<Battle> The new battle struct
     */
    static std::unique_ptr<Battle> create(core::system::Player& player);

    /**
     * @brief Sets and initializes the controller to use in battle
     *
     * @param controller The battle controller to use
     */
    void setController(std::unique_ptr<BattleController>&& controller);

    BattleState state;
    std::unique_ptr<BattleController> controller;
    // TODO - view

private:
    Battle();
};

} // namespace battle
} // namespace game

#endif
