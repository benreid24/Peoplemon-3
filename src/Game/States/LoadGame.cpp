#include <Game/States/LoadGame.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>
#include <Game/States/MainGame.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float FadeTime = 1.5f;
}

using namespace bl::menu;

LoadGame::Ptr LoadGame::create(core::system::Systems& s) { return Ptr(new LoadGame(s)); }

LoadGame::LoadGame(core::system::Systems& s)
: State(s)
, state(SelectingSave)
, selectedSave(0)
, saveMenu(ArrowSelector::create(12.f, sf::Color::Black))
, actionMenu(ArrowSelector::create(12.f, sf::Color::Black)) {
    bgndTxtr = bl::engine::Resources::textures()
                   .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                      "LoadGame/loadGameBgnd.png"))
                   .data;
    background.setTexture(*bgndTxtr, true);
    menuBackground.setFillColor(sf::Color::White);
    menuBackground.setOutlineColor(sf::Color::Black);
    menuBackground.setOutlineThickness(3.f);
    menuBackground.setPosition(150.f, 200.f);

    saveMenu.setPosition({170.f, 216.f});
    saveMenu.setMinHeight(30.f);

    cover.setFillColor(sf::Color::Transparent);
    cover.setSize({static_cast<float>(core::Properties::WindowWidth()),
                   static_cast<float>(core::Properties::WindowHeight())});

    Item::Ptr load = TextItem::create("Load", core::Properties::MenuFont(), sf::Color::Black, 26);
    load->getSignal(Item::Activated).willAlwaysCall([this]() { state = SaveLoaded; });
    Item::Ptr del = TextItem::create("Delete", core::Properties::MenuFont(), sf::Color::Black, 26);
    del->getSignal(Item::Activated).willAlwaysCall([this]() { state = SaveDeleted; });
    Item::Ptr back =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(90, 0, 0), 26);
    back->getSignal(Item::Activated).willAlwaysCall([this]() {
        inputDriver.processImmediate(core::component::Command::Back);
    });
    actionMenu.setMinHeight(30.f);
    actionMenu.setRootItem(load);
    actionMenu.addItem(del, load.get(), Item::Bottom);
    actionMenu.addItem(back, del.get(), Item::Bottom);
    actionBackground.setFillColor(sf::Color::White);
    actionBackground.setOutlineColor(sf::Color::Black);
    actionBackground.setOutlineThickness(3.f);
    actionBackground.setSize(
        {actionMenu.getBounds().width + 30.f, actionMenu.getBounds().height + 24.f});
}

const char* LoadGame::name() const { return "LoadGame"; }

void LoadGame::activate(bl::engine::Engine&) {
    saves.clear();
    core::file::GameSave::listSaves(saves);

    Item::Ptr item =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(90, 0, 0), 26);
    item->getSignal(Item::Activated).willAlwaysCall([this]() {
        inputDriver.processImmediate(core::component::Command::Back);
    });
    saveMenu.setRootItem(item);

    for (unsigned int i = 0; i < saves.size(); ++i) {
        const auto& save = saves[i];

        Item::Ptr it =
            TextItem::create(save.saveName, core::Properties::MenuFont(), sf::Color::Black, 26);
        it->getSignal(Item::Activated).willAlwaysCall([this, i]() { saveSelected(i); });
        saveMenu.addItem(it, item.get(), Item::Bottom);
        item = it;
    }

    state = SelectingSave;
    menuBackground.setSize({saveMenu.getBounds().width + 30.f, saveMenu.getBounds().height + 32.f});
    inputDriver.drive(&saveMenu);
    systems.player().inputSystem().addListener(inputDriver);
}

void LoadGame::deactivate(bl::engine::Engine&) {
    systems.player().inputSystem().removeListener(inputDriver);
}

void LoadGame::update(bl::engine::Engine& engine, float dt) {
    systems.player().update();

    switch (state) {
    case SelectingSave:
        if (inputDriver.backPressed()) { engine.popState(); }
        break;

    case ChooseAction:
        if (inputDriver.backPressed()) {
            state = SelectingSave;
            inputDriver.drive(&saveMenu);
        }
        break;

    case SaveLoaded:
        if (saves[selectedSave].load(engine.eventBus())) {
            fadeTime = 0.f;
            state    = Fading;
        }
        else {
            systems.hud().displayMessage("Failed to load game save",
                                         std::bind(&LoadGame::errorDone, this));
            inputDriver.drive(nullptr);
            state = Error;
        }
        break;

    case SaveDeleted:
        if (saves[selectedSave].remove()) { activate(engine); }
        else {
            systems.hud().displayMessage("Failed to delete game save",
                                         std::bind(&LoadGame::errorDone, this));
            inputDriver.drive(nullptr);
            state = Error;
        }
        break;

    case Fading:
        fadeTime += dt;
        cover.setFillColor(sf::Color(0, 0, 0, std::min(255.f, 255.f * (fadeTime / FadeTime))));
        if (fadeTime >= FadeTime) { systems.engine().replaceState(MainGame::create(systems)); }
        break;

    case Error:
        systems.hud().update(dt);
        break;

    default:
        state = SelectingSave;
        inputDriver.drive(&saveMenu);
        break;
    }
}

void LoadGame::render(bl::engine::Engine& engine, float) {
    engine.window().clear();

    engine.window().draw(background);
    engine.window().draw(menuBackground);
    if (state != SelectingSave) {
        engine.window().draw(actionBackground);
        actionMenu.render(engine.window());
    }
    saveMenu.render(engine.window());
    if (state == Fading) { engine.window().draw(cover); }

    engine.window().display();
}

void LoadGame::saveSelected(unsigned int i) {
    selectedSave = i;
    state        = ChooseAction;
    inputDriver.drive(&actionMenu);
    const sf::Vector2f pos(menuBackground.getPosition().x + menuBackground.getGlobalBounds().width +
                               40.f,
                           menuBackground.getPosition().y + static_cast<float>(i) * 26.f + 32.f);
    actionBackground.setPosition(pos);
    actionMenu.setPosition(pos + sf::Vector2f(22.f, 14.f));
}

void LoadGame::errorDone() { activate(systems.engine()); }

} // namespace state
} // namespace game
