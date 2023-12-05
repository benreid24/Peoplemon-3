#include <Game/States/MainMenu.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
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
, hintTimer(0.f) {}

const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::activate(bl::engine::Engine& engine) {
    // TODO - music

    if (!backgroundTxtr) {
        using bl::menu::Item;
        using bl::menu::TextItem;

        menu.create(engine,
                    engine.renderer().getObserver(),
                    bl::menu::ArrowSelector::create(14.f, sf::Color::Black));
        backgroundTxtr = engine.renderer().texturePool().getOrLoadTexture(
            bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "mainMenu.png"));
        background.create(engine, backgroundTxtr);

        newGame = TextItem::create(
            "New Game", core::Properties::MenuFont(), sf::Color::Black, 32, sf::Text::Bold);
        newGame->getSignal(Item::Activated).willCall([this]() {
            BL_LOG_INFO << "New Game selected";
            systems.engine().replaceState(NewGame::create(systems));
        });

        loadGame = TextItem::create(
            "Load Game", core::Properties::MenuFont(), sf::Color::Black, 32, sf::Text::Bold);
        loadGame->getSignal(Item::Activated).willCall([this]() {
            BL_LOG_INFO << "Load Game selected";
            systems.engine().pushState(LoadGame::create(systems));
        });

        settings = TextItem::create(
            "Settings", core::Properties::MenuFont(), sf::Color::Black, 32, sf::Text::Bold);
        settings->getSignal(Item::Activated).willCall([this]() {
            BL_LOG_INFO << "Settings selected";
            systems.engine().pushState(SettingsMenu::create(systems));
        });

        quit = TextItem::create(
            "Quit", core::Properties::MenuFont(), sf::Color::Black, 32, sf::Text::Bold);
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

        const glm::vec2 HintPosition(core::Properties::WindowSize().x * 0.15f,
                                     core::Properties::WindowSize().y * 0.63f);
        const glm::vec2 HintPadding{4.f, 4.f};
        const auto& a = systems.engine().inputSystem().getActor();
        const sf::Color controlColor(3, 146, 168);
        hint.create(engine,
                    core::Properties::MenuFont(),
                    "Controls:",
                    18,
                    sf::Color::Black,
                    sf::Text::Bold);
        hint.addSection("\nMove up:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::MoveUp).toString(), 16, controlColor);
        hint.addSection("\nMove right:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::MoveRight).toString(), 16, controlColor);
        hint.addSection("\nMove down:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::MoveDown).toString(), 16, controlColor);
        hint.addSection("\nMove left:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::MoveLeft).toString(), 16, controlColor);
        hint.addSection("\nSprint:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::Sprint).toString(), 16, controlColor);
        hint.addSection("\nInteract:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::Interact).toString(), 16, controlColor);
        hint.addSection("\nBack:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::Back).toString(), 16, controlColor);
        hint.addSection("\nPause:", 16, sf::Color::Black);
        hint.addSection(a.getKBMTriggerControl(Control::Pause).toString(), 16, controlColor);

        hint.getTransform().setPosition(HintPadding);
        const sf::FloatRect hintBounds = hint.getLocalBounds();
        const glm::vec2 hintSize(hintBounds.width + hintBounds.left,
                                 hintBounds.height + hintBounds.top);

        hintBox.create(engine, hintSize + HintPadding * 2.f);
        hintBox.getOverlayScaler().positionInParentSpace({0.15f, 0.63f});
        hintBox.setFillColor(sf::Color(252, 248, 212));
        hintBox.setOutlineColor(sf::Color::Black);
        hintBox.setOutlineThickness(2.f);
        hint.setParent(hintBox);
        hintBox.setParent(background);
    }

    inputDriver.drive(&menu);
    systems.engine().inputSystem().getActor().addListener(*this);
    hintTimer = 0.f;
    hintBox.setHidden(true);

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hintBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hint.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    menu.addToOverlay(background.entity());
}

void MainMenu::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
    systems.engine().inputSystem().getActor().removeListener(*this);
}

void MainMenu::update(bl::engine::Engine&, float dt, float) {
    hintTimer += dt;
    if (hintTimer >= 5.f) { hintBox.setHidden(false); }
}

bool MainMenu::observe(const bl::input::Actor&, unsigned int activatedControl,
                       bl::input::DispatchType, bool eventTriggered) {
    inputDriver.sendControl(activatedControl, eventTriggered);
    hintTimer = 0.f;
    hintBox.setHidden(true);
    return true;
}

} // namespace state
} // namespace game
