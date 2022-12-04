#include <Game/States/StorageSystem.hpp>

#include <Core/Events/StorageSystem.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/States/PeoplemonMenu.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f BoxTitlePosition(365.f, 70.f);
const sf::Vector2f BoxTitleSize(701.f - BoxTitlePosition.x, 130.f - BoxTitlePosition.y);
const sf::Vector2f BoxPosition(293.f, 158.f);
const sf::Vector2f BoxSize(784.f - BoxPosition.x, 581.f - BoxPosition.y);
const sf::Vector2f ThumbnailPosition(68.f, 284.f);
const sf::Vector2f ThumbnailSize(218.f - ThumbnailPosition.x, 434.f - ThumbnailPosition.y);
const sf::Vector2f NamePosition(ThumbnailPosition.x + ThumbnailSize.x * 0.5f,
                                ThumbnailPosition.y + ThumbnailSize.y + 4.f);
const sf::Vector2f LevelPosition(ThumbnailPosition.x + 4.f, 475.f);
const sf::Vector2f ItemLabelPosition(LevelPosition.x, LevelPosition.y + 32.f);
const sf::Vector2f ItemPosition(ItemLabelPosition.x, ItemLabelPosition.y + 25.f);
constexpr float SlideTime = 0.5f;
const float SlideVel      = BoxSize.x / SlideTime;
} // namespace

bl::engine::State::Ptr StorageSystem::create(core::system::Systems& s) {
    return bl::engine::State::Ptr(new StorageSystem(s));
}

StorageSystem::StorageSystem(core::system::Systems& s)
: State(s)
, state(MenuState::ChooseAction)
, currentBox(0)
, hovered(nullptr)
, actionMenu(bl::menu::ArrowSelector::create(7.f, sf::Color::Black))
, contextMenu(bl::menu::ArrowSelector::create(7.f, sf::Color::Black))
, depositedPeoplemon(-1) {
    auto joinPath = bl::util::FileUtil::joinPath;
    using bl::menu::Item;
    using bl::menu::TextItem;
    using core::Properties;

    backgroundTxtr =
        TextureManager::load(joinPath(Properties::MenuImagePath(), "StorageSystem/storageBGND.png"))
            .data;
    background.setTexture(*backgroundTxtr, true);

    leftArrowTxtr = TextureManager::load(
                        joinPath(Properties::MenuImagePath(), "StorageSystem/storageArrowLeft.png"))
                        .data;
    leftArrow.setTexture(*leftArrowTxtr, true);
    rightArrowTxtr = TextureManager::load(joinPath(Properties::MenuImagePath(),
                                                   "StorageSystem/storageArrowRight.png"))
                         .data;
    rightArrow.setTexture(*rightArrowTxtr, true);
    leftArrow.setPosition(BoxTitlePosition.x - leftArrow.getGlobalBounds().width - 3.f,
                          BoxTitlePosition.y + BoxTitleSize.y * 0.5f -
                              leftArrow.getGlobalBounds().height * 0.5f);
    rightArrow.setPosition(BoxTitlePosition.x + BoxTitleSize.x + 3.f,
                           BoxTitlePosition.y + BoxTitleSize.y * 0.5f -
                               rightArrow.getGlobalBounds().height * 0.5f);
    boxTitle.setFont(Properties::MenuFont());
    boxTitle.setCharacterSize(42);
    boxTitle.setFillColor(sf::Color::Black);
    boxTitle.setPosition(BoxTitlePosition + BoxTitleSize * 0.5f);

    nickname.setFont(Properties::MenuFont());
    nickname.setFillColor(sf::Color(240, 40, 50));
    nickname.setPosition(NamePosition);
    nickname.setCharacterSize(26);
    level.setFont(Properties::MenuFont());
    level.setFillColor(sf::Color(35, 160, 245));
    level.setPosition(LevelPosition);
    level.setCharacterSize(22);
    itemLabel.setFont(Properties::MenuFont());
    itemLabel.setFillColor(sf::Color(200, 255, 255));
    itemLabel.setPosition(ItemLabelPosition);
    itemLabel.setCharacterSize(14);
    itemLabel.setString("Hold item:");
    itemName.setFont(Properties::MenuFont());
    itemName.setFillColor(sf::Color(165, 255, 255));
    itemName.setPosition(ItemPosition);
    itemName.setCharacterSize(16);
    thumbnail.setPosition(ThumbnailPosition);

    TextItem::Ptr depositItem =
        TextItem::create("Deposit", Properties::MenuFont(), sf::Color::Black, 34);
    depositItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::startDeposit, this));
    withdrawActionItem = TextItem::create("Browse", Properties::MenuFont(), sf::Color::Black, 34);
    withdrawActionItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::startBrowse, this));
    TextItem::Ptr closeItem =
        TextItem::create("Close", Properties::MenuFont(), sf::Color::Black, 34);
    closeItem->getSignal(Item::Activated).willAlwaysCall(std::bind(&StorageSystem::close, this));
    actionMenu.setRootItem(withdrawActionItem);
    actionMenu.addItem(depositItem, withdrawActionItem.get(), Item::Bottom);
    actionMenu.addItem(closeItem, depositItem.get(), Item::Bottom);
    actionMenu.setPosition({30.f, 25.f});
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {12.f, 2.f, 2.f, 0.f});

    TextItem::Ptr withdrawItem = TextItem::create("Withdraw", Properties::MenuFont());
    withdrawItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::onWithdraw, this));
    TextItem::Ptr moveItem = TextItem::create("Move", Properties::MenuFont());
    moveItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::onStartMove, this));
    TextItem::Ptr itemItem = TextItem::create("Take Item", Properties::MenuFont());
    itemItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::onTakeItem, this));
    TextItem::Ptr releaseItem = TextItem::create("Release", Properties::MenuFont());
    releaseItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::onRelease, this));
    TextItem::Ptr backItem = TextItem::create("Back", Properties::MenuFont());
    backItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::onCloseContextMenu, this));
    contextMenu.setRootItem(withdrawItem);
    contextMenu.addItem(moveItem, withdrawItem.get(), Item::Bottom);
    contextMenu.addItem(itemItem, moveItem.get(), Item::Bottom);
    contextMenu.addItem(releaseItem, itemItem.get(), Item::Bottom);
    contextMenu.addItem(backItem, releaseItem.get(), Item::Bottom);
    contextMenu.configureBackground(sf::Color::White, sf::Color::Black, 2.f, {12.f, 2.f, 2.f, 0.f});

    pageSlideFailSound = core::Properties::MenuMoveFailSound();
    pageSlideSound     = bl::audio::AudioSystem::getOrLoadSound(
        joinPath(Properties::SoundPath(), "Menu/storageBoxChange.mp3"));
    cursorMoveSound = bl::audio::AudioSystem::getOrLoadSound(
        joinPath(Properties::SoundPath(), "Menu/storageCursorMove.mp3"));
}

const char* StorageSystem::name() const { return "StorageSystem"; }

void StorageSystem::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    // create view for menu
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));
    view    = engine.window().getView();
    boxView = bl::interface::ViewUtil::computeSubView({BoxPosition, BoxSize}, view);
    boxView.setCenter(BoxSize * 0.5f);

    hovered = systems.player().state().storage.get(currentBox, cursor.getPosition());
    if (state == MenuState::WaitingDeposit) {
        if (depositedPeoplemon >= 0) {
            actionMenu.setSelectedItem(withdrawActionItem.get());
            const auto& ppl = systems.player().state().peoplemon[depositedPeoplemon];
            hovered         = nullptr;
            updatePeoplemonInfo(ppl);
            cursor.setHolding(ppl.id());
            enterState(MenuState::PlacingPeoplemon);
            showInfo = true;
        }
        else {
            enterState(MenuState::ChooseAction);
        }
    }
    else {
        enterState(MenuState::ChooseAction);
        finishBoxChange();
    }

    finishBoxChange();
    if (hovered != nullptr) { updatePeoplemonInfo(hovered->peoplemon); }
}

void StorageSystem::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    bl::event::Dispatcher::dispatch<core::event::StorageSystemClosed>({});
    engine.renderSystem().cameras().popCamera();
}

void StorageSystem::update(bl::engine::Engine&, float dt) {
    cursor.update(dt);

    switch (state) {
    case MenuState::BoxSliding:
        slideOffset += slideVel * dt;
        if (std::abs(slideOffset) >= BoxSize.x) { enterState(prevState); }
        break;

    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        systems.hud().update(dt);
        break;

    case MenuState::CursorMoving:
        if (!cursor.moving()) { enterState(prevState); }
        break;

    default:
        break;
    }
}

void StorageSystem::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();

    engine.window().draw(background);
    engine.window().draw(boxTitle);
    if (currentBox > 0) { engine.window().draw(leftArrow); }
    if (currentBox < 13) { engine.window().draw(rightArrow); }

    engine.window().draw(thumbnail);
    if (showInfo) {
        engine.window().draw(nickname);
        engine.window().draw(level);
        engine.window().draw(itemLabel);
        engine.window().draw(itemName);
    }
    actionMenu.render(engine.window());

    const sf::View origView = engine.window().getView();
    engine.window().setView(boxView);
    if (state == MenuState::BoxSliding) {
        sf::RenderStates states;
        states.transform.translate(slideOffset, 0.f);
        slidingOutGrid.render(engine.window(), states);
        const float sign = -(slideVel / std::abs(slideVel));
        states.transform.translate(BoxSize.x * sign, 0.f);
        activeGrid.render(engine.window(), states);
    }
    else {
        activeGrid.render(engine.window(), {});
        switch (state) {
        case MenuState::BrowseMenuOpen:
        case MenuState::BrowsingBox:
        case MenuState::PlacingPeoplemon:
        case MenuState::CursorMoving:
        case MenuState::WaitingContextMessage:
        case MenuState::WaitingReleaseConfirm:
        case MenuState::MovingPeoplemon:
            cursor.render(engine.window());
            break;
        default:
            break;
        }
    }

    engine.window().setView(origView);

    switch (state) {
    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        systems.hud().render(engine.window(), lag);
        [[fallthrough]];
    case MenuState::BrowseMenuOpen:
        contextMenu.render(engine.window());
        break;

    default:
        break;
    }

    engine.window().display();
}

void StorageSystem::startDeposit() {
    systems.engine().pushState(PeoplemonMenu::create(
        systems, PeoplemonMenu::Context::StorageSelect, -1, &depositedPeoplemon));
    enterState(MenuState::WaitingDeposit);
}

void StorageSystem::startBrowse() { enterState(MenuState::BrowsingBox); }

void StorageSystem::close() { systems.engine().popState(); }

void StorageSystem::onCursor(const sf::Vector2i& pos) {
    if (state == MenuState::PlacingPeoplemon || state == MenuState::MovingPeoplemon) return;
    onHover(systems.player().state().storage.get(currentBox, pos));
}

void StorageSystem::onHover(core::pplmn::StoredPeoplemon* ppl) {
    hovered = ppl;
    if (ppl != nullptr) {
        showInfo = true;
        updatePeoplemonInfo(ppl->peoplemon);
    }
    else {
        showInfo = false;
        thumbTxtr =
            TextureManager::load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                              "StorageSystem/question.png"))
                .data;
        thumbnail.setTexture(*thumbTxtr, true);
        thumbnail.setScale(ThumbnailSize.x / static_cast<float>(thumbTxtr->getSize().x),
                           ThumbnailSize.y / static_cast<float>(thumbTxtr->getSize().y));
    }
}

void StorageSystem::updatePeoplemonInfo(const core::pplmn::OwnedPeoplemon& ppl) {
    nickname.setString(ppl.name());
    nickname.setOrigin(nickname.getGlobalBounds().width * 0.5f, 0.f);
    level.setString("Level " + std::to_string(ppl.currentLevel()));
    itemName.setString(ppl.holdItem() == core::item::Id::None ?
                           "None" :
                           core::item::Item::getName(ppl.holdItem()));
    thumbTxtr = TextureManager::load(core::pplmn::Peoplemon::thumbnailImage(ppl.id())).data;
    thumbnail.setTexture(*thumbTxtr, true);
    thumbnail.setScale(ThumbnailSize.x / static_cast<float>(thumbTxtr->getSize().x),
                       ThumbnailSize.y / static_cast<float>(thumbTxtr->getSize().y));
}

void StorageSystem::onSelect(const sf::Vector2i& pos) {
    selectPos = pos;

    switch (state) {
    case MenuState::PlacingPeoplemon:
        systems.player().state().storage.add(
            currentBox, pos, systems.player().state().peoplemon[depositedPeoplemon]);
        systems.player().state().peoplemon.erase(systems.player().state().peoplemon.begin() +
                                                 depositedPeoplemon);
        activeGrid.update(systems.player().state().storage.getBox(currentBox));
        cursor.setHolding(core::pplmn::Id::Unknown);
        enterState(MenuState::BrowsingBox);
        onCursor(cursor.getPosition());
        break;
    case MenuState::BrowsingBox:
        if (hovered != nullptr) { enterState(MenuState::BrowseMenuOpen); }
        break;
    case MenuState::MovingPeoplemon:
        if (hovered) {
            hovered = systems.player().state().storage.move(*hovered, currentBox, pos);
            activeGrid.update(systems.player().state().storage.getBox(currentBox));
        }
        cursor.setHolding(core::pplmn::Id::Unknown);
        enterState(MenuState::BrowsingBox);
        break;
    default:
        break;
    }
}

void StorageSystem::boxLeft() {
    if (currentBox == 0) return;
    --currentBox;
    slideVel = SlideVel;
    cursor.setX(core::player::StorageSystem::BoxWidth - 1);
    finishBoxChange();
    enterState(MenuState::BoxSliding);
}

void StorageSystem::boxRight() {
    if (currentBox == core::player::StorageSystem::BoxCount - 1) return;
    ++currentBox;
    slideVel = -SlideVel;
    cursor.setX(0);
    finishBoxChange();
    enterState(MenuState::BoxSliding);
}

void StorageSystem::finishBoxChange() {
    boxTitle.setString("Storage Box " + std::to_string(currentBox + 1));
    boxTitle.setOrigin(boxTitle.getGlobalBounds().width * 0.5f,
                       boxTitle.getGlobalBounds().height * 0.5f);
    slideOffset    = 0.f;
    slidingOutGrid = std::move(activeGrid);
    activeGrid.update(systems.player().state().storage.getBox(currentBox));
}

void StorageSystem::showContextMessage(const std::string& msg, bool cm) {
    closeMenuAfterMessage = cm;
    systems.hud().displayMessage(msg, std::bind(&StorageSystem::onMessageDone, this));
    enterState(MenuState::WaitingContextMessage);
}

void StorageSystem::onWithdraw() {
    if (systems.player().state().peoplemon.size() == 6) {
        showContextMessage("Your party is full! Ditch some other loser to make room.", false);
    }
    else {
        showContextMessage(hovered->peoplemon.name() + " was added to your party!");
        systems.player().state().peoplemon.emplace_back(hovered->peoplemon);
        systems.player().state().storage.remove(currentBox, selectPos);
        activeGrid.update(systems.player().state().storage.getBox(currentBox));
    }
}

void StorageSystem::onStartMove() {
    ogMovePos         = cursor.getPosition();
    hovered->position = {-5, -5};
    cursor.setHolding(hovered->peoplemon.id());
    activeGrid.update(systems.player().state().storage.getBox(currentBox));
    enterState(MenuState::MovingPeoplemon);
}

void StorageSystem::onTakeItem() {
    if (hovered->peoplemon.holdItem() != core::item::Id::None) {
        showContextMessage(hovered->peoplemon.name() + " had their " +
                           core::item::Item::getName(hovered->peoplemon.holdItem()) +
                           " ripped from their hands!");
        systems.player().state().bag.addItem(hovered->peoplemon.holdItem());
        hovered->peoplemon.holdItem() = core::item::Id::None;
    }
    else {
        showContextMessage(hovered->peoplemon.name() + " isn't holding anything!", false);
    }
}

void StorageSystem::onRelease() {
    systems.hud().promptUser(
        "Are you sure you want to tell " + hovered->peoplemon.name() + " to go away forever?",
        {"Yes", "No"},
        std::bind(&StorageSystem::onReleaseConfirm, this, std::placeholders::_1));
    enterState(MenuState::WaitingReleaseConfirm);
}

void StorageSystem::onCloseContextMenu() {
    bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuBackSound());
    enterState(MenuState::BrowsingBox);
}

bool StorageSystem::observe(const bl::input::Actor&, unsigned int cmd, bl::input::DispatchType,
                            bool eventTriggered) {
    bl::menu::Menu* toSend = nullptr;
    if (cmd == core::input::Control::Back) {
        if (!eventTriggered) return true;

        switch (state) {
        case MenuState::ChooseAction:
            close();
            break;
        case MenuState::PlacingPeoplemon:
            cursor.setHolding(core::pplmn::Id::Unknown);
            enterState(MenuState::ChooseAction);
            break;
        case MenuState::BrowsingBox:
            enterState(MenuState::ChooseAction);
            break;
        case MenuState::MovingPeoplemon:
            if (hovered) {
                hovered->position = ogMovePos;
                activeGrid.update(systems.player().state().storage.getBox(currentBox));
            }
            cursor.setHolding(core::pplmn::Id::Unknown);
            [[fallthrough]];
        case MenuState::BrowseMenuOpen:
            enterState(MenuState::BrowsingBox);
            onCursor(cursor.getPosition());
            break;
        default:
            break;
        }
        return true;
    }

    switch (state) {
    case MenuState::ChooseAction:
        toSend = &actionMenu;
        break;

    case MenuState::PlacingPeoplemon:
    case MenuState::BrowsingBox:
    case MenuState::MovingPeoplemon:
        if (cmd == core::input::Control::Interact) { onSelect(cursor.getPosition()); }
        else if (cmd == core::input::Control::MoveLeft && cursor.getPosition().x == 0) {
            if (currentBox > 0) {
                boxLeft();
                bl::audio::AudioSystem::playOrRestartSound(pageSlideSound);
            }
            else {
                bl::audio::AudioSystem::playOrRestartSound(pageSlideFailSound);
            }
        }
        else if (cmd == core::input::Control::MoveRight &&
                 cursor.getPosition().x == core::player::StorageSystem::BoxWidth - 1) {
            if (currentBox < core::player::StorageSystem::BoxCount - 1) {
                boxRight();
                bl::audio::AudioSystem::playOrRestartSound(pageSlideSound);
            }
            else {
                bl::audio::AudioSystem::playOrRestartSound(pageSlideFailSound);
            }
        }
        else {
            if (cursor.process(cmd, eventTriggered)) {
                onCursor(cursor.getPosition());
                bl::audio::AudioSystem::playOrRestartSound(cursorMoveSound);
                enterState(MenuState::CursorMoving);
            }
            else {
                bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveFailSound());
            }
        }
        break;

    case MenuState::BrowseMenuOpen:
        toSend = &contextMenu;
        break;

    default:
        break;
    }

    if (toSend != nullptr) {
        menuDriver.drive(toSend);
        menuDriver.sendControl(cmd, eventTriggered);
    }

    return true;
}

void StorageSystem::enterState(MenuState ns) {
    prevState = state;
    state     = ns;

    switch (ns) {
    case MenuState::BrowseMenuOpen:
        contextMenu.setPosition(BoxPosition +
                                sf::Vector2f(cursor.getPosition() + sf::Vector2i{1, 1}) *
                                    menu::StorageCursor::TileSize() +
                                sf::Vector2f(5.f, 5.f));
        break;
    default:
        break;
    }
    if (hovered != nullptr) { updatePeoplemonInfo(hovered->peoplemon); }
}

void StorageSystem::onReleaseConfirm(const std::string& c) {
    if (c == "Yes") {
        showContextMessage(hovered->peoplemon.name() +
                           " was released and will probably end up on the streets.");
        systems.player().state().storage.remove(currentBox, selectPos);
        activeGrid.update(systems.player().state().storage.getBox(currentBox));
    }
    else {
        enterState(MenuState::BrowseMenuOpen);
    }
}

void StorageSystem::onMessageDone() {
    enterState(closeMenuAfterMessage ? MenuState::BrowsingBox : MenuState::BrowseMenuOpen);
    onCursor(cursor.getPosition());
}

} // namespace state
} // namespace game
