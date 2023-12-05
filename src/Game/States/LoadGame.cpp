#include <Game/States/LoadGame.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
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
, selectedSave(0) {}

const char* LoadGame::name() const { return "LoadGame"; }

void LoadGame::activate(bl::engine::Engine& engine) {
    if (!bgndTxtr) {
        bgndTxtr = engine.renderer().texturePool().getOrLoadTexture(bl::util::FileUtil::joinPath(
            core::Properties::MenuImagePath(), "LoadGame/loadGameBgnd.png"));
        background.create(engine, bgndTxtr);

        saveMenu.create(
            engine, engine.renderer().getObserver(), ArrowSelector::create(12.f, sf::Color::Black));
        saveMenu.configureBackground(
            sf::Color::White, sf::Color::Black, 3.f, {22.f, 4.f, 4.f, 6.f});
        saveMenu.setMinHeight(30.f);
        saveMenu.setPosition({170.f, 200.f});
        saveMenu.setMaximumSize({-1.f, 250.f});

        // TODO - use fade post effect instead
        cover.create(engine, bgndTxtr->size());
        cover.setFillColor(sf::Color::Transparent);
        cover.getTransform().setDepth(bl::cam::OverlayCamera::MinDepth);

        actionMenu.create(
            engine, engine.renderer().getObserver(), ArrowSelector::create(12.f, sf::Color::Black));
        Item::Ptr load =
            TextItem::create("Load", core::Properties::MenuFont(), sf::Color::Black, 26);
        load->getSignal(Item::Activated).willAlwaysCall([this]() {
            fadeTime = 0.f;
            inputDriver.drive(nullptr);
            cover.setHidden(false);
            state = Fading;
        });
        Item::Ptr del =
            TextItem::create("Delete", core::Properties::MenuFont(), sf::Color::Black, 26);
        del->getSignal(Item::Activated).willAlwaysCall([this]() { state = SaveDeleted; });
        Item::Ptr back =
            TextItem::create("Back", core::Properties::MenuFont(), sf::Color(140, 0, 0), 26);
        back->getSignal(Item::Activated).willAlwaysCall([this]() {
            observe(systems.engine().inputSystem().getActor(),
                    core::input::Control::Back,
                    bl::input::DispatchType::TriggerActivated,
                    true);
        });
        actionMenu.setMinHeight(28.f);
        actionMenu.setRootItem(load);
        actionMenu.addItem(del, load.get(), Item::Bottom);
        actionMenu.addItem(back, del.get(), Item::Bottom);
        actionMenu.configureBackground(
            sf::Color::White, sf::Color::Black, 3.f, {22.f, 2.f, 2.f, 0.f});
    }

    saves.clear();
    core::file::GameSave::listSaves(saves);

    Item::Ptr item =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(140, 0, 0), 26);
    item->getSignal(Item::Activated).willAlwaysCall([this]() {
        observe(systems.engine().inputSystem().getActor(), core::input::Control::Back, bl::input::DispatchType::TriggerActivated, true);
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

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    cover.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    cover.setHidden(true);
    saveMenu.addToOverlay(background.entity());
    actionMenu.addToOverlay(background.entity());
    actionMenu.setHidden(true);

    state = SelectingSave;
    inputDriver.drive(&saveMenu);
    systems.engine().inputSystem().getActor().addListener(*this);
    systems.engine().inputSystem().getActor().addListener(inputDriver);
}

void LoadGame::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
    systems.engine().inputSystem().getActor().removeListener(inputDriver);
    systems.engine().inputSystem().getActor().removeListener(*this);
}

void LoadGame::update(bl::engine::Engine& engine, float dt, float) {
    switch (state) {
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
            if (saves[selectedSave].load()) {
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

    case SelectingSave:
    case ChooseAction:
        break;

    default:
        state = SelectingSave;
        inputDriver.drive(&saveMenu);
        break;
    }
}

bool LoadGame::observe(const bl::input::Actor&, unsigned int activatedControl,
                       bl::input::DispatchType, bool fromEvent) {
    if (activatedControl == core::input::Control::Back && fromEvent) {
        switch (state) {
        case SelectingSave:
            bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
            systems.engine().popState();
            break;

        case ChooseAction:
            state = SelectingSave;
            bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
            inputDriver.drive(&saveMenu);
            actionMenu.setHidden(true);
            break;

        default:
            break;
        }
    }
    return true;
}

// void LoadGame::render(bl::engine::Engine& engine, float lag) {
//     engine.window().clear();
//
//     engine.window().draw(background);
//     if (state != SelectingSave) { actionMenu.render(engine.window()); }
//     saveMenu.render(engine.window());
//     if (state == Error) { systems.hud().render(engine.window(), lag); }
//     if (state == Fading) { engine.window().draw(cover); }
//
//     engine.window().display();
// }

void LoadGame::saveSelected(unsigned int i) {
    selectedSave = i;
    state        = ChooseAction;
    inputDriver.drive(&actionMenu);
    const glm::vec2 pos(170.f + saveMenu.getBounds().width + 40.f,
                        200.f + static_cast<float>(i) * 26.f + 32.f);
    actionMenu.setPosition(pos + glm::vec2(22.f, 14.f));
    actionMenu.setHidden(false);
}

void LoadGame::errorDone() {
    // TODO - refactor out reset logic?
    activate(systems.engine());
}

} // namespace state
} // namespace game
