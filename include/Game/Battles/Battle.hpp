#ifndef GAME_BATTLES_BATTLE_HPP
#define GAME_BATTLES_BATTLE_HPP

#include <Core/Systems/Player.hpp>
#include <Game/Battles/BattleControllers/BattleController.hpp>
#include <Game/Battles/BattleState.hpp>
#include <Game/Battles/BattleView.hpp>
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
    /// Represents the different types of battles. Affects only the intro
    enum struct Type { WildPeoplemon, Trainer, Online };

    /**
     * @brief Creates the battle struct and initializes the player battler
     *
     * @param player The player data
     * @param type The type of battle this is
     * @return std::unique_ptr<Battle> The new battle struct
     */
    static std::unique_ptr<Battle> create(core::system::Player& player, Type type);

    /**
     * @brief Sets and initializes the controller to use in battle
     *
     * @param controller The battle controller to use
     */
    void setController(std::unique_ptr<BattleController>& controller);

    const Type type;
    BattleState state;
    std::unique_ptr<BattleController> controller;
    BattleView view;

private:
    Battle(Type type);
};

} // namespace battle
} // namespace game

#endif
