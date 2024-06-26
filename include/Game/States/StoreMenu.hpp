#ifndef GAME_STATES_STOREMENU_HPP
#define GAME_STATES_STOREMENU_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Events/Store.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Core/Systems/HUD/QtyEntry.hpp>
#include <Game/Menus/StoreItemRow.hpp>
#include <Game/States/State.hpp>
#include <unordered_map>
#include <vector>

namespace game
{
namespace state
{
/**
 * @brief State that renders and implements a store menu to buy and sell items
 *
 * @ingroup States
 */
class StoreMenu
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Create a new StoreMenu
     *
     * @param systems The main game systems
     * @param data The items to populate the store with
     * @return bl::engine::State::Ptr The new store state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems,
                                         const core::event::StoreOpened& data);

    /**
     * @brief Destroy the Store State object
     */
    virtual ~StoreMenu() = default;

    /**
     * @brief Returns "StoreMenu"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes to event buses
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes from event buses
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the store menu
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds since the last call to update
     */
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    struct Item {
        const core::item::Id item;
        const int price;

        Item(core::item::Id item, int price);
    };
    enum struct MenuState {
        ImpulseBuyMessage,
        GetAction,
        BuyMenu,
        BuyQty,
        BuyDing,
        SellMenu,
        SellQty,
        SellDing,
        TooPoorDing
    };

    MenuState menuState;
    std::vector<Item> items;
    std::unordered_map<core::item::Id, int> sellPrices;
    bl::audio::AudioSystem::Handle dingSound;
    core::input::MenuDriver inputDriver;
    unsigned int buyingItemIndex;
    menu::StoreItemRow* sellingItem;
    float dingTime;
    unsigned int curCat;

    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;

    bl::menu::Menu* renderMenu;
    bl::menu::Menu actionMenu;
    bl::menu::Menu buyMenu;
    bl::menu::Menu sellMenus[3];
    core::system::hud::QtyEntry qtyEntry;
    bl::gfx::Text actionText;
    bl::gfx::Text boxText;
    bl::gfx::Text moneyText;
    bl::gfx::Text catText;
    bl::gfx::Triangle leftArrow;
    bl::gfx::Triangle rightArrow;

    StoreMenu(core::system::Systems& systems, const core::event::StoreOpened& data);

    void enterState(MenuState state);
    void setBoxText(const std::string& text);
    void setMoneyText();
    int getSellPrice(core::item::Id item) const;

    void closeMenu();
    void selectBuyItem(unsigned int i);
    void selectSellItem(menu::StoreItemRow* row);
    void catRight();
    void catLeft();
    void catSync();

    void close();

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;

    bl::menu::Item::Ptr makeItemRow(core::item::Id item, int price, unsigned int i);
    bl::menu::Item::Ptr makeSellItemRow(core::item::Id item, int qty);
    bl::menu::Item::Ptr makeCloseItem();
};

} // namespace state
} // namespace game

#endif
