#ifndef GAME_BATTLES_BATTLER_HPP
#define GAME_BATTLES_BATTLER_HPP

#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Game/Battles/TurnAction.hpp>

namespace game
{
namespace battle
{
/**
 * @brief Base class for battlers in the game. This provides storage for peoplemon and turn choices
 *
 * @ingroup Battles
 *
 */
class Battler {
public:
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
    unsigned int chosenPeoplemon() const;

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

protected:
    /**
     * @brief Initializes the battler with the peoplemon available to it
     *
     * @param peoplemon The peoplemon to use in battle
     */
    Battler(std::vector<core::pplmn::OwnedPeoplemon>& peoplemon);

    /**
     * @brief Base classes may override this to perform specific logic to occur when a turn is made
     *
     */
    virtual void startChooseAction() = 0;

    /**
     * @brief Base classes may override this to perform specific logic when a peoplemon must be
     *        picked
     *
     */
    virtual void startChoosePeoplemon() = 0;

    /**
     * @brief Selects the action to take on this turn
     *
     * @param action The action to store and report to the battle
     */
    void chooseAction(TurnAction action);

    /**
     * @brief Selects the move to use this turn when fighting
     *
     * @param move The move to use
     */
    void chooseMove(core::pplmn::MoveId move);

    /**
     * @brief Selects the peoplemon to switch to
     *
     * @param i Index of the peoplemon to switch to
     */
    void choosePeoplemon(unsigned int i);

    /**
     * @brief Selects the item to use
     *
     * @param item The item to use
     */
    void chooseItem(core::item::Id item);

private:
    std::vector<core::pplmn::BattlePeoplemon> team;
    unsigned int currentPeoplemon;

    TurnAction action;
    core::item::Id useItem;
    unsigned int switchIndex;
    bool actionChoosed;
    bool subActionPicked;
};

} // namespace battle
} // namespace game

#endif
