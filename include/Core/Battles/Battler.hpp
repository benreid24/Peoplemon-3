#ifndef GAME_BATTLES_BATTLER_HPP
#define GAME_BATTLES_BATTLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>
#include <Core/Battles/BattlerSubstate.hpp>
#include <Core/Battles/TurnAction.hpp>
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
     * @brief Calls refresh on the controller. Should be called every update
     *
     */
    void refresh();

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
     * @param fromFaint True if the current has fainted, false if the switch is for another reason
     * @param reviveOnly True if fainted peoplemon must be selected
     *
     */
    void pickPeoplemon(bool fromFaint, bool reviveOnly);

    /**
     * @brief Returns the action the battler is using this turn
     *
     */
    TurnAction chosenAction() const;

    /**
     * @brief Returns the move the battler is using this turn
     *
     */
    int chosenMove() const;

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
     * @brief Returns the index of the peoplemon that is out now
     *
     */
    unsigned int outNowIndex() const;

    /**
     * @brief Returns true if the battler has at least one non-fainted peoplemon
     *
     */
    bool canFight() const;

    /**
     * @brief Returns whether or not the battler has another peoplemon besides the active one
     *
     */
    bool canSwitch() const;

    /**
     * @brief Returns the name of the battler
     *
     */
    const std::string& name() const;

    /**
     * @brief Returns the priority of the battler's action. Fight returns the move priority and
     *        other choices return 1000. Player goes first in case of tie
     *
     * @return unsigned int The priority of the battler's turn
     */
    unsigned int getPriority() const;

    /**
     * @brief Returns the state of this battler
     *
     */
    BattlerSubstate& getSubstate();

    /**
     * @brief Notifies the battler that a turn has began
     *
     */
    void notifyTurnBegin();

    /**
     * @brief Notifies the battler that a turn has ended
     *
     */
    void notifyTurnEnd();

    /**
     * @brief Notifies the battler that a switch occured for them
     *
     * @param fromFaint True for if the switch is from a faint, false otherwise
     *
     */
    void notifySwitch(bool fromFaint);

private:
    BattleState& state;
    BattlerSubstate substate;
    std::unique_ptr<BattlerController> controller;
    std::vector<core::pplmn::BattlePeoplemon> team;
    std::uint8_t currentPeoplemon;
};

} // namespace battle
} // namespace core

#endif
