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
: State(s, bl::engine::StateMask::Menu)
, state(SelectingSave)
, selectedSave(0)
, fadeout(nullptr) {}

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

        actionMenu.create(
            engine, engine.renderer().getObserver(), ArrowSelector::create(12.f, sf::Color::Black));
        Item::Ptr load =
            TextItem::create("Load", core::Properties::MenuFont(), sf::Color::Black, 26);
        load->getSignal(Item::Activated).willAlwaysCall([this]() {
            inputDriver.drive(nullptr);
            state   = Fading;
            fadeout = systems.engine()
                          .renderer()
                          .getObserver()
                          .getRenderGraph()
                          .putTask<bl::rc::rgi::FadeEffectTask>(FadeTime);
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
            sf::Color::White, sf::Color::Black, 3.f, {22.f, 2.f, 2.f, 2.f});
    }

    scene = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    reset();
}

void LoadGame::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().removeScene(scene);
    systems.engine().inputSystem().getActor().removeListener(inputDriver);
    systems.engine().inputSystem().getActor().removeListener(*this);
}

void LoadGame::update(bl::engine::Engine& engine, float dt, float) {
    switch (state) {
    case SaveDeleted:
        if (saves[selectedSave].remove()) { reset(); }
        else {
            systems.hud().displayMessage("Failed to delete game save",
                                         std::bind(&LoadGame::errorDone, this));
            inputDriver.drive(nullptr);
            state = Error;
        }
        break;

    case Fading:
        if (fadeout->complete()) {
            engine.renderer()
                .getObserver()
                .getRenderGraph()
                .removeTask<bl::rc::rgi::FadeEffectTask>();
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

void LoadGame::saveSelected(unsigned int i) {
    selectedSave = i;
    state        = ChooseAction;
    inputDriver.drive(&actionMenu);
    const glm::vec2 pos(170.f + saveMenu.getBounds().width + 40.f,
                        200.f + static_cast<float>(i) * 26.f + 32.f);
    actionMenu.setPosition(pos + glm::vec2(22.f, 14.f));
    actionMenu.setHidden(false);
}

void LoadGame::errorDone() { reset(); }

void LoadGame::reset() {
    saves.clear();
    core::file::GameSave::listSaves(saves);

    Item::Ptr item =
        TextItem::create("Back", core::Properties::MenuFont(), sf::Color(140, 0, 0), 26);
    item->getSignal(Item::Activated).willAlwaysCall([this]() {
        observe(systems.engine().inputSystem().getActor(),
                core::input::Control::Back,
                bl::input::DispatchType::TriggerActivated,
                true);
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

    bl::rc::Overlay* overlay = systems.engine().renderer().getObserver().getCurrentOverlay();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    saveMenu.addToOverlay(background.entity());
    actionMenu.addToOverlay(background.entity());
    actionMenu.setHidden(true);

    state = SelectingSave;
    inputDriver.drive(&saveMenu);
    systems.engine().inputSystem().getActor().addListener(*this);
    systems.engine().inputSystem().getActor().addListener(inputDriver);
}

} // namespace state
} // namespace game
