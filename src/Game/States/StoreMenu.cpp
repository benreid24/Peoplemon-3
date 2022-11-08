#include <Game/States/StoreMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f QtyPosition{293.f, 399.f};
constexpr float DingTime               = 0.6f;
constexpr core::item::Category cats[3] = {
    core::item::Category::Regular, core::item::Category::Peopleball, core::item::Category::TM};
const std::string catTexts[3] = {"Regular Items", "Peopleballs", "TM's"};
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
, sellMenus{{bl::menu::ArrowSelector::create(8.f, sf::Color::Black)},
            {bl::menu::ArrowSelector::create(8.f, sf::Color::Black)},
            {bl::menu::ArrowSelector::create(8.f, sf::Color::Black)}}
, leftArrow({0.f, 13.f}, {15.f, 0.f}, {15.f, 26.f})
, rightArrow({0.f, 0.f}, {0.f, 26.f}, {15.f, 13.f}) {
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

    boxText.setPosition(28.f, 408.f);
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
    catText.setFont(core::Properties::MenuFont());
    catText.setCharacterSize(30);
    catText.setFillColor(sf::Color(0, 60, 20));

    leftArrow.setFillColor(sf::Color::Black);
    rightArrow.setFillColor(sf::Color::Black);
    rightArrow.setPosition(730.f, 83.f);

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
    for (int i = 0; i < 3; ++i) {
        sellMenus[i].setPosition({378.f, 140.f});
        sellMenus[i].setMaximumSize({500.f, 420.f});
    }
    curCat = 0;

    for (const auto& p : data.sellPrices) { sellPrices.emplace(p.first, p.second); }
}

const char* StoreMenu::name() const { return "StoreMenu"; }

void StoreMenu::activate(bl::engine::Engine& engine) {
    if (items.empty()) {
        BL_LOG_ERROR << "Nothing is for sale asshole";
        engine.popState();
        return;
    }

    // create view for menu
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));

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

    std::vector<core::player::Bag::Item> playerItems;
    for (int i = 0; i < 3; ++i) {
        systems.player().state().bag.getByCategory(cats[i], playerItems);
        if (!playerItems.empty()) {
            bl::menu::Item::Ptr root =
                makeSellItemRow(playerItems.front().id, playerItems.front().qty);
            sellMenus[i].setRootItem(root);
            prev = root.get();
            for (unsigned int j = 1; j < playerItems.size(); ++j) {
                bl::menu::Item::Ptr row = makeSellItemRow(playerItems[j].id, playerItems[j].qty);
                sellMenus[i].addItem(row, prev, bl::menu::Item::Bottom);
                prev = row.get();
            }
            sellMenus[i].addItem(makeCloseItem(), prev, bl::menu::Item::Bottom);
        }
        else {
            sellMenus[i].setRootItem(makeCloseItem());
        }
    }
    curCat = 0;
    catSync();

    // general setup
    dingTime = 0.f;
    enterState(MenuState::GetAction);
    setMoneyText();
    renderMenu = &buyMenu;
    systems.engine().inputSystem().getActor().addListener(*this);

    // impulse buy ability
    for (const auto& ppl : systems.player().state().peoplemon) {
        if (ppl.ability() == core::pplmn::SpecialAbility::ImpulseBuy) {
            if (bl::util::Random::get<int>(0, 100) <= 5) {
                core::item::Id cheapest = core::item::Id::None;
                int cheapestPrice       = std::numeric_limits<int>::max();
                for (const auto item : items) {
                    if (item.price < cheapestPrice) {
                        cheapestPrice = item.price;
                        cheapest      = item.item;
                    }
                }

                if (cheapestPrice <= systems.player().state().monei) {
                    systems.player().state().monei -= cheapestPrice;
                    systems.player().state().bag.addItem(cheapest);
                    enterState(MenuState::ImpulseBuyMessage);
                    setBoxText(ppl.name() + " made an Impulse Buy and purchased a " +
                               core::item::Item::getName(cheapest) + "!");
                }
            }
        }
    }
}

void StoreMenu::deactivate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().popCamera();
    systems.engine().inputSystem().getActor().removeListener(*this);
    engine.eventBus().dispatch<core::event::StoreClosed>({});
}

void StoreMenu::update(bl::engine::Engine&, float dt) {
    dingTime += dt;

    switch (menuState) {
    case MenuState::BuyDing:
    case MenuState::SellDing:
    case MenuState::TooPoorDing:
        if (dingTime >= DingTime) {
            enterState(menuState == MenuState::SellDing ? MenuState::SellMenu : MenuState::BuyMenu);
        }
        break;
    case MenuState::ImpulseBuyMessage:
        if (dingTime >= DingTime * 2.3f) { enterState(MenuState::GetAction); }
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
    if (menuState == MenuState::SellMenu || menuState == MenuState::SellDing ||
        menuState == MenuState::SellQty) {
        engine.window().draw(catText);
        engine.window().draw(rightArrow);
        engine.window().draw(leftArrow);
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
        renderMenu = &sellMenus[curCat];
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
        qtyEntry.configure(0, systems.player().state().bag.itemCount(sellingItem->getItem()), 1);
        qtyEntry.setPosition(QtyPosition - qtyEntry.getSize());
        setBoxText("How many would you like to sell?");
        break;

    case MenuState::TooPoorDing:
        setBoxText("You're too poor!");
        [[fallthrough]];

    case MenuState::BuyDing:
    case MenuState::SellDing:
    case MenuState::ImpulseBuyMessage:
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
    bl::interface::wordWrap(boxText, 260.f);
}

void StoreMenu::setMoneyText() {
    // TODO - $ in our font looks like an S
    moneyText.setString("$" + std::to_string(systems.player().state().monei));
}

void StoreMenu::close() { systems.engine().popState(); }

bool StoreMenu::observe(const bl::input::Actor&, unsigned int control, bl::input::DispatchType,
                        bool eventTriggered) {
    if (control == core::input::Control::Back) {
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
        return true;
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
        switch (control) {
        case core::input::Control::MoveRight:
            catRight();
            break;
        case core::input::Control::MoveLeft:
            catLeft();
            break;
        default:
            sendTo = &sellMenus[curCat];
            break;
        }
        break;
    case MenuState::BuyQty:
    case MenuState::SellQty:
        switch (control) {
        case core::input::Control::MoveUp:
            qtyEntry.up(1, eventTriggered);
            break;
        case core::input::Control::MoveRight:
            qtyEntry.up(10, eventTriggered);
            break;
        case core::input::Control::MoveDown:
            qtyEntry.down(1, eventTriggered);
            break;
        case core::input::Control::MoveLeft:
            qtyEntry.down(10, eventTriggered);
            break;
        case core::input::Control::Interact:
            if (menuState == MenuState::BuyQty) {
                const int qty = qtyEntry.curQty();
                if (qty > 0) {
                    systems.player().state().monei -= qty * items[buyingItemIndex].price;
                    systems.player().state().bag.addItem(items[buyingItemIndex].item, qty);
                    enterState(MenuState::BuyDing);
                }
                else {
                    enterState(MenuState::BuyMenu);
                }
            }
            else {
                const int qty = qtyEntry.curQty();
                if (qty > 0) {
                    systems.player().state().monei += getSellPrice(sellingItem->getItem()) * qty;
                    systems.player().state().bag.removeItem(sellingItem->getItem(), qty);
                    const int newQty =
                        systems.player().state().bag.itemCount(sellingItem->getItem());
                    if (newQty > 0) { sellingItem->updateQty(newQty); }
                    else {
                        sellMenus[curCat].removeItem(sellingItem);
                    }
                    enterState(MenuState::SellDing);
                }
                else {
                    enterState(MenuState::SellMenu);
                }
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
        inputDriver.sendControl(control, eventTriggered);
    }

    return true;
}

bl::menu::Item::Ptr StoreMenu::makeItemRow(core::item::Id item, int price, unsigned int i) {
    StoreItemRow::Ptr row = StoreItemRow::create(item, price);
    row->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::selectBuyItem, this, i));
    return row;
}

bl::menu::Item::Ptr StoreMenu::makeSellItemRow(core::item::Id item, int qty) {
    StoreItemRow::Ptr row = StoreItemRow::create(qty, item, getSellPrice(item));
    row->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&StoreMenu::selectSellItem, this, row.get()));
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

void StoreMenu::selectSellItem(StoreItemRow* row) {
    sellingItem = row;
    enterState(MenuState::SellQty);
}

void StoreMenu::catRight() {
    if (dingTime >= DingTime) {
        dingTime = 0.f;
        curCat   = (curCat + 1) % 3;
        catSync();
        bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveSound());
    }
}

void StoreMenu::catLeft() {
    if (dingTime >= DingTime) {
        dingTime = 0.f;
        curCat   = curCat > 0 ? curCat - 1 : 2;
        catSync();
        bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveSound());
    }
}

void StoreMenu::catSync() {
    catText.setString(catTexts[curCat]);
    catText.setPosition(745.f - catText.getGlobalBounds().width -
                            rightArrow.getGlobalBounds().width - 9.f,
                        actionText.getPosition().y);
    leftArrow.setPosition(catText.getPosition().x - leftArrow.getGlobalBounds().width + 3.f,
                          rightArrow.getPosition().y);
    renderMenu = &sellMenus[curCat];
}

int StoreMenu::getSellPrice(core::item::Id item) const {
    const auto it = sellPrices.find(item);
    return it == sellPrices.end() ? core::item::Item::getValue(item) * 4 / 5 : it->second;
}

} // namespace state
} // namespace game
