#include <Game/States/PauseMenu.hpp>

#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <Core/Properties.hpp>
#include <Game/States/BagMenu.hpp>
#include <Game/States/PeoplemonMenu.hpp>
#include <Game/States/SaveGame.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float Width  = 200.f;
constexpr float Height = 385.f;
} // namespace

bl::engine::State::Ptr PauseMenu::create(core::system::Systems& systems) {
    return bl::engine::State::Ptr(new PauseMenu(systems));
}

PauseMenu::PauseMenu(core::system::Systems& s)
: State(s)
, menu(bl::menu::ArrowSelector::create(14.f, sf::Color::Black)) {
    using bl::menu::Item;
    using bl::menu::TextItem;

    resume = TextItem::create("Resume", core::Properties::MenuFont());
    resume->getSignal(Item::Activated).willCall([this]() { this->systems.engine().popState(); });

    ppldex = TextItem::create("Peopledex", core::Properties::MenuFont());
    ppldex->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Peopledex"; });

    pplmon = TextItem::create("Peoplemon", core::Properties::MenuFont());
    pplmon->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(
            PeoplemonMenu::create(systems, PeoplemonMenu::Context::PauseMenu));
    });

    bag = TextItem::create("Bag", core::Properties::MenuFont());
    bag->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(BagMenu::create(systems, BagMenu::Context::PauseMenu));
    });

    map = TextItem::create("Map", core::Properties::MenuFont());
    map->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Map"; });

    save = TextItem::create("Save", core::Properties::MenuFont());
    save->getSignal(Item::Activated).willCall([this]() {
        systems.engine().pushState(SaveGame::create(systems));
    });

    settings = TextItem::create("Settings", core::Properties::MenuFont());
    settings->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Settings"; });

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
    else {
        menu.addItem(bag, ppldex.get(), Item::Bottom);
    }
    menu.addItem(map, bag.get(), Item::Bottom);
    menu.addItem(save, map.get(), Item::Bottom);
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
    systems.player().inputSystem().addListener(inputDriver);
    inputDriver.drive(&menu);
    view = engine.window().getView();
    const sf::Vector2f size(core::Properties::WindowWidth(), core::Properties::WindowHeight());
    view.setCenter(size * 0.5f);
    view.setSize(size);
    systems.hud().hideEntryCard();
    // TODO - get sound specific to menu opening/closing
    bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveSound());
}

void PauseMenu::deactivate(bl::engine::Engine&) {
    systems.player().inputSystem().removeListener(inputDriver);
    inputDriver.drive(nullptr);
}

void PauseMenu::update(bl::engine::Engine&, float dt) {
    systems.player().update();
    systems.world().update(dt);
    const core::component::Command input = inputDriver.mostRecentInput();
    if (input == core::component::Command::Back || input == core::component::Command::Pause) {
        bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
        systems.engine().popState();
    }
}

void PauseMenu::render(bl::engine::Engine&, float lag) {
    systems.render().render(systems.engine().window(), systems.world().activeMap(), lag);

    const sf::View oldView = systems.engine().window().getView();
    systems.engine().window().setView(view);
    menu.render(systems.engine().window());
    systems.engine().window().setView(oldView);

    systems.engine().window().display();
}

} // namespace state
} // namespace game
