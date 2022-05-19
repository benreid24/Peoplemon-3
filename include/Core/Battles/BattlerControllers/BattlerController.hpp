#ifndef GAME_BATTLES_BATTLERCONTROLLERS_BATTLERCONTROLLER_HPP
#define GAME_BATTLES_BATTLERCONTROLLERS_BATTLERCONTROLLER_HPP

#include <Core/Battles/TurnAction.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>

namespace core
{
namespace battle
{
class Battler;

/**
 * @brief Base class for battlers in the game. This provides storage for peoplemon and turn choices
 *
 * @ingroup Battles
 *
 */
class BattlerController {
public:
    /**
     * @brief Destroy the Battler Controller object
     *
     */
    virtual ~BattlerController() = default;

    /**
     * @brief Sets the owner of this controller
     *
     * @param owner The battler using this controller
     */
    void setOwner(Battler& owner);

    /**
     * @brief Returns the name of the battler
     *
     */
    virtual const std::string& name() const = 0;

    /**
     * @brief Method that allows controllers to poll status from non-callback based sources
     *
     */
    virtual void refresh() = 0;

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
     * @brief Returns whether or not the player has chosen to continue
     *
     */
    bool shouldContinue() const;

    /**
     * @brief Prompts the player to continue or not
     *
     */
    void chooseToContinue();

    /**
     * @brief Removes the item from the battler's inventory
     *
     * @param item The item to remove
     */
    virtual void removeItem(item::Id item) = 0;

protected:
    Battler* owner;

    /**
     * @brief Initializes the battler with the peoplemon available to it
     *
     * @param peoplemon The peoplemon to use in battle
     */
    BattlerController();

    /**
     * @brief Base classes may override this to perform specific logic to occur when a turn is made
     *
     */
    virtual void startChooseAction() = 0;

    /**
     * @brief Base classes may override this to perform specific logic when a peoplemon must be
     *        picked
     *
     * @param fromFaint True if the current has fainted, false if the switch is for another reason
     * @param reviveOnly True if fainted peoplemon must be selected
     *
     */
    virtual void startChoosePeoplemon(bool fromFaint, bool reviveOnly) = 0;

    /**
     * @brief Prompts the player to continue or not
     *
     */
    virtual void startChooseToContinue() = 0;

    /**
     * @brief Selects the move to use this turn when fighting
     *
     * @param move The index of the move to use
     */
    void chooseMove(int move);

    /**
     * @brief Selects the peoplemon to switch to
     *
     * @param i Index of the peoplemon to switch to
     */
    void choosePeoplemon(std::uint8_t i);

    /**
     * @brief Selects the item to use
     *
     * @param pplIndex Index of the peoplemon to use the item on
     * @param item The item to use
     */
    void chooseItem(std::uint8_t pplIndex, core::item::Id item);

    /**
     * @brief Chooses to run away
     *
     */
    void chooseRun();

    /**
     * @brief Choose whether to give up or not
     *
     * @param yes True to give up, false to continue
     */
    void chooseGiveUp(bool yes);

private:
    TurnAction action;
    union {
        core::item::Id useItem;
        int move;
        bool dontGiveUp;
    };
    std::uint8_t switchIndex;
    bool subActionPicked;
};

} // namespace battle
} // namespace core

#endif
