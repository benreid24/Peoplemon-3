#include <Game/States/MainMenu.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Files/Util.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>
#include <Game/States/NewGame.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr MainMenu::create(core::system::Systems& systems) {
    return Ptr(new MainMenu(systems));
}

MainMenu::MainMenu(core::system::Systems& systems)
: State(systems) {
    using bl::menu::Item;
    using bl::menu::TextRenderItem;

    backgroundTxtr =
        bl::engine::Resources::textures()
            .load(bl::file::Util::joinPath(core::Properties::MenuImagePath(), "mainMenu.png"))
            .data;
    background.setTexture(*backgroundTxtr, true);

    sf::Text sfText("New Game", core::Properties::MenuFont());
    sfText.setFillColor(sf::Color::Black);
    sfText.setCharacterSize(32);
    sfText.setStyle(sf::Text::Bold);

    newGame = Item::create(TextRenderItem::create(sfText));
    newGame->getSignal(Item::Activated).willCall([&systems]() {
        BL_LOG_INFO << "New Game selected";
        systems.engine().replaceState(NewGame::create(systems));
    });

    sfText.setString("Load Game");
    loadGame = Item::create(TextRenderItem::create(sfText));
    loadGame->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Load Game selected"; });

    sfText.setString("Settings");
    settings = Item::create(TextRenderItem::create(sfText));
    settings->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Settings selected"; });

    sfText.setString("Quit");
    quit = Item::create(TextRenderItem::create(sfText));
    quit->getSignal(Item::Activated).willCall([this]() {
        BL_LOG_INFO << "Quit selected";
        this->systems.engine().flags().set(bl::engine::Flags::PopState);
    });

    newGame->attach(loadGame, Item::Bottom);
    loadGame->attach(settings, Item::Bottom);
    settings->attach(quit, Item::Bottom);

    selector = bl::menu::ArrowSelector::create(14.f);
    selector->getArrow().setFillColor(sf::Color::Black);
    menu = std::make_shared<bl::menu::Menu>(newGame, selector);
    inputDriver.drive(*menu);
    renderer.setUniformSize({0.f, 45.f});
}

const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::activate(bl::engine::Engine& engine) {
    // TODO - music
    menu->setSelectedItem(newGame);
    systems.player().inputSystem().addListener(inputDriver);
}

void MainMenu::deactivate(bl::engine::Engine& engine) {
    systems.player().inputSystem().removeListener(inputDriver);
}

void MainMenu::update(bl::engine::Engine& engine, float dt) { systems.player().update(); }

void MainMenu::render(bl::engine::Engine& engine, float lag) {
    sf::RenderWindow& w = engine.window();
    w.clear();
    w.draw(background);
    menu->render(renderer, w, {w.getView().getSize().x * 0.15f, w.getView().getSize().y * 0.25f});
    w.display();
}

} // namespace state
} // namespace game
