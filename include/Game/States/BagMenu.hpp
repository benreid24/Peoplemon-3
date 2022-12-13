#ifndef GAME_STATES_BAGMENU_HPP
#define GAME_STATES_BAGMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Events/BagMenu.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Game/Menus/BagItemButton.hpp>
#include <Game/States/PeoplemonMenu.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief State for displaying the player's inventory and selecting items
 *
 * @ingroup States
 *
 */
class BagMenu
: public State
, public bl::input::Listener {
public:
    /// Represents how the menu was opened and affects how it may be used
    using Context = core::event::OpenBagMenu::Context;

    /**
     * @brief Creates a new BagMenu
     *
     * @param systems The main game systems
     * @param ctx Context the bag is being used from
     * @param result Optional item to populate with the chosen item
     * @param outNow Index of the currently out Peoplemon if in battle
     * @param unpause Pointer to boolean to set if pause menu should close after bag closes
     * @return bl::engine::State::Ptr
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, Context ctx,
                                         core::item::Id* result = nullptr, int outNow = -1,
                                         int* chosenPeoplemon = nullptr, bool* unpause = nullptr);

    /**
     * @brief Destroy the Bag Menu object
     *
     */
    virtual ~BagMenu() = default;

    /**
     * @brief Returns "BagMenu"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Populates the menu with the player's inventory
     *
     */
    virtual void activate(bl::engine::Engine&) override;

    /**
     * @brief Unsubscribes from event and input busses
     *
     */
    virtual void deactivate(bl::engine::Engine&) override;

    /**
     * @brief Updates the menu
     *
     * @param dt Time elapsed in seconds since the last call to update
     */
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the menu to the screen
     *
     */
    virtual void render(bl::engine::Engine&, float) override;

private:
    enum struct MenuState {
        Browsing,
        Sliding,
        ChoosingGive,
        ShowingMessage,
        ChoosingItemPeoplemon,
        ImmediatelyPop
    };

    const Context context;
    const int outNow;
    core::item::Id* const result;
    int* itemPeoplemon;
    bool* unpause;

    MenuState state;
    float slideVel;
    float slideOff;
    bl::menu::Menu* activeMenu;
    bl::menu::Menu* slideOut;

    bl::resource::Ref<sf::Texture> bgndTxtr;
    sf::Sprite background;

    core::input::MenuDriver inputDriver;
    bl::menu::Menu actionMenu;
    bl::menu::Menu regularMenu;
    bl::menu::Menu ballMenu;
    bl::menu::Menu keyMenu;
    bl::menu::Menu tmMenu;
    bl::menu::Menu* menuTabs[4];
    sf::Text pocketLabel;
    sf::Text description;
    bool actionOpen;
    menu::BagItemButton* selectedItem;
    int selectedPeoplemon;

    BagMenu(core::system::Systems& systems, Context ctx, core::item::Id* result, int outNow,
            int* chosenPeoplemon, bool* unpause);

    void itemHighlighted(const menu::BagItemButton* but);
    void itemSelected(const menu::BagItemButton* but);
    void exitSelected();
    void beginSlide(bool left);
    void removeAndUpdateItem(int qty);

    void useItem();
    void giveItem();
    void dropItem();
    void doDrop(int qty);
    void resetAction();

    void messageDone();
    void keyItemConfirmUse(const std::string& choice);

    // this will only get called for inputs not processed by menus
    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
