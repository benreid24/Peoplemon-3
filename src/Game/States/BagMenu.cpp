#include <Game/States/BagMenu.hpp>

#include <BLIB/Interfaces/Utilities.hpp>
#include <Core/Events/Maps.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
namespace
{
using ExitCb = std::function<void()>;
using ItemCb = std::function<void(menu::BagItemButton*)>;

constexpr float SlideTime                   = 0.5f;
int lastTab                                 = -1;
int lastPosPerTab[4]                        = {0, 0, 0, 0};
const core::item::Category tabCategories[4] = {core::item::Category::Regular,
                                               core::item::Category::Peopleball,
                                               core::item::Category::TM,
                                               core::item::Category::Key};
const char* tabTitles[4]                    = {"Regular Items", "Peopleballs", "TM's", "Key Items"};
constexpr float MenuX                       = 365.f;
constexpr float MenuY                       = 30.f;
constexpr float MenuWidth                   = 319.f;
constexpr float MenuHeight                  = 385.f;

void populateMenu(bl::menu::Menu& menu, const std::vector<core::player::Bag::Item>& items,
                  const ExitCb& ecb, const ItemCb& icb, const ItemCb& dcb, int ti) {
    menu::BagItemButton::Ptr exit = menu::BagItemButton::create({core::item::Id::None, 0});
    exit->getSignal(bl::menu::Item::Activated).willAlwaysCall(ecb);
    exit->getSignal(bl::menu::Item::Selected).willAlwaysCall(std::bind(dcb, exit.get()));
    menu.setRootItem(exit);
    menu.setMoveFailSound(bl::audio::AudioSystem::InvalidHandle);

    menu::BagItemButton* b        = exit.get();
    menu::BagItemButton* selectMe = exit.get();
    int i                         = items.size() - 1;
    for (auto rit = items.rbegin(); rit != items.rend(); ++rit) {
        menu::BagItemButton::Ptr nb = menu::BagItemButton::create(*rit);
        nb->getSignal(bl::menu::Item::Activated).willAlwaysCall(std::bind(icb, nb.get()));
        menu.addItem(nb, b, bl::menu::Item::Top);
        auto* nbp = nb.get();
        nb->getSignal(bl::menu::Item::Selected).willAlwaysCall([nbp, i, ti, dcb]() {
            lastPosPerTab[ti] = i;
            dcb(nbp);
        });

        b = nb.get();
        if (lastPosPerTab[ti] == i) { selectMe = nb.get(); }
        --i;
    }
    if (b != exit.get()) { menu.attachExisting(exit.get(), b, bl::menu::Item::Top); }
    menu.setSelectedItem(selectMe);
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
                                       int outNow, int* chosenPpl, bool* up) {
    return bl::engine::State::Ptr(new BagMenu(s, c, i, outNow, chosenPpl, up));
}

BagMenu::BagMenu(core::system::Systems& s, Context c, core::item::Id* i, int outNow, int* chosenPpl,
                 bool* up)
: State(s, bl::engine::StateMask::Menu)
, context(c)
, outNow(outNow)
, result(i)
, itemPeoplemon(chosenPpl ? chosenPpl : &selectedPeoplemon)
, unpause(up)
, state(MenuState::Browsing)
, activeMenu(&regularMenu)
, actionOpen(false) {
    actionMenu.create(s.engine(),
                      s.engine().renderer().getObserver(),
                      bl::menu::ArrowSelector::create(10.f, sf::Color::Black));
    actionMenu.setDepth(bl::cam::OverlayCamera::MinDepth);
    regularMenu.create(
        s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());
    ballMenu.create(
        s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());
    keyMenu.create(s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());
    tmMenu.create(s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());

    bgndTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "Bag/background.png"));
    background.create(s.engine(), bgndTxtr);

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

    pocketLabel.create(s.engine(), core::Properties::MenuFont(), "", 27, sf::Color(0, 20, 75));
    pocketLabel.getTransform().setPosition(72.f, 342.f);
    pocketLabel.setParent(background);

    description.create(s.engine(), core::Properties::MenuFont(), "", 20, sf::Color::Black);
    description.getTransform().setPosition(98.f, 473.f);
    description.wordWrap(607.f);
    description.setParent(background);

    menuTabs[0] = &regularMenu;
    menuTabs[1] = &ballMenu;
    menuTabs[2] = &tmMenu;
    menuTabs[3] = &keyMenu;

    for (int i = 0; i < 4; ++i) {
        menuTabs[i]->configureBackground(
            sf::Color::Transparent, sf::Color::Transparent, 0.f, {0.f, 0.f, 0.f, 0.f});
        menuTabs[i]->setPosition({MenuX, MenuY});
        menuTabs[i]->setMaximumSize({-1.f, MenuHeight});
        menuTabs[i]->setScissor(sf::IntRect(MenuX, MenuY, MenuWidth, MenuHeight));
    }
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
        else { toDrive = &actionMenu; }
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
        if (result) *result = core::item::Id::None;

        std::vector<core::player::Bag::Item> items;
        for (int i = 0; i < 4; ++i) {
            systems.player().state().bag.getByCategory(tabCategories[i], items);
            populateMenu(*menuTabs[i],
                         items,
                         std::bind(&BagMenu::exitSelected, this),
                         std::bind(&BagMenu::itemSelected, this, std::placeholders::_1),
                         std::bind(&BagMenu::itemHighlighted, this, std::placeholders::_1),
                         i);
        }

        state      = MenuState::Browsing;
        actionOpen = false;
        activeMenu = lastTab >= 0 ? menuTabs[lastTab] : &regularMenu;
        toDrive    = activeMenu;
        pocketLabel.getSection().setString(tabTitles[lastTab >= 0 ? lastTab : 0]);
        if (lastTab < 0) lastTab = 0;
    }

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    pocketLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    description.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    actionMenu.addToOverlay(background.entity());
    actionMenu.setHidden(true);
    for (unsigned int i = 0; i < std::size(menuTabs); ++i) {
        menuTabs[i]->addToOverlay(background.entity());
        menuTabs[i]->setHidden(menuTabs[i] != activeMenu);
    }

    engine.inputSystem().getActor().addListener(*this);
    inputDriver.drive(toDrive);
}

void BagMenu::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
    engine.inputSystem().getActor().removeListener(*this);
}

void BagMenu::update(bl::engine::Engine& engine, float dt, float) {
    switch (state) {
    case MenuState::Sliding: {
        slideOff += slideVel * dt;

        const float m  = slideVel > 0.f ? -1.f : 1.f;
        const float x  = MenuX + slideOff;
        const float wf = MenuWidth * m + x;
        activeMenu->setPosition({wf, MenuY});
        slideOut->setPosition({x, MenuY});

        if (std::abs(slideOff) >= activeMenu->getBounds().width) {
            state = MenuState::Browsing;
            activeMenu->setPosition({MenuX, MenuY});
            inputDriver.drive(activeMenu);
            slideOut->setHidden(true);
        }
    } break;

    case MenuState::ImmediatelyPop:
        engine.popState();
        break;

    case MenuState::ShowingMessage:
    case MenuState::Browsing:
    default:
        break;
    }
}

bool BagMenu::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                      bool fromEvent) {
    inputDriver.sendControl(ctrl, fromEvent);

    if (state == MenuState::Browsing && fromEvent) {
        if (actionOpen) {
            if (ctrl == core::input::Control::Back) {
                bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
                resetAction();
            }
        }
        else {
            switch (ctrl) {
            case core::input::Control::Back:
                systems.engine().popState();
                break;

            case core::input::Control::MoveLeft:
                beginSlide(true);
                lastTab    = lastTab > 0 ? lastTab - 1 : 3;
                activeMenu = menuTabs[lastTab];
                activeMenu->setHidden(false);
                pocketLabel.getSection().setString(tabTitles[lastTab]);
                itemHighlighted(
                    dynamic_cast<const menu::BagItemButton*>(activeMenu->getSelectedItem()));
                break;

            case core::input::Control::MoveRight:
                beginSlide(false);
                lastTab    = lastTab < 3 ? lastTab + 1 : 0;
                activeMenu = menuTabs[lastTab];
                activeMenu->setHidden(false);
                pocketLabel.getSection().setString(tabTitles[lastTab]);
                itemHighlighted(
                    dynamic_cast<const menu::BagItemButton*>(activeMenu->getSelectedItem()));
                break;

            default:
                break;
            }
        }
    }

    return true;
}

void BagMenu::itemSelected(const menu::BagItemButton* but) {
    selectedItem = const_cast<menu::BagItemButton*>(but);
    actionOpen   = true;
    actionMenu.setHidden(false);
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
            state = MenuState::ImmediatelyPop;
        }
        else {
            switch (type) {
            case core::item::Type::KeyItem:
                state = MenuState::ShowingMessage;
                switch (selectedItem->getItem().id) {
                case core::item::Id::TransportationCrystal:
                    if (systems.player().state().bag.hasItem(core::item::Id::Penny)) {
                        systems.hud().promptUser(
                            "Use your Penny to use the Transportation Crystal?",
                            {"Yes", "No"},
                            std::bind(&BagMenu::keyItemConfirmUse, this, std::placeholders::_1));
                    }
                    else {
                        systems.hud().displayMessage("You need a Penny to use this!",
                                                     std::bind(&BagMenu::messageDone, this));
                    }
                    break;
                case core::item::Id::Lantern:
                    if (systems.world().activeMap().lightingSystem().lightsAreOn()) {
                        systems.player().showLantern();
                        if (unpause != nullptr) { *unpause = true; }
                        state = MenuState::ImmediatelyPop;
                    }
                    else {
                        systems.hud().displayMessage(
                            "It's not very dark here. Better save the batteries for later.",
                            std::bind(&BagMenu::messageDone, this));
                    }
                    break;
                case core::item::Id::BigRedButton:
                    systems.hud().promptUser(
                        "This will make all trainers hostile again! Are you sure you want to do "
                        "this? This cannot be undone except through violence.",
                        {"Yes", "No"},
                        std::bind(&BagMenu::keyItemConfirmUse, this, std::placeholders::_1));
                    break;
                case core::item::Id::MiniFridge:
                    systems.hud().promptUser(
                        "Blow yourself up and respawn at the last PC Center you visited?",
                        {"Yes", "No"},
                        std::bind(&BagMenu::keyItemConfirmUse, this, std::placeholders::_1));
                    break;
                default:
                    systems.hud().displayMessage(
                        "The Professor's voice rings in your head: \"Not now I'm working!\"");
                    systems.hud().displayMessage("... Or something like that.",
                                                 std::bind(&BagMenu::messageDone, this));
                    break;
                }
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
                systems.hud().displayMessage("The Professor's voice echoes in your head: \"That "
                                             "thing is completely useless! "
                                             "What are you expecting to happen bro?\"",
                                             std::bind(&BagMenu::messageDone, this));
                state = MenuState::ShowingMessage;
                break;
            default:
                systems.hud().displayMessage(
                    "The Professor's voice echoes in your head: \"You can't use that here bro\"",
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
    actionMenu.setHidden(true);
}

void BagMenu::itemHighlighted(const menu::BagItemButton* but) {
    const auto i = but->getItem().id;
    if (i != core::item::Id::None) {
        const std::string& d = core::item::Item::getDescription(i);
        description.getSection().setString(d);
    }
    else { description.getSection().setString("Close the bag"); }
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
    else { selectedItem->update(it); }
}

void BagMenu::keyItemConfirmUse(const std::string& c) {
    messageDone();
    if (c == "Yes") {
        switch (selectedItem->getItem().id) {
        case core::item::Id::MiniFridge:
            systems.player().whiteout();
            if (unpause != nullptr) { *unpause = true; }
            state = MenuState::ImmediatelyPop;
            break;
        case core::item::Id::BigRedButton:
            systems.trainers().resetDefeated();
            systems.hud().displayMessage("All trainer Peoplemon have been healed! Watch your back.",
                                         std::bind(&BagMenu::messageDone, this));
            state = MenuState::ShowingMessage;
            break;
        case core::item::Id::TransportationCrystal:
            if (systems.player().state().bag.removeItem(core::item::Id::Penny)) {
                bl::event::Dispatcher::dispatch<core::event::SwitchMapTriggered>(
                    {"TheVoid.map", 1});
                if (unpause != nullptr) { *unpause = true; }
                state = MenuState::ImmediatelyPop;
            }
            else {
                systems.hud().displayMessage("What happened to your Penny?",
                                             std::bind(&BagMenu::messageDone, this));
                state = MenuState::ShowingMessage;
            }
            break;
        default:
            break;
        }
    }
}

} // namespace state
} // namespace game
