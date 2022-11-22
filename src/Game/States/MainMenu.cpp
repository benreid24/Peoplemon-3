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
using namespace core::input;

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
    menu.setPosition(
        {core::Properties::WindowSize().x * 0.15f, core::Properties::WindowSize().y * 0.25f});

    const sf::Vector2f HintPosition(core::Properties::WindowSize().x * 0.15f,
                                    core::Properties::WindowSize().y * 0.63f);
    const sf::Vector2f HintPadding{4.f, 4.f};
    const auto& a = systems.engine().inputSystem().getActor();
    hint.setFont(core::Properties::MenuFont());
    hint.setFillColor(sf::Color::Black);
    hint.setCharacterSize(16);
    hint.setString("Controls:\nMove up: " + a.getKBMTriggerControl(Control::MoveUp).toString() +
                   "\nMove right: " + a.getKBMTriggerControl(Control::MoveRight).toString() +
                   "\nMove down: " + a.getKBMTriggerControl(Control::MoveDown).toString() +
                   "\nMove left: " + a.getKBMTriggerControl(Control::MoveLeft).toString() +
                   "\nSprint: " + a.getKBMTriggerControl(Control::Sprint).toString() +
                   "\nInteract: " + a.getKBMTriggerControl(Control::Interact).toString() +
                   "\nBack: " + a.getKBMTriggerControl(Control::Back).toString() +
                   "\nPause: " + a.getKBMTriggerControl(Control::Pause).toString());
    hint.setPosition(HintPosition + HintPadding);
    const sf::Vector2f hintSize(hint.getGlobalBounds().width + hint.getLocalBounds().left,
                                hint.getGlobalBounds().height + hint.getLocalBounds().top);

    hintBox.setSize(hintSize + HintPadding * 2.f);
    hintBox.setPosition(HintPosition);
    hintBox.setFillColor(sf::Color(252, 248, 212));
    hintBox.setOutlineColor(sf::Color::Black);
    hintBox.setOutlineThickness(2.f);
}

const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::activate(bl::engine::Engine& engine) {
    // TODO - music
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));
    inputDriver.drive(&menu);
    systems.engine().inputSystem().getActor().addListener(*this);
    hintTimer.restart();
}

void MainMenu::deactivate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().popCamera();
    systems.engine().inputSystem().getActor().removeListener(*this);
}

void MainMenu::update(bl::engine::Engine&, float) {}

void MainMenu::render(bl::engine::Engine& engine, float) {
    sf::RenderWindow& w = engine.window();

    w.clear();
    w.draw(background);
    menu.render(w);
    if (hintTimer.getElapsedTime().asSeconds() >= 5.f) {
        w.draw(hintBox);
        w.draw(hint);
    }
    w.display();
}

bool MainMenu::observe(const bl::input::Actor&, unsigned int activatedControl,
                       bl::input::DispatchType, bool eventTriggered) {
    inputDriver.sendControl(activatedControl, eventTriggered);
    hintTimer.restart();
    return true;
}

} // namespace state
} // namespace game
