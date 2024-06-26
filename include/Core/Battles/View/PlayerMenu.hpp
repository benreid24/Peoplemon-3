#ifndef CORE_BATTLES_VIEW_PLAYERMENU_HPP
#define CORE_BATTLES_VIEW_PLAYERMENU_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Battles/Battler.hpp>
#include <Core/Battles/TurnAction.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>

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
     * @param engine The game engine instance
     * @param canRun Whether or not to allow running
     */
    PlayerMenu(bl::engine::Engine& engine, bool canRun);

    /**
     * @brief Creates UI elements and adds them to the scene
     *
     * @param scene The scene to use
     */
    void init(bl::rc::scene::CodeScene* scene);

    /**
     * @brief Polls state from opened peoplemon or bag menu
     */
    void refresh();

    /**
     * @brief Updates the menu for the given peoplemon
     *
     * @param i The index of the peoplemon in the team
     * @param ppl The peoplemon itself
     */
    void setPeoplemon(int i, const pplmn::BattlePeoplemon& ppl);

    /**
     * @brief Resets the menu to the turn start state
     */
    void beginTurn();

    /**
     * @brief Opens the menu to select a peoplemon to send out
     *
     * @param fromFaint True if the current peoplemon has fainted
     * @param fromRevive True if the eligible peoplemon must be fainted
     */
    void choosePeoplemonMidTurn(bool fromFaint, bool fromRevive);

    /**
     * @brief Initiates the process of choosing the move to forget
     */
    void chooseMoveToForget();

    /**
     * @brief Returns true when the player's choice has been made
     */
    bool ready() const;

    /**
     * @brief The selected turn action
     */
    TurnAction selectedAction() const;

    /**
     * @brief Returns the selected move index
     */
    int selectedMove() const;

    /**
     * @brief Returns the selected item
     */
    item::Id selectedItem() const;

    /**
     * @brief Returns the selected peoplemon index
     */
    int selectedPeoplemon() const;

    /**
     * @brief Processes player input to update the menu state
     *
     * @param input The control to handle
     * @param ignoreDebounce True to process always, false to rate limit
     */
    void handleInput(input::EntityControl input, bool ignoreDebounce);

    /**
     * @brief Renders the menu
     *
     * @param ctx The render context
     */
    void render(bl::rc::scene::CodeScene::RenderContext& ctx);

private:
    enum struct State {
        Hidden,
        PickingAction,
        PickingMove,
        PickingItem,
        PickingPeoplemon,
        ChoosingMoveToForget
    };

    bl::engine::Engine& engine;
    const bool canRun;
    State state;
    bool stateLoopGuard;
    input::MenuDriver menuDriver;
    TurnAction chosenAction;
    int chosenMoveOrPeoplemon;
    item::Id chosenItem;
    int currentPeoplemon;

    bl::menu::Menu actionMenu;
    bl::menu::TextItem::Ptr fightItem;
    bl::menu::TextItem::Ptr switchItem;
    bl::menu::TextItem::Ptr bagItem;
    bl::menu::TextItem::Ptr runItem;

    bl::menu::Menu moveMenu;
    bl::menu::TextItem::Ptr moveItems[4];
    const pplmn::OwnedMove* moves;

    bl::rc::res::TextureRef moveTxtr;
    bl::gfx::Sprite moveBox;
    bl::gfx::Text movePP;
    bl::gfx::Text movePwr;
    bl::gfx::Text moveAcc;

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
