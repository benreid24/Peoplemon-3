#include <Game/States/MainMenu.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>
#include <Game/States/LoadGame.hpp>
#include <Game/States/NewGame.hpp>
#include <Game/States/SettingsMenu.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr MainMenu::create(core::system::Systems& systems) {
    return Ptr(new MainMenu(systems));
}

MainMenu::MainMenu(core::system::Systems& systems)
: State(systems)
, menu(bl::menu::ArrowSelector::create(14.f, sf::Color::Black)) {
    using bl::menu::Item;
    using bl::menu::TextItem;

    backgroundTxtr =
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "mainMenu.png"))
            .data;
    background.setTexture(*backgroundTxtr, true);

    newGame = TextItem::create("New Game", core::Properties::MenuFont(), sf::Color::Black, 32);
    newGame->getTextObject().setStyle(sf::Text::Bold);
    newGame->getSignal(Item::Activated).willCall([&systems]() {
        BL_LOG_INFO << "New Game selected";
        systems.engine().replaceState(NewGame::create(systems));
    });

    loadGame = TextItem::create("Load Game", core::Properties::MenuFont(), sf::Color::Black, 32);
    loadGame->getTextObject().setStyle(sf::Text::Bold);
    loadGame->getSignal(Item::Activated).willCall([&systems]() {
        BL_LOG_INFO << "Load Game selected";
        systems.engine().pushState(LoadGame::create(systems));
    });

    settings = TextItem::create("Settings", core::Properties::MenuFont(), sf::Color::Black, 32);
    settings->getTextObject().setStyle(sf::Text::Bold);
    settings->getSignal(Item::Activated).willCall([&systems]() {
        BL_LOG_INFO << "Settings selected";
        systems.engine().pushState(SettingsMenu::create(systems));
    });

    quit = TextItem::create("Quit", core::Properties::MenuFont(), sf::Color::Black, 32);
    quit->getTextObject().setStyle(sf::Text::Bold);
    quit->getSignal(Item::Activated).willCall([this]() {
        BL_LOG_INFO << "Quit selected";
        this->systems.engine().flags().set(bl::engine::Flags::PopState);
    });

    menu.setRootItem(newGame);
    menu.addItem(loadGame, newGame.get(), Item::Bottom);
    menu.addItem(settings, loadGame.get(), Item::Bottom);
    menu.addItem(quit, settings.get(), Item::Bottom);
    menu.setMinHeight(38.f);
    menu.setSelectedItem(newGame.get());
}

const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::activate(bl::engine::Engine& engine) {
    // TODO - music
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));
    inputDriver.drive(&menu);
    systems.engine().inputSystem().getActor().addListener(inputDriver);
}

void MainMenu::deactivate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().popCamera();
    systems.engine().inputSystem().getActor().removeListener(inputDriver);
    inputDriver.drive(nullptr);
}

void MainMenu::update(bl::engine::Engine&, float) {}

void MainMenu::render(bl::engine::Engine& engine, float) {
    sf::RenderWindow& w = engine.window();
    menu.setPosition({w.getView().getSize().x * 0.15f, w.getView().getSize().y * 0.25f});

    w.clear();
    w.draw(background);
    menu.render(w);
    w.display();
}

} // namespace state
} // namespace game
