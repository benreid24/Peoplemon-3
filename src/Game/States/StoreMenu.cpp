#include <Game/States/StoreMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Game/Menus/StoreItemRow.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f QtyPosition{293.f, 399.f};
constexpr float DingTime = 0.6f;
} // namespace

using menu::StoreItemRow;

StoreMenu::Item::Item(core::item::Id item, int price)
: item(item)
, price(price) {}

bl::engine::State::Ptr StoreMenu::create(core::system::Systems& systems,
                                         const core::event::StoreOpened& data) {
    return bl::engine::State::Ptr(new StoreMenu(systems, data));
}

StoreMenu::StoreMenu(core::system::Systems& systems, const core::event::StoreOpened& data)
: State(systems)
, menuState(MenuState::GetAction)
, actionMenu(bl::menu::ArrowSelector::create(14.f, sf::Color::Black))
, buyMenu(bl::menu::ArrowSelector::create(8.f, sf::Color::Black))
, sellMenu(bl::menu::ArrowSelector::create(8.f, sf::Color::Black)) {
    bgndTxtr =
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "store.png"))
            .data;
    background.setTexture(*bgndTxtr, true);

    dingSound = bl::audio::AudioSystem::getOrLoadSound(
        bl::util::FileUtil::joinPath(core::Properties::SoundPath(), "Menu/ding.wav"));

    if (data.items.empty()) {
        BL_LOG_INFO
            << "Received no item list, selling all regular items and Peopleballs at normal price";

        for (const core::item::Id item : core::item::Item::validIds()) {
            const core::item::Category cat = core::item::Item::getCategory(item);
            if (cat == core::item::Category::Regular || cat == core::item::Category::Peopleball) {
                items.emplace_back(item, core::item::Item::getValue(item));
            }
        }
    }
    else {
        items.reserve(data.items.size());
        for (const auto& item : data.items) { items.emplace_back(item.first, item.second); }
    }

    boxText.setPosition(26.f, 408.f);
    moneyText.setPosition(605.f, 15.f);
    actionText.setPosition(378.f, 67.f);
    boxText.setFont(core::Properties::MenuFont());
    moneyText.setFont(core::Properties::MenuFont());
    actionText.setFont(core::Properties::MenuFont());
    boxText.setCharacterSize(18);
    moneyText.setCharacterSize(20);
    actionText.setCharacterSize(30);
    boxText.setFillColor(sf::Color::Black);
    moneyText.setFillColor(sf::Color(10, 115, 0));

    bl::menu::TextItem::Ptr buyItem =
        bl::menu::TextItem::create("Buy", core::Properties::MenuFont(), sf::Color::Black, 28);
    bl::menu::TextItem::Ptr sellItem =
        bl::menu::TextItem::create("Sell", core::Properties::MenuFont(), sf::Color::Black, 28);
    bl::menu::TextItem::Ptr closeItem =
        bl::menu::TextItem::create("Close", core::Properties::MenuFont(), sf::Color::Black, 28);
    buyItem->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::enterState, this, MenuState::BuyMenu));
    sellItem->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::enterState, this, MenuState::SellMenu));
    closeItem->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::close, this));
    actionMenu.setRootItem(buyItem);
    actionMenu.addItem(sellItem, buyItem.get(), bl::menu::Item::Bottom);
    actionMenu.addItem(closeItem, sellItem.get(), bl::menu::Item::Bottom);
    actionMenu.setSelectedItem(buyItem.get());
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 2.f, {5.f, 2.f, -4.f, -6.f});
    actionMenu.setPosition({20.f, 230.f});

    buyMenu.setPosition({378.f, 140.f});
    buyMenu.setMaximumSize({500.f, 420.f});
}

const char* StoreMenu::name() const { return "StoreMenu"; }

void StoreMenu::activate(bl::engine::Engine& engine) {
    if (items.empty()) {
        BL_LOG_ERROR << "Nothing is for sale asshole";
        engine.popState();
        return;
    }

    // create view for menu
    oldView = engine.window().getView();
    sf::View view(oldView);
    const sf::Vector2f size(background.getGlobalBounds().width,
                            background.getGlobalBounds().height);
    view.setCenter(size * 0.5f);
    view.setSize(size);
    engine.window().setView(view);

    // populate buy menu
    bl::menu::Item::Ptr root = makeItemRow(items.front().item, items.front().price, 0);
    buyMenu.setRootItem(root);
    bl::menu::Item* prev = root.get();
    for (unsigned int i = 1; i < items.size(); ++i) {
        bl::menu::Item::Ptr row = makeItemRow(items[i].item, items[i].price, i);
        buyMenu.addItem(row, prev, bl::menu::Item::Bottom);
        prev = row.get();
    }
    buyMenu.addItem(makeCloseItem(), prev, bl::menu::Item::Bottom);

    // TODO - populate selling menus

    // general setup
    dingTime = 0.f;
    enterState(MenuState::GetAction);
    setMoneyText();
    renderMenu = &buyMenu;
    systems.player().inputSystem().addListener(*this);
}

void StoreMenu::deactivate(bl::engine::Engine& engine) {
    engine.window().setView(oldView);
    systems.player().inputSystem().removeListener(*this);
}

void StoreMenu::update(bl::engine::Engine&, float dt) {
    systems.player().update();
    dingTime += dt;

    switch (menuState) {
    case MenuState::BuyDing:
    case MenuState::SellDing:
    case MenuState::TooPoorDing:
        if (dingTime >= DingTime) {
            enterState(menuState == MenuState::SellDing ? MenuState::SellMenu : MenuState::BuyMenu);
        }
        break;
    default:
        break;
    }
}

void StoreMenu::render(bl::engine::Engine& engine, float) {
    engine.window().clear();

    engine.window().draw(background);
    engine.window().draw(moneyText);
    engine.window().draw(boxText);
    engine.window().draw(actionText);

    actionMenu.render(engine.window());
    renderMenu->render(engine.window());

    if (menuState == MenuState::BuyQty || menuState == MenuState::SellQty) {
        qtyEntry.render(engine.window());
    }

    engine.window().display();
}

void StoreMenu::enterState(MenuState newState) {
    menuState = newState;

    switch (newState) {
    case MenuState::GetAction:
        setBoxText("Would you like to buy or sell?");
        break;

    case MenuState::BuyMenu:
        renderMenu = &buyMenu;
        setBoxText("What would you like to buy?");
        actionText.setFillColor(sf::Color(20, 60, 240));
        actionText.setString("Buying");
        break;

    case MenuState::SellMenu:
        renderMenu = &sellMenu;
        setBoxText("What can I take off your hands?");
        actionText.setFillColor(sf::Color(240, 60, 20));
        actionText.setString("Selling");
        break;

    case MenuState::BuyQty:
        qtyEntry.configure(0, systems.player().state().monei / items[buyingItemIndex].price, 1);
        qtyEntry.setPosition(QtyPosition - qtyEntry.getSize());
        setBoxText("How many would you like to buy?");
        break;

    case MenuState::SellQty:
        qtyEntry.configure(0, systems.player().state().bag.itemCount(sellingItem), 1);
        qtyEntry.setPosition(QtyPosition - qtyEntry.getSize());
        setBoxText("How many would you like to sell?");
        break;

    case MenuState::TooPoorDing:
        setBoxText("You're too poor!");
        [[fallthrough]];

    case MenuState::BuyDing:
    case MenuState::SellDing:
        bl::audio::AudioSystem::playOrRestartSound(dingSound);
        dingTime = 0.f;
        setMoneyText();
        break;

    default:
        break;
    }
}

void StoreMenu::setBoxText(const std::string& t) {
    boxText.setString(t);
    bl::interface::wordWrap(boxText, 262.f);
}

void StoreMenu::setMoneyText() {
    // TODO - $ in our font looks like an S
    moneyText.setString("$" + std::to_string(systems.player().state().monei));
}

void StoreMenu::close() { systems.engine().popState(); }

void StoreMenu::process(core::component::Command control) {
    if (control == core::component::Command::Back) {
        switch (menuState) {
        case MenuState::GetAction:
            close();
            break;
        case MenuState::BuyMenu:
        case MenuState::SellMenu:
            enterState(MenuState::GetAction);
            break;
        case MenuState::SellQty:
            enterState(MenuState::SellMenu);
            break;
        case MenuState::BuyQty:
            enterState(MenuState::BuyMenu);
            break;
        default:
            break;
        }
        return;
    }

    bl::menu::Menu* sendTo = nullptr;
    switch (menuState) {
    case MenuState::GetAction:
        sendTo = &actionMenu;
        break;
    case MenuState::BuyMenu:
        sendTo = &buyMenu;
        break;
    case MenuState::SellMenu:
        sendTo = &sellMenu;
        break;
    case MenuState::BuyQty:
    case MenuState::SellQty:
        switch (control) {
        case core::component::Command::MoveUp:
            qtyEntry.up(1);
            break;
        case core::component::Command::MoveRight:
            qtyEntry.up(10);
            break;
        case core::component::Command::MoveDown:
            qtyEntry.down(1);
            break;
        case core::component::Command::MoveLeft:
            qtyEntry.down(10);
            break;
        case core::component::Command::Interact:
            if (menuState == MenuState::BuyQty) {
                systems.player().state().monei -= qtyEntry.curQty() * items[buyingItemIndex].price;
                systems.player().state().bag.addItem(items[buyingItemIndex].item,
                                                     qtyEntry.curQty());
                enterState(MenuState::BuyDing);
            }
            else {
                // TODO - sell and stuff
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    if (sendTo != nullptr && dingTime >= DingTime) {
        inputDriver.drive(sendTo);
        inputDriver.process(control);
    }
}

bl::menu::Item::Ptr StoreMenu::makeItemRow(core::item::Id item, int price, unsigned int i) {
    StoreItemRow::Ptr row = StoreItemRow::create(item, price);
    row->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::selectBuyItem, this, i));
    return row;
}

bl::menu::Item::Ptr StoreMenu::makeCloseItem() {
    bl::menu::TextItem::Ptr ci =
        bl::menu::TextItem::create("Close", core::Properties::MenuFont(), sf::Color::Black, 16);
    ci->getSignal(bl::menu::Item::Activated).willAlwaysCall(std::bind(&StoreMenu::closeMenu, this));
    return ci;
}

void StoreMenu::selectBuyItem(unsigned int i) {
    buyingItemIndex = i;
    enterState(systems.player().state().monei >= items[i].price ? MenuState::BuyQty :
                                                                  MenuState::TooPoorDing);
}

void StoreMenu::closeMenu() { enterState(MenuState::GetAction); }

} // namespace state
} // namespace game
