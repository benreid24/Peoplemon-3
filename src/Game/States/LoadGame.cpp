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
    saveMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {22.f, 8.f, 8.f, 8.f});
    saveMenu.setMinHeight(30.f);
    saveMenu.setPosition({170.f, 200.f});
    saveMenu.setMaximumSize({-1.f, 250.f});

    cover.setFillColor(sf::Color::Transparent);
    cover.setSize({static_cast<float>(core::Properties::WindowWidth()),
                   static_cast<float>(core::Properties::WindowHeight())});

    Item::Ptr load = TextItem::create("Load", core::Properties::MenuFont(), sf::Color::Black, 26);
    load->getSignal(Item::Activated).willAlwaysCall([this]() {
        fadeTime = 0.f;
        inputDriver.drive(nullptr);
        state = Fading;
    });
    Item::Ptr del = TextItem::create("Delete", core::Properties::MenuFont(), sf::Color::Black, 26);
    del->getSignal(Item::Activated).willAlwaysCall([this]() { state = SaveDeleted; });
    Item::Ptr back =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(140, 0, 0), 26);
    back->getSignal(Item::Activated).willAlwaysCall([this]() {
        inputDriver.processImmediate(core::component::Command::Back);
    });
    actionMenu.setMinHeight(28.f);
    actionMenu.setRootItem(load);
    actionMenu.addItem(del, load.get(), Item::Bottom);
    actionMenu.addItem(back, del.get(), Item::Bottom);
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {22.f, 2.f, 2.f, 0.f});
}

const char* LoadGame::name() const { return "LoadGame"; }

void LoadGame::activate(bl::engine::Engine&) {
    saves.clear();
    core::file::GameSave::listSaves(saves);

    Item::Ptr item =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(140, 0, 0), 26);
    item->getSignal(Item::Activated).willAlwaysCall([this]() {
        inputDriver.processImmediate(core::component::Command::Back);
    });
    saveMenu.setRootItem(item);

    for (unsigned int i = 0; i < saves.size(); ++i) {
        const auto& save = saves[i];

        Item::Ptr it =
            TextItem::create(save.saveName, core::Properties::MenuFont(), sf::Color::Black, 26);
        it->getSignal(Item::Activated).willAlwaysCall([this, i]() { saveSelected(i); });
        saveMenu.addItem(it, item.get(), Item::Top);
        item = it;
    }
    saveMenu.setSelectedItem(item.get());

    state = SelectingSave;
    inputDriver.drive(&saveMenu);
    systems.player().inputSystem().addListener(inputDriver);
}

void LoadGame::deactivate(bl::engine::Engine&) {
    systems.player().inputSystem().removeListener(inputDriver);
}

void LoadGame::update(bl::engine::Engine& engine, float dt) {
    systems.player().update();

    const core::component::Command input = inputDriver.mostRecentInput();
    switch (state) {
    case SelectingSave:
        if (input == core::component::Command::Back) {
            bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
            engine.popState();
        }
        break;

    case ChooseAction:
        if (input == core::component::Command::Back) {
            state = SelectingSave;
            bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
            inputDriver.drive(&saveMenu);
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
        if (fadeTime >= FadeTime) {
            if (saves[selectedSave].load(&engine.eventBus())) {
                systems.engine().replaceState(MainGame::create(systems));
            }
            else {
                systems.hud().displayMessage("Failed to load game save",
                                             std::bind(&LoadGame::errorDone, this));
                inputDriver.drive(nullptr);
                state = Error;
            }
        }
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

void LoadGame::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();

    engine.window().draw(background);
    if (state != SelectingSave) { actionMenu.render(engine.window()); }
    saveMenu.render(engine.window());
    if (state == Error) { systems.hud().render(engine.window(), lag); }
    if (state == Fading) { engine.window().draw(cover); }

    engine.window().display();
}

void LoadGame::saveSelected(unsigned int i) {
    selectedSave = i;
    state        = ChooseAction;
    inputDriver.drive(&actionMenu);
    const sf::Vector2f pos(170.f + saveMenu.getBounds().width + 40.f,
                           200.f + static_cast<float>(i) * 26.f + 32.f);
    actionMenu.setPosition(pos + sf::Vector2f(22.f, 14.f));
}

void LoadGame::errorDone() { activate(systems.engine()); }

} // namespace state
} // namespace game
