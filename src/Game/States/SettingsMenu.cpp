#include <Game/States/SettingsMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f MenuPosition(40.f, 70.f);
const sf::Color LabelColor(0, 60, 130);
constexpr unsigned int FontSize  = 30;
constexpr unsigned int LabelSize = FontSize + 5;
constexpr unsigned int SmallSize = 26;
} // namespace
using namespace bl::menu;
using namespace core::input;

bl::engine::State::Ptr SettingsMenu::create(core::system::Systems& s) {
    return Ptr{new SettingsMenu(s)};
}

SettingsMenu::SettingsMenu(core::system::Systems& s)
: State(s)
, state(MenuState::TopMenu)
, topMenu(ArrowSelector::create(10.f, sf::Color::Black))
, videoMenu(ArrowSelector::create(10.f, sf::Color::Black))
, audioMenu(ArrowSelector::create(10.f, sf::Color::Black))
, controlsTopMenu(ArrowSelector::create(10.f, sf::Color::Black))
, controlsKbmMenu(ArrowSelector::create(8.f, sf::Color::Black))
, controlsPadMenu(ArrowSelector::create(8.f, sf::Color::Black)) {
    const auto joinPath     = bl::util::FileUtil::joinPath;
    auto& textures          = bl::engine::Resources::textures();
    const std::string& Path = joinPath(core::Properties::MenuImagePath(), "Settings");
    const sf::Font& font    = core::Properties::MenuFont();

    bgndTexture = textures.load(joinPath(Path, "background.png")).data;
    background.setTexture(*bgndTexture);

    const auto makeBack = [this, &font]() {
        Item::Ptr bi = TextItem::create("Back", font, sf::Color::Black, FontSize);
        bi->getSignal(Item::Activated).willAlwaysCall(std::bind(&SettingsMenu::back, this));
        return bi;
    };

    // Top menu
    Item::Ptr videoItem = TextItem::create("Video", font, sf::Color::Black, FontSize);
    videoItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::VideoMenu);
    });
    Item::Ptr audioItem = TextItem::create("Audio", font, sf::Color::Black, FontSize);
    audioItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::AudioMenu);
    });
    Item::Ptr controlsItem = TextItem::create("Controls", font, sf::Color::Black, FontSize);
    controlsItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::ControlsTopMenu);
    });

    topMenu.setRootItem(videoItem);
    topMenu.addItem(audioItem, videoItem.get(), Item::Bottom);
    topMenu.addItem(controlsItem, audioItem.get(), Item::Bottom);
    topMenu.addItem(makeBack(), controlsItem.get(), Item::Bottom);
    topMenu.setPosition(MenuPosition);

    // Video menu
    Item::Ptr videoLabel = TextItem::create("Video Settings", font, LabelColor, LabelSize);
    videoLabel->setSelectable(false);
    windowModeItem = TextItem::create("Window mode", font, sf::Color::Black, FontSize);
    // TODO - make dropdown

    videoMenu.setRootItem(windowModeItem);
    videoMenu.addItem(videoLabel, windowModeItem.get(), Item::Top);
    videoMenu.addItem(makeBack(), windowModeItem.get(), Item::Bottom);
    videoMenu.setPosition(MenuPosition);

    // Audio menu
    Item::Ptr audioLabel = TextItem::create("Audio Settings", font, LabelColor, LabelSize);
    audioLabel->setSelectable(false);
    muteItem = TextItem::create("Mute", font, sf::Color::Black, FontSize);
    // TODO - make toggle
    volumeItem = TextItem::create("Volume", font, sf::Color::Black, FontSize);
    // TODO - tie into qty select

    audioMenu.setRootItem(muteItem);
    audioMenu.addItem(audioLabel, muteItem.get(), Item::Top);
    audioMenu.addItem(volumeItem, muteItem.get(), Item::Bottom);
    audioMenu.addItem(makeBack(), volumeItem.get(), Item::Bottom);
    audioMenu.setPosition(MenuPosition);

    // Controls top menu
    Item::Ptr topCtrlLabel = TextItem::create("Control Settings", font, LabelColor, LabelSize);
    topCtrlLabel->setSelectable(false);
    Item::Ptr kbmItem = TextItem::create("Keyboard and mouse", font, sf::Color::Black, FontSize);
    kbmItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::ControlsKBMMenu);
    });
    Item::Ptr padItem = TextItem::create("Controller", font, sf::Color::Black, FontSize);
    padItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::ControlsPadMenu);
    });

    controlsTopMenu.setRootItem(kbmItem);
    controlsTopMenu.addItem(topCtrlLabel, kbmItem.get(), Item::Top);
    controlsTopMenu.addItem(padItem, kbmItem.get(), Item::Bottom);
    controlsTopMenu.addItem(makeBack(), padItem.get(), Item::Bottom);
    controlsTopMenu.setPosition(MenuPosition);

    const auto makeCtrl = [this, &font](unsigned int ctrl, bool kbm) {
        TextItem::Ptr item =
            TextItem::create(ctrlString(ctrl, kbm), font, sf::Color::Black, SmallSize);
        item->getSignal(Item::Activated).willAlwaysCall([this, ctrl, kbm]() {
            startBindControl(kbm, ctrl);
        });
        return item;
    };

    // kbm menu
    Item::Ptr kbmLabel = TextItem::create("Keyboard and Mouse", font, LabelColor, LabelSize);
    kbmLabel->setSelectable(false);
    kbmUpItem          = makeCtrl(Control::MoveUp, true);
    kbmRightItem       = makeCtrl(Control::MoveRight, true);
    kbmDownItem        = makeCtrl(Control::MoveDown, true);
    kbmLeftItem        = makeCtrl(Control::MoveLeft, true);
    kbmSprintItem      = makeCtrl(Control::Sprint, true);
    kbmInteractItem    = makeCtrl(Control::Interact, true);
    kbmBackItem        = makeCtrl(Control::Back, true);
    kbmPauseItem       = makeCtrl(Control::Pause, true);
    Item::Ptr backItem = makeBack();

    controlsKbmMenu.setRootItem(kbmUpItem);
    controlsKbmMenu.setPosition(MenuPosition);
    controlsKbmMenu.setPadding({75.f, 12.f});

    controlsKbmMenu.addItem(kbmLabel, kbmUpItem.get(), Item::Top);
    controlsKbmMenu.addItem(kbmRightItem, kbmUpItem.get(), Item::Bottom);
    controlsKbmMenu.addItem(kbmDownItem, kbmRightItem.get(), Item::Bottom);
    controlsKbmMenu.addItem(kbmLeftItem, kbmDownItem.get(), Item::Bottom);
    controlsKbmMenu.addItem(backItem, kbmLeftItem.get(), Item::Bottom);

    controlsKbmMenu.addItem(kbmSprintItem, kbmUpItem.get(), Item::Right);
    controlsKbmMenu.addItem(kbmInteractItem, kbmSprintItem.get(), Item::Bottom);
    controlsKbmMenu.addItem(kbmBackItem, kbmInteractItem.get(), Item::Bottom);
    controlsKbmMenu.addItem(kbmPauseItem, kbmBackItem.get(), Item::Bottom);

    controlsKbmMenu.attachExisting(kbmRightItem.get(), kbmInteractItem.get(), Item::Left);
    controlsKbmMenu.attachExisting(kbmDownItem.get(), kbmBackItem.get(), Item::Left);
    controlsKbmMenu.attachExisting(kbmLeftItem.get(), kbmPauseItem.get(), Item::Left);
    controlsKbmMenu.attachExisting(backItem.get(), kbmPauseItem.get(), Item::Bottom, false);

    // controller menu
    Item::Ptr padLabel = TextItem::create("Controller", font, LabelColor, LabelSize);
    padLabel->setSelectable(false);
    padUpItem          = makeCtrl(Control::MoveUp, false);
    padRightItem       = makeCtrl(Control::MoveRight, false);
    padDownItem        = makeCtrl(Control::MoveDown, false);
    padLeftItem        = makeCtrl(Control::MoveLeft, false);
    padSprintItem      = makeCtrl(Control::Sprint, false);
    padInteractItem    = makeCtrl(Control::Interact, false);
    padBackItem        = makeCtrl(Control::Back, false);
    padPauseItem       = makeCtrl(Control::Pause, false);
    backItem = makeBack();

    controlsPadMenu.setRootItem(padUpItem);
    controlsPadMenu.setPosition(MenuPosition);
    controlsPadMenu.setPadding({75.f, 12.f});

    controlsPadMenu.addItem(padLabel, padUpItem.get(), Item::Top);
    controlsPadMenu.addItem(padRightItem, padUpItem.get(), Item::Bottom);
    controlsPadMenu.addItem(padDownItem, padRightItem.get(), Item::Bottom);
    controlsPadMenu.addItem(padLeftItem, padDownItem.get(), Item::Bottom);
    controlsPadMenu.addItem(backItem, padLeftItem.get(), Item::Bottom);

    controlsPadMenu.addItem(padSprintItem, padUpItem.get(), Item::Right);
    controlsPadMenu.addItem(padInteractItem, padSprintItem.get(), Item::Bottom);
    controlsPadMenu.addItem(padBackItem, padInteractItem.get(), Item::Bottom);
    controlsPadMenu.addItem(padPauseItem, padBackItem.get(), Item::Bottom);

    controlsPadMenu.attachExisting(padRightItem.get(), padInteractItem.get(), Item::Left);
    controlsPadMenu.attachExisting(padDownItem.get(), padBackItem.get(), Item::Left);
    controlsPadMenu.attachExisting(padLeftItem.get(), padPauseItem.get(), Item::Left);
    controlsPadMenu.attachExisting(backItem.get(), padPauseItem.get(), Item::Bottom, false);
}

const char* SettingsMenu::name() const { return "SettingsMenu"; }

void SettingsMenu::activate(bl::engine::Engine& engine) {
    state = MenuState::TopMenu;
    inputDriver.drive(&topMenu);
    engine.inputSystem().getActor().addListener(*this);
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(background.getGlobalBounds()));
}

void SettingsMenu::deactivate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().removeListener(*this);
    engine.renderSystem().cameras().popCamera();

    // TODO - sync settings to config and save to file
}

void SettingsMenu::update(bl::engine::Engine&, float) {
    // TODO - ?
}

void SettingsMenu::render(bl::engine::Engine& engine, float) {
    engine.window().clear();

    engine.window().draw(background);
    switch (state) {
    case MenuState::TopMenu:
        topMenu.render(engine.window());
        break;

    case MenuState::VideoSelectMode:
        // TODO - anything else?
    case MenuState::VideoMenu:
        videoMenu.render(engine.window());
        break;

    case MenuState::AudioSelectVolume:
        // TODO - render qty entry
        [[fallthrough]];
    case MenuState::AudioMenu:
        audioMenu.render(engine.window());
        break;

    case MenuState::ControlsTopMenu:
        controlsTopMenu.render(engine.window());
        break;

    case MenuState::ControlsKBMMenu:
        controlsKbmMenu.render(engine.window());
        break;

    case MenuState::ControlsPadMenu:
        controlsPadMenu.render(engine.window());
        break;

    default:
        break;
    }

    engine.window().display();
}

void SettingsMenu::enterState(MenuState s) {
    state = s;

    switch (state) {
    case MenuState::TopMenu:
        inputDriver.drive(&topMenu);
        break;
    case MenuState::VideoMenu:
        inputDriver.drive(&videoMenu);
        break;
    case MenuState::VideoSelectMode:
        // TODO
        break;
    case MenuState::AudioMenu:
        inputDriver.drive(&audioMenu);
        break;
    case MenuState::AudioSelectVolume:
        // TODO
        break;
    case MenuState::ControlsTopMenu:
        inputDriver.drive(&controlsTopMenu);
        break;
    case MenuState::ControlsKBMMenu:
        inputDriver.drive(&controlsKbmMenu);
        break;
    case MenuState::ControlsPadMenu:
        inputDriver.drive(&controlsPadMenu);
        break;
    default:
        break;
    }
}

void SettingsMenu::close() { systems.engine().popState(); }

bool SettingsMenu::observe(const bl::input::Actor&, unsigned int activatedControl,
                           bl::input::DispatchType, bool eventTriggered) {
    if (activatedControl == Control::Back) {
        if (eventTriggered) { back(); }
    }
    else {
        switch (state) {
        case MenuState::VideoSelectMode:
            // TODO
            break;
        case MenuState::AudioSelectVolume:
            // TODO
            break;
        case MenuState::ControlsBindingControl:
            // TODO
            break;
        default:
            inputDriver.sendControl(activatedControl, eventTriggered);
            break;
        }
    }

    return true;
}

void SettingsMenu::back() {
    switch (state) {
    case MenuState::TopMenu:
        close();
        break;
    case MenuState::VideoMenu:
    case MenuState::AudioMenu:
    case MenuState::ControlsTopMenu:
        enterState(MenuState::TopMenu);
        break;
    case MenuState::VideoSelectMode:
        enterState(MenuState::VideoMenu);
        break;
    case MenuState::AudioSelectVolume:
        enterState(MenuState::AudioMenu);
        break;
    case MenuState::ControlsKBMMenu:
    case MenuState::ControlsPadMenu:
        enterState(MenuState::ControlsTopMenu);
        break;
    default:
        break;
    }
}

void SettingsMenu::startBindControl(bool, unsigned int) {
    // TODO
}

std::string SettingsMenu::ctrlString(unsigned int ctrl, bool kbm) const {
    const auto& a        = systems.engine().inputSystem().getActor();
    const std::string cs = kbm ? a.getKBMTriggerControl(ctrl).toString() :
                                 a.getJoystickTriggerControl(ctrl).toString();

    switch (ctrl) {
    case Control::MoveUp:
        return "Move up: " + cs;
    case Control::MoveRight:
        return "Move right: " + cs;
    case Control::MoveDown:
        return "Move down: " + cs;
    case Control::MoveLeft:
        return "Move left: " + cs;
    case Control::Sprint:
        return "Sprint: " + cs;
    case Control::Interact:
        return "Interact: " + cs;
    case Control::Back:
        return "Back: " + cs;
    case Control::Pause:
        return "Pause: " + cs;
    default:
        return "ERROR: Unknown control";
    }
}

} // namespace state
} // namespace game
