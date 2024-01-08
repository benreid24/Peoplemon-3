#include <Game/States/PauseMenu.hpp>

#include <Core/Properties.hpp>
#include <Game/States/BagMenu.hpp>
#include <Game/States/FlyMap.hpp>
#include <Game/States/Peopledex.hpp>
#include <Game/States/PeoplemonMenu.hpp>
#include <Game/States/SaveGame.hpp>
#include <Game/States/SettingsMenu.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr PauseMenu::create(core::system::Systems& systems) {
    return bl::engine::State::Ptr(new PauseMenu(systems));
}

PauseMenu::PauseMenu(core::system::Systems& s)
: State(s, bl::engine::StateMask::Paused)
, openedOnce(false)
, unpause(false) {
    using bl::menu::Item;
    using bl::menu::TextItem;

    menu.create(s.engine(),
                s.engine().renderer().getObserver(),
                bl::menu::ArrowSelector::create(14.f, sf::Color::Black));

    resume = TextItem::create("Resume", core::Properties::MenuFont());
    resume->getSignal(Item::Activated).willCall([this]() { this->systems.engine().popState(); });

    ppldex = TextItem::create("Peopledex", core::Properties::MenuFont());
    ppldex->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(Peopledex::create(systems));
    });

    pplmon = TextItem::create("Peoplemon", core::Properties::MenuFont());
    pplmon->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(
            PeoplemonMenu::create(systems, PeoplemonMenu::Context::PauseMenu));
    });

    bag = TextItem::create("Bag", core::Properties::MenuFont());
    bag->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(
            BagMenu::create(systems, BagMenu::Context::PauseMenu, nullptr, -1, nullptr, &unpause));
    });

    map = TextItem::create("Map", core::Properties::MenuFont());
    map->getSignal(Item::Activated).willCall([this]() {
        this->systems.engine().pushState(FlyMap::create(this->systems, this->unpause));
    });

    save = TextItem::create("Save", core::Properties::MenuFont());
    save->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(SaveGame::create(systems));
    });

    settings = TextItem::create("Settings", core::Properties::MenuFont());
    settings->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(SettingsMenu::create(systems));
    });

    quit = TextItem::create("Quit", core::Properties::MenuFont());
    quit->getSignal(Item::Activated).willCall([this]() {
        this->systems.engine().flags().set(bl::engine::Flags::Terminate);
    });

    menu.setRootItem(resume);
    menu.addItem(ppldex, resume.get(), Item::Bottom);
    if (!systems.player().state().peoplemon.empty()) {
        menu.addItem(pplmon, ppldex.get(), Item::Bottom);
        menu.addItem(bag, pplmon.get(), Item::Bottom);
    }
    else { menu.addItem(bag, ppldex.get(), Item::Bottom); }
    if (systems.player().state().bag.hasItem(core::item::Id::Map)) {
        menu.addItem(map, bag.get(), Item::Bottom);
        menu.addItem(save, map.get(), Item::Bottom);
    }
    else { menu.addItem(save, bag.get(), Item::Bottom); }
    menu.addItem(settings, save.get(), Item::Bottom);
    menu.addItem(quit, settings.get(), Item::Bottom);
    menu.attachExisting(resume.get(), quit.get(), Item::Bottom);

    const sf::Vector2f scr(core::Properties::WindowWidth(), core::Properties::WindowHeight());
    menu.setMinHeight(38.f);
    menu.setSelectedItem(resume.get());
    menu.configureBackground(sf::Color::White, sf::Color::Black, 4.f, {24.f, 8.f, 8.f, 8.f});
    menu.setPosition(
        {scr.x - menu.visibleSize().x - 32.f, scr.y * 0.5f - menu.visibleSize().y * 0.5f});
}

const char* PauseMenu::name() const { return "PauseMenu"; }

void PauseMenu::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);
    inputDriver.drive(&menu);
    inputDriver.resetDebounce();
    menu.addToOverlay();
    systems.hud().hideEntryCard();
    if (!openedOnce) {
        bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveSound());
        openedOnce = true;
    }
}

void PauseMenu::deactivate(bl::engine::Engine&) {
    menu.removeFromOverlay();
    systems.engine().inputSystem().getActor().removeListener(*this);
    inputDriver.drive(nullptr);
    unpause = false;
}

void PauseMenu::update(bl::engine::Engine& engine, float dt, float) {
    if (unpause) {
        engine.popState();
        return;
    }
}

bool PauseMenu::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                        bool fromEvent) {
    inputDriver.sendControl(ctrl, fromEvent);
    if (fromEvent) {
        if (ctrl == core::input::Control::Back || ctrl == core::input::Control::Pause) {
            bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
            systems.engine().popState();
        }
    }
    return true;
}

// void PauseMenu::render(bl::engine::Engine&, float lag) {
//     if (systems.flight().flying()) return;
//
//     systems.render().render(systems.engine().window(), systems.world().activeMap(), lag);
//     menu.render(systems.engine().window(), menuRenderStates);
//
//     systems.engine().window().display();
// }

} // namespace state
} // namespace game
