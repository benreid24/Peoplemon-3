#include <Game/States/MainMenu.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Files/Util.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>
#include <Game/States/MapExplorer.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr MainMenu::create(core::systems::Systems& systems) {
    return Ptr(new MainMenu(systems));
}

MainMenu::MainMenu(core::systems::Systems& systems)
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
    newGame->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "New Game selected"; });

    sfText.setString("Load Game");
    loadGame = Item::create(TextRenderItem::create(sfText));
    loadGame->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Load Game selected"; });

    sfText.setString("Settings");
    settings = Item::create(TextRenderItem::create(sfText));
    settings->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Settings selected"; });

    sfText.setString("Quit");
    quit = Item::create(TextRenderItem::create(sfText));
    quit->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Quit selected"; });

    newGame->attach(loadGame, Item::Bottom);
    loadGame->attach(settings, Item::Bottom);
    settings->attach(quit, Item::Bottom);

    selector = bl::menu::ArrowSelector::create(14.f);
    selector->getArrow().setFillColor(sf::Color::Black);
    menu = std::make_shared<bl::menu::Menu>(newGame, selector);

    keyboardEventGenerator = std::make_shared<bl::menu::KeyboardEventGenerator>(*menu);
    mouseEventGenerator    = std::make_shared<bl::menu::MouseEventGenerator>(*menu);
}

const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::activate(bl::engine::Engine& engine) {
    // TODO - music
    menu->setSelectedItem(newGame);
    engine.eventBus().subscribe(keyboardEventGenerator.get());
    engine.eventBus().subscribe(mouseEventGenerator.get());
    quit->getSignal(bl::menu::Item::Activated).clear();
    quit->getSignal(bl::menu::Item::Activated).willCall([&engine]() {
        BL_LOG_INFO << "Quit selected";
        engine.flags().set(bl::engine::Flags::PopState);
    });
}

void MainMenu::deactivate(bl::engine::Engine& engine) {
    engine.eventBus().unsubscribe(keyboardEventGenerator.get());
    engine.eventBus().unsubscribe(mouseEventGenerator.get());
}

void MainMenu::update(bl::engine::Engine& engine, float dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
        engine.replaceState(MapExplorer::create(systems, "WorldMap.map"));
    }
}

void MainMenu::render(bl::engine::Engine& engine, float lag) {
    sf::RenderWindow& w = engine.window();
    w.clear();
    w.draw(background);
    menu->render(renderer, w, {w.getView().getSize().x * 0.15f, w.getView().getSize().y * 0.25f});
    w.display();
}

} // namespace state
} // namespace game
