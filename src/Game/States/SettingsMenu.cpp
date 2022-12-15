#include <Game/States/SettingsMenu.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f MenuPosition(40.f, 70.f);
const sf::Color LabelColor(0, 60, 130);
constexpr unsigned int FontSize  = 30;
constexpr float FontSizeF        = static_cast<float>(FontSize);
constexpr unsigned int LabelSize = FontSize + 5;
constexpr unsigned int SmallSize = 22;

sf::VideoMode defaultVideoMode() {
    return sf::VideoMode(core::Properties::WindowWidth(), core::Properties::WindowHeight(), 32);
}

sf::VideoMode bestFullscreenMode() {
    const unsigned int ar =
        sf::VideoMode::getDesktopMode().width / sf::VideoMode::getDesktopMode().height;
    const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
    sf::VideoMode const* best               = nullptr;
    unsigned int highest                    = 0;

    for (const sf::VideoMode& mode : modes) {
        const unsigned int mar = mode.width / mode.height;
        if (mar == ar) {
            const unsigned int s = mode.width * mode.height;
            if (s > highest || best == nullptr) {
                best    = &mode;
                highest = s;
            }
        }
    }

    if (best == nullptr) {
        BL_LOG_ERROR << "Failed to select a valid fullscreen video mode. Attempting defualt mode";
        return defaultVideoMode();
    }
    return *best;
}

std::string volumeString() {
    return "Volume: " + std::to_string(static_cast<int>(bl::audio::AudioSystem::getVolume())) + "%";
}

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
    const std::string& Path = joinPath(core::Properties::MenuImagePath(), "Settings");
    const sf::Font& font    = core::Properties::MenuFont();

    bgndTexture = TextureManager::load(joinPath(Path, "background.png"));
    background.setTexture(*bgndTexture);
    hint.setFont(font);
    hint.setFillColor(sf::Color(65, 10, 0));
    hint.setCharacterSize(18);
    hint.setPosition(590.f, 440.f);

    const auto makeBack = [this, &font]() {
        Item::Ptr bi = TextItem::create("Back", font, sf::Color::Black, FontSize);
        bi->getSignal(Item::Activated).willAlwaysCall(std::bind(&SettingsMenu::back, this));
        bi->getSignal(Item::Selected).willAlwaysCall([this]() { setHint(""); });
        return bi;
    };

    // Top menu
    Item::Ptr videoItem = TextItem::create("Video", font, sf::Color::Black, FontSize);
    videoItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::VideoMenu);
    });
    videoItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Settings related to the Peoplemon window.");
    });
    Item::Ptr audioItem = TextItem::create("Audio", font, sf::Color::Black, FontSize);
    audioItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::AudioMenu);
    });
    audioItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Settings related to music and sounds.");
    });
    Item::Ptr controlsItem = TextItem::create("Controls", font, sf::Color::Black, FontSize);
    controlsItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::ControlsTopMenu);
    });
    controlsItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Change your controls for both keyboard/mouse and controller.");
    });

    topMenu.setRootItem(videoItem);
    topMenu.addItem(audioItem, videoItem.get(), Item::Bottom);
    topMenu.addItem(controlsItem, audioItem.get(), Item::Bottom);
    topMenu.addItem(makeBack(), controlsItem.get(), Item::Bottom);
    topMenu.setPosition(MenuPosition);

    // Video menu
    Item::Ptr videoLabel = TextItem::create("Video Settings", font, LabelColor, LabelSize);
    videoLabel->setSelectable(false);
    windowModeTextItem = TextItem::create(windowModeString(), font, sf::Color::Black, FontSize);
    windowModeDropdownItem =
        SubmenuItem::create(videoMenu, windowModeTextItem, Item::Right, Item::Bottom);
    windowModeDropdownItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        onWindowModeOpen();
    });
    windowModeDropdownItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Set whether to be fullscreen or windowed.");
    });
    fullscreenItem = TextItem::create("Fullscreen", font, sf::Color::Black, FontSize);
    fullscreenItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        onWindowModeChange(true);
    });
    fullscreenItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("The Peoplemon window will be fullscreen.");
    });
    windowedItem = TextItem::create("Windowed", font, sf::Color::Black, FontSize);
    windowedItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        onWindowModeChange(false);
    });
    windowedItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("The Peoplemon window will be a regular window that can be moved, resized, "
                "minimized, etc.");
    });
    windowModeDropdownItem->addOption(fullscreenItem);
    windowModeDropdownItem->addOption(windowedItem);
    windowModeDropdownItem->addOption(makeBack());
    vsyncItem = ToggleTextItem::create("Vsync Enabled:", font, sf::Color::Black, FontSize);
    vsyncItem->setBoxProperties(
        sf::Color::White, sf::Color::Black, FontSizeF, 2.f, FontSizeF * 0.4f, false);
    vsyncItem->getSignal(Item::Activated).willAlwaysCall([this]() { onVsyncUpdate(); });
    vsyncItem->setChecked(systems.engine().settings().windowParameters().vsyncEnabled());
    vsyncItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Vsync limits the render rate to your monitor refresh rate to prevent tearing. "
                "Graphics will be smoother but there may be input delay.");
    });

    videoMenu.setRootItem(windowModeDropdownItem);
    videoMenu.addItem(videoLabel, windowModeDropdownItem.get(), Item::Top);
    videoMenu.addItem(vsyncItem, windowModeDropdownItem.get(), Item::Bottom);
    videoMenu.addItem(makeBack(), vsyncItem.get(), Item::Bottom);
    videoMenu.setPosition(MenuPosition);
    videoMenu.setPadding({25.f, 10.f});

    // Audio menu
    Item::Ptr audioLabel = TextItem::create("Audio Settings", font, LabelColor, LabelSize);
    audioLabel->setSelectable(false);
    muteItem = ToggleTextItem::create("Mute", font, sf::Color::Black, FontSize);
    muteItem->setChecked(bl::audio::AudioSystem::getMuted());
    muteItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        bl::audio::AudioSystem::setMuted(muteItem->isChecked());
    });
    muteItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Enable and disable all sounds and music.");
    });
    volumeItem = TextItem::create(volumeString(), font, sf::Color::Black, FontSize);
    volumeItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::AudioSelectVolume);
    });
    volumeItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Sets the volume of all sounds and music.");
    });

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
    kbmItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Configure your controls when using keyboard and mouse.");
    });
    Item::Ptr padItem = TextItem::create("Controller", font, sf::Color::Black, FontSize);
    padItem->getSignal(Item::Activated).willAlwaysCall([this]() {
        enterState(MenuState::ControlsPadMenu);
    });
    padItem->getSignal(Item::Selected).willAlwaysCall([this]() {
        setHint("Configure your controls when using a controller.");
    });

    controlsTopMenu.setRootItem(kbmItem);
    controlsTopMenu.addItem(topCtrlLabel, kbmItem.get(), Item::Top);
    controlsTopMenu.addItem(padItem, kbmItem.get(), Item::Bottom);
    controlsTopMenu.addItem(makeBack(), padItem.get(), Item::Bottom);
    controlsTopMenu.setPosition(MenuPosition);

    const auto makeCtrl = [this, &font](unsigned int ctrl, bool kbm) {
        TextItem::Ptr item =
            TextItem::create(ctrlString(ctrl, kbm), font, sf::Color::Black, SmallSize);
        TextItem* rp = item.get();
        item->getSignal(Item::Activated).willAlwaysCall([this, ctrl, kbm, rp]() {
            ctrlItem = rp;
            startBindControl(kbm, ctrl);
        });
        item->getSignal(Item::Selected).willAlwaysCall([this]() {
            setHint("Select to rebind this control.");
        });
        return item;
    };

    // kbm menu
    Item::Ptr kbmLabel = TextItem::create("Keyboard and Mouse", font, LabelColor, LabelSize);
    kbmLabel->setSelectable(false);
    bl::menu::TextItem::Ptr kbmUpItem       = makeCtrl(Control::MoveUp, true);
    bl::menu::TextItem::Ptr kbmRightItem    = makeCtrl(Control::MoveRight, true);
    bl::menu::TextItem::Ptr kbmDownItem     = makeCtrl(Control::MoveDown, true);
    bl::menu::TextItem::Ptr kbmLeftItem     = makeCtrl(Control::MoveLeft, true);
    bl::menu::TextItem::Ptr kbmSprintItem   = makeCtrl(Control::Sprint, true);
    bl::menu::TextItem::Ptr kbmInteractItem = makeCtrl(Control::Interact, true);
    bl::menu::TextItem::Ptr kbmBackItem     = makeCtrl(Control::Back, true);
    bl::menu::TextItem::Ptr kbmPauseItem    = makeCtrl(Control::Pause, true);
    Item::Ptr backItem                      = makeBack();

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
    bl::menu::TextItem::Ptr padUpItem       = makeCtrl(Control::MoveUp, false);
    bl::menu::TextItem::Ptr padRightItem    = makeCtrl(Control::MoveRight, false);
    bl::menu::TextItem::Ptr padDownItem     = makeCtrl(Control::MoveDown, false);
    bl::menu::TextItem::Ptr padLeftItem     = makeCtrl(Control::MoveLeft, false);
    bl::menu::TextItem::Ptr padSprintItem   = makeCtrl(Control::Sprint, false);
    bl::menu::TextItem::Ptr padInteractItem = makeCtrl(Control::Interact, false);
    bl::menu::TextItem::Ptr padBackItem     = makeCtrl(Control::Back, false);
    bl::menu::TextItem::Ptr padPauseItem    = makeCtrl(Control::Pause, false);
    backItem                                = makeBack();

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
    enterState(MenuState::TopMenu);
}

void SettingsMenu::deactivate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().removeListener(*this);
    engine.renderSystem().cameras().popCamera();

    engine.inputSystem().saveToConfig();
    core::Properties::save();
}

void SettingsMenu::update(bl::engine::Engine&, float) {
    if (state == MenuState::ControlsBindingControl && ctrlConfigurator.finished()) {
        ctrlItem->getTextObject().setString(ctrlString(bindingCtrl, bindingKbm));
        if (bindingKbm) {
            enterState(MenuState::ControlsKBMMenu);
            controlsKbmMenu.refreshPositions();
        }
        else {
            enterState(MenuState::ControlsPadMenu);
            controlsPadMenu.refreshPositions();
        }
    }
}

void SettingsMenu::render(bl::engine::Engine& engine, float) {
    engine.window().clear();

    engine.window().draw(background);
    engine.window().draw(hint);
    switch (state) {
    case MenuState::TopMenu:
        topMenu.render(engine.window());
        break;

    case MenuState::VideoSelectMode:
    case MenuState::VideoMenu:
        videoMenu.render(engine.window());
        break;

    case MenuState::AudioSelectVolume:
        audioMenu.render(engine.window());
        volumeEntry.render(engine.window());
        break;
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

    case MenuState::ControlsBindingControl:
        if (bindingKbm) { controlsKbmMenu.render(engine.window()); }
        else { controlsPadMenu.render(engine.window()); }
        break;

    default:
        break;
    }

    engine.window().display();
}

void SettingsMenu::enterState(MenuState s) {
    state = s;

    Menu* sm = nullptr;

    switch (state) {
    case MenuState::TopMenu:
        inputDriver.drive(&topMenu);
        sm = &topMenu;
        break;
    case MenuState::VideoMenu:
        inputDriver.drive(&videoMenu);
        sm = &videoMenu;
        break;
    case MenuState::VideoSelectMode:
        // nothing
        break;
    case MenuState::AudioMenu:
        inputDriver.drive(&audioMenu);
        sm = &audioMenu;
        break;
    case MenuState::AudioSelectVolume:
        volumeEntry.configure(0, 100, static_cast<int>(bl::audio::AudioSystem::getVolume()));
        volumeEntry.setPosition({volumeItem->getPosition().x + volumeItem->getSize().x -
                                     volumeEntry.getSize().x + audioMenu.getBounds().left + 20.f,
                                 volumeItem->getPosition().y + audioMenu.getBounds().top + 20.f});
        break;
    case MenuState::ControlsTopMenu:
        inputDriver.drive(&controlsTopMenu);
        sm = &controlsTopMenu;
        break;
    case MenuState::ControlsKBMMenu:
        inputDriver.drive(&controlsKbmMenu);
        sm = &controlsKbmMenu;
        break;
    case MenuState::ControlsPadMenu:
        inputDriver.drive(&controlsPadMenu);
        sm = &controlsPadMenu;
        break;
    default:
        break;
    }

    if (sm != nullptr) { sm->setSelectedItem(const_cast<Item*>(sm->getSelectedItem())); }
}

void SettingsMenu::close() { systems.engine().popState(); }

bool SettingsMenu::observe(const bl::input::Actor&, unsigned int activatedControl,
                           bl::input::DispatchType, bool eventTriggered) {
    if (activatedControl == Control::Back) {
        if (eventTriggered) { back(); }
    }
    else {
        switch (state) {
        case MenuState::AudioSelectVolume:
            switch (activatedControl) {
            case Control::MoveUp:
                volumeEntry.up(1, eventTriggered);
                bl::audio::AudioSystem::setVolume(static_cast<float>(volumeEntry.curQty()));
                break;
            case Control::MoveRight:
                volumeEntry.up(10, eventTriggered);
                bl::audio::AudioSystem::setVolume(static_cast<float>(volumeEntry.curQty()));
                break;
            case Control::MoveDown:
                volumeEntry.down(1, eventTriggered);
                bl::audio::AudioSystem::setVolume(static_cast<float>(volumeEntry.curQty()));
                break;
            case Control::MoveLeft:
                volumeEntry.down(10, eventTriggered);
                bl::audio::AudioSystem::setVolume(static_cast<float>(volumeEntry.curQty()));
                break;
            case Control::Interact:
                if (eventTriggered) { back(); }
                break;
            default:
                break;
            }
            break;
        case MenuState::ControlsBindingControl:
            // do nothing
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
        windowModeDropdownItem->closeMenu();
        enterState(MenuState::VideoMenu);
        break;
    case MenuState::AudioSelectVolume:
        volumeItem->getTextObject().setString(volumeString());
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

void SettingsMenu::startBindControl(bool kbm, unsigned int ctrl) {
    auto& a     = systems.engine().inputSystem().getActor();
    bindingCtrl = ctrl;
    bindingKbm  = kbm;
    if (kbm) {
        ctrlConfigurator.start(a.getKBMTriggerControl(ctrl));
        setHint("Press any key or mouse button to rebind the control.");
    }
    else {
        ctrlConfigurator.start(a.getJoystickTriggerControl(ctrl));
        setHint("Move a joystick or press a button on your controller to rebind the control.");
    }
    enterState(MenuState::ControlsBindingControl);
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

void SettingsMenu::onWindowModeOpen() {
    enterState(MenuState::VideoSelectMode);
    videoMenu.setSelectedItem(isFullscreen() ? fullscreenItem.get() : windowedItem.get());
}

bool SettingsMenu::isFullscreen() const {
    return (systems.engine().settings().windowParameters().style() & sf::Style::Fullscreen) != 0;
}

std::string SettingsMenu::windowModeString() const {
    return std::string("Window mode: ") + (isFullscreen() ? "Fullscreen" : "Windowed");
}

void SettingsMenu::onWindowModeChange(bool fs) {
    windowModeDropdownItem->closeMenu();
    if (fs != isFullscreen()) {
        bl::engine::Settings::WindowParameters params =
            systems.engine().settings().windowParameters();
        if (fs) {
            params.withVideoMode(bestFullscreenMode());
            params.withStyle(params.style() | sf::Style::Fullscreen);
        }
        else {
            params.withVideoMode(defaultVideoMode());
            params.withStyle(params.style() & (~sf::Style::Fullscreen));
        }

        windowModeTextItem->getTextObject().setString(windowModeString());
        systems.engine().reCreateWindow(params);
    }
    enterState(MenuState::VideoMenu);
}

void SettingsMenu::onVsyncUpdate() {
    bl::engine::Settings::WindowParameters params = systems.engine().settings().windowParameters();
    params.withVSyncEnabled(vsyncItem->isChecked());
    systems.engine().updateExistingWindow(params);
}

void SettingsMenu::setHint(const std::string& m) {
    hint.setString(m);
    bl::interface::wordWrap(hint, 183.f);
}

} // namespace state
} // namespace game
