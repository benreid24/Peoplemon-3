#include <Game/States/BagMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
using ExitCb = std::function<void()>;
using ItemCb = std::function<void(menu::BagItemButton*)>;

constexpr float SlideTime = 0.5f;

void populateMenu(bl::menu::Menu& menu, const std::vector<core::player::Bag::Item>& items,
                  const ExitCb& ecb, const ItemCb& icb, const ItemCb& dcb) {
    menu::BagItemButton::Ptr exit = menu::BagItemButton::create({core::item::Id::None, 0});
    exit->getSignal(bl::menu::Item::Activated).willAlwaysCall(ecb);
    exit->getSignal(bl::menu::Item::Selected).willAlwaysCall(std::bind(dcb, exit.get()));
    menu.setRootItem(exit);
    menu.setMoveFailSound(bl::audio::AudioSystem::InvalidHandle);

    menu::BagItemButton* b = exit.get();
    for (auto rit = items.rbegin(); rit != items.rend(); ++rit) {
        menu::BagItemButton::Ptr nb = menu::BagItemButton::create(*rit);
        nb->getSignal(bl::menu::Item::Activated).willAlwaysCall(std::bind(icb, nb.get()));
        menu.addItem(nb, b, bl::menu::Item::Top);
        nb->getSignal(bl::menu::Item::Selected).willAlwaysCall(std::bind(dcb, nb.get()));
        b = nb.get();
    }
    if (b != exit.get()) { menu.attachExisting(exit.get(), b, bl::menu::Item::Top); }
    menu.setSelectedItem(b);
}

bool needPickPeoplemon(core::item::Id item, core::item::Type type) {
    switch (type) {
    case core::item::Type::EvolveStone:
    case core::item::Type::TM:
        return true;
    case core::item::Type::TargetPeoplemon:
        return item != core::item::Id::Pp6Pack && item != core::item::Id::SuperPp6Pack;
    default:
        return false;
    }
}
} // namespace

bl::engine::State::Ptr BagMenu::create(core::system::Systems& s, Context c, core::item::Id* i,
                                       int outNow, int* chosenPpl) {
    return bl::engine::State::Ptr(new BagMenu(s, c, i, outNow, chosenPpl));
}

BagMenu::BagMenu(core::system::Systems& s, Context c, core::item::Id* i, int outNow, int* chosenPpl)
: State(s)
, context(c)
, outNow(outNow)
, result(i)
, itemPeoplemon(chosenPpl ? chosenPpl : &selectedPeoplemon)
, state(MenuState::Browsing)
, activeMenu(&regularMenu)
, actionMenu(bl::menu::ArrowSelector::create(10.f, sf::Color::Black))
, regularMenu(bl::menu::NoSelector::create())
, keyMenu(bl::menu::NoSelector::create())
, tmMenu(bl::menu::NoSelector::create())
, actionOpen(false) {
    bgndTxtr = bl::engine::Resources::textures()
                   .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                      "Bag/background.png"))
                   .data;
    background.setTexture(*bgndTxtr, true);

    if (context == Context::PauseMenu) {
        bl::menu::Item::Ptr use =
            bl::menu::TextItem::create("Use", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::Item::Ptr give =
            bl::menu::TextItem::create("Give", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::Item::Ptr drop =
            bl::menu::TextItem::create("Drop", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::Item::Ptr back =
            bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 26);

        use->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::useItem, this));
        give->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::giveItem, this));
        drop->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::dropItem, this));
        back->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::resetAction, this));

        actionMenu.setRootItem(use);
        actionMenu.addItem(drop, use.get(), bl::menu::Item::Bottom);
        actionMenu.addItem(back, drop.get(), bl::menu::Item::Right);
        actionMenu.addItem(give, back.get(), bl::menu::Item::Top);
        actionMenu.attachExisting(give.get(), use.get(), bl::menu::Item::Right);
    }
    else {
        bl::menu::Item::Ptr use =
            bl::menu::TextItem::create("Use", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::Item::Ptr back =
            bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 26);

        use->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::useItem, this));
        back->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&BagMenu::resetAction, this));

        actionMenu.setRootItem(use);
        actionMenu.addItem(back, use.get(), bl::menu::Item::Bottom);
    }
    actionMenu.setPadding({16.f, 4.f});
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {14.f, 2.f, 2.f, 2.f});
    actionMenu.setPosition({252.f, 82.f});

    pocketLabel.setString("Regular Items");
    pocketLabel.setFont(core::Properties::MenuFont());
    pocketLabel.setCharacterSize(30);
    pocketLabel.setFillColor(sf::Color(0, 20, 75));
    pocketLabel.setPosition({72.f, 342.f});

    description.setFont(core::Properties::MenuFont());
    description.setCharacterSize(20);
    description.setFillColor(sf::Color::Black);
    description.setPosition({98.f, 473.f});

    regularMenu.configureBackground(
        sf::Color::Transparent, sf::Color::Transparent, 0.f, {0.f, 0.f, 0.f, 0.f});
    tmMenu.configureBackground(
        sf::Color::Transparent, sf::Color::Transparent, 0.f, {0.f, 0.f, 0.f, 0.f});
    keyMenu.configureBackground(
        sf::Color::Transparent, sf::Color::Transparent, 0.f, {0.f, 0.f, 0.f, 0.f});
    regularMenu.setPosition({365.f, 30.f});
    keyMenu.setPosition({365.f, 30.f});
    tmMenu.setPosition({365.f, 30.f});
    regularMenu.setMaximumSize({-1.f, 385.f});
    tmMenu.setMaximumSize({-1.f, 385.f});
    keyMenu.setMaximumSize({-1.f, 385.f});
}

const char* BagMenu::name() const { return "BagMenu"; }

void BagMenu::activate(bl::engine::Engine& engine) {
    bl::menu::Menu* toDrive = nullptr;

    if (state == MenuState::ChoosingGive) {
        if (*itemPeoplemon >= 0) {
            resetAction();
            systems.player().state().peoplemon[*itemPeoplemon].holdItem() =
                selectedItem->getItem().id;
            removeAndUpdateItem(1);
            toDrive = activeMenu;
        }
        else {
            toDrive = &actionMenu;
        }
        state = MenuState::Browsing;
    }
    else if (state == MenuState::ChoosingItemPeoplemon) {
        if (*itemPeoplemon >= 0) {
            if (context == Context::BattleUse) {
                // battle applies and removes the item
                if (result) *result = selectedItem->getItem().id;
                state = MenuState::ImmediatelyPop;
            }
            else {
                resetAction();
                removeAndUpdateItem(0); // item is removed in other menu
                state = MenuState::Browsing;
            }
            toDrive = activeMenu;
        }
        else {
            toDrive = &actionMenu;
            state   = MenuState::Browsing;
        }
    }
    else { // first time activation
        oldView = engine.window().getView();
        sf::View v(oldView);
        const sf::Vector2f size(background.getGlobalBounds().width,
                                background.getGlobalBounds().height);
        v.setCenter(size * 0.5f);
        v.setSize(size);
        engine.window().setView(v);

        systems.player().inputSystem().addListener(inputDriver);
        if (result) *result = core::item::Id::None;

        std::vector<core::player::Bag::Item> items;
        systems.player().state().bag.getByCategory(core::item::Category::TM, items);
        populateMenu(tmMenu,
                     items,
                     std::bind(&BagMenu::exitSelected, this),
                     std::bind(&BagMenu::itemSelected, this, std::placeholders::_1),
                     std::bind(&BagMenu::itemHighlighted, this, std::placeholders::_1));
        systems.player().state().bag.getByCategory(core::item::Category::Key, items);
        populateMenu(keyMenu,
                     items,
                     std::bind(&BagMenu::exitSelected, this),
                     std::bind(&BagMenu::itemSelected, this, std::placeholders::_1),
                     std::bind(&BagMenu::itemHighlighted, this, std::placeholders::_1));
        systems.player().state().bag.getByCategory(core::item::Category::Regular, items);
        populateMenu(regularMenu,
                     items,
                     std::bind(&BagMenu::exitSelected, this),
                     std::bind(&BagMenu::itemSelected, this, std::placeholders::_1),
                     std::bind(&BagMenu::itemHighlighted, this, std::placeholders::_1));

        state      = MenuState::Browsing;
        actionOpen = false;
        activeMenu = &regularMenu;
        toDrive    = activeMenu;
    }

    systems.player().inputSystem().addListener(inputDriver);
    inputDriver.drive(toDrive);
}

void BagMenu::deactivate(bl::engine::Engine& engine) {
    engine.window().setView(oldView);
    systems.player().inputSystem().removeListener(inputDriver);
}

void BagMenu::update(bl::engine::Engine& engine, float dt) {
    systems.player().update();

    switch (state) {
    case MenuState::Browsing: {
        const core::component::Command input = inputDriver.mostRecentInput();

        if (actionOpen) {
            if (input == core::component::Command::Back) {
                bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
                resetAction();
            }
        }
        else {
            switch (input) {
            case core::component::Command::Back:
                engine.popState();
                break;

            case core::component::Command::MoveLeft:
                beginSlide(true);
                if (activeMenu == &regularMenu) {
                    activeMenu = &keyMenu;
                    pocketLabel.setString("Key Items");
                }
                else if (activeMenu == &keyMenu) {
                    activeMenu = &tmMenu;
                    pocketLabel.setString("TM's");
                }
                else {
                    activeMenu = &regularMenu;
                    pocketLabel.setString("Regular Items");
                }
                itemHighlighted(
                    dynamic_cast<const menu::BagItemButton*>(activeMenu->getSelectedItem()));
                break;

            case core::component::Command::MoveRight:
                beginSlide(false);
                if (activeMenu == &regularMenu) {
                    activeMenu = &tmMenu;
                    pocketLabel.setString("TM's");
                }
                else if (activeMenu == &keyMenu) {
                    activeMenu = &regularMenu;
                    pocketLabel.setString("Regular Items");
                }
                else {
                    activeMenu = &keyMenu;
                    pocketLabel.setString("Key Items");
                }
                itemHighlighted(
                    dynamic_cast<const menu::BagItemButton*>(activeMenu->getSelectedItem()));
                break;

            default:
                break;
            }
        }
    } break;

    case MenuState::Sliding:
        slideOff += slideVel * dt;
        if (std::abs(slideOff) >= activeMenu->getBounds().width) {
            state = MenuState::Browsing;
            inputDriver.drive(activeMenu);
        }
        break;

    case MenuState::ShowingMessage:
        systems.hud().update(dt);
        break;

    case MenuState::ImmediatelyPop:
        engine.popState();
        break;

    default:
        break;
    }
}

void BagMenu::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    if (state == MenuState::ImmediatelyPop) return;

    engine.window().draw(background);
    if (state == MenuState::Sliding) {
        const sf::View oldView = engine.window().getView();
        sf::RenderStates states;
        states.transform.translate({slideOff, 0.f});
        slideOut->render(engine.window(), states);
        const float m = slideVel > 0.f ? -1.f : 1.f;
        states.transform.translate({m * regularMenu.getBounds().width, 0});
        activeMenu->render(engine.window(), states);
        engine.window().setView(oldView);
    }
    else {
        activeMenu->render(engine.window());
    }
    if (actionOpen) { actionMenu.render(engine.window()); }
    engine.window().draw(pocketLabel);
    engine.window().draw(description);
    if (state == MenuState::ShowingMessage) { systems.hud().render(engine.window(), lag); }
    engine.window().display();
}

void BagMenu::itemSelected(const menu::BagItemButton* but) {
    selectedItem = const_cast<menu::BagItemButton*>(but);
    actionOpen   = true;
    inputDriver.drive(&actionMenu);
}

void BagMenu::exitSelected() { systems.engine().popState(); }

void BagMenu::useItem() {
    if (context == Context::BattleUse) {
        if (!core::item::Item::canUseInBattle(selectedItem->getItem().id)) {
            state = MenuState::ShowingMessage;
            systems.hud().displayMessage("This item cannot be used in battle!",
                                         std::bind(&BagMenu::messageDone, this));
            return;
        }
    }

    const core::item::Type type = core::item::Item::getType(selectedItem->getItem().id);
    if (needPickPeoplemon(selectedItem->getItem().id, type)) {
        state = MenuState::ChoosingItemPeoplemon;
        systems.engine().pushState(PeoplemonMenu::create(systems,
                                                         context == Context::BattleUse ?
                                                             PeoplemonMenu::Context::UseItemBattle :
                                                             PeoplemonMenu::Context::UseItem,
                                                         outNow,
                                                         itemPeoplemon,
                                                         selectedItem->getItem().id));
    }
    else {
        if (context == Context::BattleUse) {
            if (result) *result = selectedItem->getItem().id;
            systems.engine().popState();
        }
        else {
            switch (type) {
            case core::item::Type::KeyItem:
                // TODO - use key items
                break;
            case core::item::Type::PlayerModifier:
                if (core::item::Item::hasEffectOnPlayer(selectedItem->getItem().id,
                                                        systems.player().state())) {
                    core::item::Item::useOnPlayer(selectedItem->getItem().id,
                                                  systems.player().state());
                    systems.player().state().bag.removeItem(selectedItem->getItem().id);
                    systems.hud().displayMessage(
                        core::item::Item::getUseLine(selectedItem->getItem().id),
                        std::bind(&BagMenu::messageDone, this));
                }
                else {
                    systems.hud().displayMessage("It won't have any effect",
                                                 std::bind(&BagMenu::messageDone, this));
                }
                state = MenuState::ShowingMessage;
                break;
            case core::item::Type::Useless:
                systems.hud().displayMessage(
                    "The Professor's voice echos in your head: \"That thing is completely useless! "
                    "What are you expecting to happen bro?\"",
                    std::bind(&BagMenu::messageDone, this));
                state = MenuState::ShowingMessage;
                break;
            default:
                systems.hud().displayMessage(
                    "The Professor's voice echos in your head: \"You can't use that here bro\"",
                    std::bind(&BagMenu::messageDone, this));
                state = MenuState::ShowingMessage;
                break;
            }
        }
    }
}

void BagMenu::giveItem() {
    state = MenuState::ChoosingGive;
    systems.engine().pushState(
        PeoplemonMenu::create(systems, PeoplemonMenu::Context::GiveItem, -1, itemPeoplemon));
}

void BagMenu::dropItem() {
    state = MenuState::ShowingMessage;
    systems.hud().getQty("How many " + core::item::Item::getName(selectedItem->getItem().id) +
                             "s should be dropped?",
                         0,
                         selectedItem->getItem().qty,
                         std::bind(&BagMenu::doDrop, this, std::placeholders::_1));
}

void BagMenu::doDrop(int qty) {
    removeAndUpdateItem(qty);
    resetAction();
}

void BagMenu::resetAction() {
    actionOpen = false;
    inputDriver.drive(activeMenu);
}

void BagMenu::itemHighlighted(const menu::BagItemButton* but) {
    const auto i = but->getItem().id;
    if (i != core::item::Id::None) {
        const std::string& d = core::item::Item::getDescription(i);
        description.setString(d);
        bl::interface::wordWrap(description, 607.f);
    }
    else {
        description.setString("Close the bag");
    }
}

void BagMenu::beginSlide(bool l) {
    slideOut = activeMenu;
    slideOff = 0.f;
    slideVel = activeMenu->getBounds().width / SlideTime;
    if (l) slideVel = -slideVel;
    inputDriver.drive(nullptr);
    state = MenuState::Sliding;
}

void BagMenu::messageDone() {
    state = MenuState::Browsing;
    resetAction();
}

void BagMenu::removeAndUpdateItem(int qty) {
    core::player::Bag::Item it = selectedItem->getItem();
    systems.player().state().bag.removeItem(it.id, qty);
    it.qty = systems.player().state().bag.itemCount(it.id);
    if (it.qty <= 0) { activeMenu->removeItem(selectedItem); }
    else {
        selectedItem->update(it);
    }
}

} // namespace state
} // namespace game
