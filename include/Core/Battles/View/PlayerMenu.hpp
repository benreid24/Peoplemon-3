#ifndef CORE_BATTLES_VIEW_PLAYERMENU_HPP
#define CORE_BATTLES_VIEW_PLAYERMENU_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Battles/Battler.hpp>
#include <Core/Battles/TurnAction.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
namespace view
{
/**
 * @brief The player menu for battles
 *
 * @ingroup Battles
 *
 */
class PlayerMenu {
public:
    /**
     * @brief Construct a new Player Menu
     *
     * @param canRun Whether or not to allow running
     * @param eventBus The event bus to use
     *
     */
    PlayerMenu(bool canRun, bl::event::Dispatcher& eventBus);

    /**
     * @brief Polls state from opened peoplemon or bag menu
     *
     */
    void refresh();

    /**
     * @brief Updates the menu for the given peoplemon
     *
     * @param i The index of the peoplemon in the team
     * @param ppl The peoplemon itself
     *
     */
    void setPeoplemon(int i, const pplmn::BattlePeoplemon& ppl);

    /**
     * @brief Resets the menu to the turn start state
     *
     */
    void beginTurn();

    /**
     * @brief Opens the menu to select a peoplemon to send out
     *
     * @param fromFaint True if the current peoplemon has fainted
     * @param fromRevive True if the eligible peoplemon must be fainted
     *
     */
    void choosePeoplemonMidTurn(bool fromFaint, bool fromRevive);

    /**
     * @brief Initiates the process of choosing the move to forget
     *
     */
    void chooseMoveToForget();

    /**
     * @brief Returns true when the player's choice has been made
     *
     */
    bool ready() const;

    /**
     * @brief The selected turn action
     *
     */
    TurnAction selectedAction() const;

    /**
     * @brief Returns the selected move index
     *
     */
    int selectedMove() const;

    /**
     * @brief Returns the selected item
     *
     */
    item::Id selectedItem() const;

    /**
     * @brief Returns the selected peoplemon index
     *
     */
    int selectedPeoplemon() const;

    /**
     * @brief Processes player input to update the menu state
     *
     */
    void handleInput(component::Command input);

    /**
     * @brief Renders the menu to the given target
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    enum struct State {
        Hidden,
        PickingAction,
        PickingMove,
        PickingItem,
        PickingPeoplemon,
        ChoosingMoveToForget
    };

    State state;
    bool stateLoopGuard;
    bl::event::Dispatcher& eventBus;
    player::input::MenuDriver menuDriver;
    TurnAction chosenAction;
    union {
        int chosenMoveOrPeoplemon;
        item::Id chosenItem;
    };
    int currentPeoplemon;

    bl::menu::Menu actionMenu;
    bl::menu::TextItem::Ptr fightItem;
    bl::menu::TextItem::Ptr switchItem;
    bl::menu::TextItem::Ptr bagItem;
    bl::menu::TextItem::Ptr runItem;

    bl::menu::Menu moveMenu;
    bl::menu::TextItem::Ptr moveItems[4];
    const pplmn::OwnedMove* moves;

    bl::resource::Resource<sf::Texture>::Ref moveTxtr;
    sf::Sprite moveBox;
    sf::Text movePP;
    sf::Text movePwr;
    sf::Text moveAcc;

    void fightChosen();
    void switchChosen();
    void itemChosen();
    void runChosen();
    void moveChosen(int i);
    void syncMove(int i);
};

} // namespace view
} // namespace battle
} // namespace core

#endif
