#ifndef GAME_STATES_PEOPLEMONMENU_HPP
#define GAME_STATES_PEOPLEMONMENU_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Events/PeoplemonMenu.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Game/Menus/PeoplemonButton.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Provides the menu for viewing and selecting Peoplemon
 *
 * @ingroup States
 */
class PeoplemonMenu
: public State
, public bl::input::Listener {
public:
    using Context = core::event::OpenPeoplemonMenu::Context;

    /**
     * @brief Creates a new PeoplemonMenu state
     *
     * @param systems The main game systems
     * @param ctx The context the menu is being used in
     * @param outNow Optional index of the peoplemon that is currently out
     * @param chosen Value to populate with the selected peoplemon's index
     * @param item The item to use (or being used in battle)
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, Context ctx,
                                         int outNow = -1, int* chosen = nullptr,
                                         core::item::Id item = core::item::Id::None);

    /// Cleans up all resources
    virtual ~PeoplemonMenu() = default;

    /**
     * @brief Returns "PeoplemonMenu"
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes the menu to the engine event bus
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes the menu to the engine event bus
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Does nothing
     */
    virtual void update(bl::engine::Engine&, float dt, float) override;

private:
    enum MenuState {
        Browsing,
        SelectingMove,
        Moving,
        ShowingMessage,
        UsingItem,
        UsingItemWaitView,
        UsingItemWaitMessage,
        WaitingForgetConfirm,
        WaitingForgetChoice
    };

    const Context context;
    const int outNow;
    int* chosenPeoplemon;
    const core::item::Id useItem;

    MenuState state;
    unsigned int mover1;
    unsigned int mover2;
    glm::vec2 moveVel;
    glm::vec2 mover1Dest;
    glm::vec2 mover2Dest;

    bl::rc::res::TextureRef backgroundTxtr;
    bl::gfx::Sprite background;

    bl::menu::Menu menu;
    core::input::MenuDriver inputDriver;
    menu::PeoplemonButton::Ptr buttons[6];
    bl::menu::ImageItem::Ptr backBut;

    bl::menu::Menu actionMenu;
    bl::menu::Item* actionRoot;
    bool actionOpen;

    PeoplemonMenu(core::system::Systems& systems, Context ctx, int outNow, int* chosen,
                  core::item::Id item);

    void connectButtons();
    bool canCancel() const;
    void setSelectable(unsigned int i);

    void selected(menu::PeoplemonButton* but);

    void chosen(); // store or battle switch
    void showInfo();
    void startMove();
    void cleanupMove(bool completed);
    void takeItem();
    void resetAction();
    void messageDone();
    void closeActionMenu();
    void confirmMoveDelete(const std::string& choice);
    void delMove(const std::string& choice);

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
