#ifndef GAME_STATES_BAGMENU_HPP
#define GAME_STATES_BAGMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
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
class BagMenu : public State {
public:
    /// Represents how the menu was opened and affects how it may be used
    enum struct Context { BattleUse, PauseMenu };

    /**
     * @brief Creates a new BagMenu
     *
     * @param systems The main game systems
     * @param ctx Context the bag is being used from
     * @param result Optional item to populate with the chosen item
     * @return bl::engine::State::Ptr
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, Context ctx,
                                         core::item::Id* result = nullptr);

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
    enum struct MenuState { Browsing, Sliding, ChoosingGive };

    const Context context;
    core::item::Id* const result;

    MenuState state;
    float slideAmount;
    float slideVel;
    float slideOff;
    bl::menu::Menu* activeMenu;
    bl::menu::Menu* slideOut;

    sf::View oldView;
    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    sf::Sprite background;

    core::player::input::MenuDriver inputDriver;
    bl::menu::Menu actionMenu;
    bl::menu::Menu regularMenu;
    bl::menu::Menu keyMenu;
    bl::menu::Menu tmMenu;
    sf::Text pocketLabel;
    sf::Text description;
    bool actionOpen;
    menu::BagItemButton* selectedItem;
    PeoplemonMenu::ContextData pplContext;

    BagMenu(core::system::Systems& systems, Context ctx, core::item::Id* result);

    void itemHighlighted(menu::BagItemButton* but);
    void itemSelected(menu::BagItemButton* but);
    void exitSelected();

    void chooseItem();
    void useItem();
    void giveItem();
    void dropItem();
    void resetAction();
};

} // namespace state
} // namespace game

#endif
