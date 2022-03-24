#ifndef GAME_BATTLES_BATTLER_HPP
#define GAME_BATTLES_BATTLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>
#include <Core/Battles/Commands/TurnAction.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>

namespace core
{
namespace battle
{
class BattleState;
class BattlerController;

/**
 * @brief Base class for battlers in the game. This provides storage for peoplemon and turn choices
 *
 * @ingroup Battles
 *
 */
class Battler {
public:
    /**
     * @brief Creates an uninitialized battler
     *
     */
    Battler(BattleState& state);

    /**
     * @brief Initializes the battler with a team and a controller
     *
     * @param team The team of peoplemon to use
     * @param controller The controller to make decisions
     */
    void init(std::vector<core::pplmn::BattlePeoplemon>&& team,
              std::unique_ptr<BattlerController>&& controller);

    /**
     * @brief Returns whether or not the battler has chosen what to do on this turn
     *
     */
    bool actionSelected() const;

    /**
     * @brief Initiates the process of selecting what to do on this turn
     *
     */
    void pickAction();

    /**
     * @brief Initiates the process of selecting a replacement peoplemon if the current one faints
     *
     */
    void pickPeoplemon();

    /**
     * @brief Returns the action the battler is using this turn
     *
     */
    TurnAction chosenAction() const;

    /**
     * @brief Returns the move the battler is using this turn
     *
     */
    core::pplmn::MoveId chosenMove() const;

    /**
     * @brief Returns the item the battler is using this turn
     *
     */
    core::item::Id chosenItem() const;

    /**
     * @brief Returns the peoplemon the battler is switching to this turn
     *
     */
    std::uint8_t chosenPeoplemon() const;

    /**
     * @brief Returns all the peoplemon held by this battler
     *
     */
    std::vector<core::pplmn::BattlePeoplemon>& peoplemon();

    /**
     * @brief Returns the peoplemon that is currently out
     *
     */
    core::pplmn::BattlePeoplemon& activePeoplemon();

    /**
     * @brief Returns true if the battler has at least one non-fainted peoplemon
     *
     */
    bool canFight() const;

private:
    BattleState& state;
    std::unique_ptr<BattlerController> controller;
    std::vector<core::pplmn::BattlePeoplemon> team;
    std::uint8_t currentPeoplemon;
};

} // namespace battle
} // namespace core

#endif
