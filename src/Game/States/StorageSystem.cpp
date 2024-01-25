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
constexpr glm::vec2 BoxTitlePosition(365.f, 70.f);
constexpr glm::vec2 BoxTitleSize(701.f - BoxTitlePosition.x, 130.f - BoxTitlePosition.y);
constexpr glm::vec2 ThumbnailPosition(68.f, 284.f);
constexpr glm::vec2 ThumbnailSize(218.f - ThumbnailPosition.x, 434.f - ThumbnailPosition.y);
constexpr glm::vec2 NamePosition(ThumbnailPosition.x + ThumbnailSize.x * 0.5f,
                                 ThumbnailPosition.y + ThumbnailSize.y + 4.f);
constexpr glm::vec2 LevelPosition(ThumbnailPosition.x + 4.f, 475.f);
constexpr glm::vec2 ItemLabelPosition(LevelPosition.x, LevelPosition.y + 36.f);
constexpr glm::vec2 ItemPosition(ItemLabelPosition.x, ItemLabelPosition.y + 25.f);
constexpr float SlideTime = 0.5f;
constexpr float SlideVel  = menu::StorageGrid::BoxSize.x / SlideTime;
} // namespace

bl::engine::State::Ptr StorageSystem::create(core::system::Systems& s) {
    return bl::engine::State::Ptr(new StorageSystem(s));
}

StorageSystem::StorageSystem(core::system::Systems& s)
: State(s, bl::engine::StateMask::Menu)
, state(MenuState::ChooseAction)
, currentBox(0)
, overlay(nullptr)
, grids{{s.engine()}, {s.engine()}}
, activeGrid(&grids[0])
, slidingOutGrid(&grids[1])
, hovered(nullptr)
, cursor(s.engine())
, depositedPeoplemon(-1) {
    auto joinPath = bl::util::FileUtil::joinPath;
    using bl::menu::Item;
    using bl::menu::TextItem;
    using core::Properties;

    actionMenu.create(s.engine(),
                      s.engine().renderer().getObserver(),
                      bl::menu::ArrowSelector::create(12.f, sf::Color::Black));
    contextMenu.create(s.engine(),
                       s.engine().renderer().getObserver(),
                       bl::menu::ArrowSelector::create(12.f, sf::Color::Black));

    backgroundTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(Properties::MenuImagePath(), "StorageSystem/storageBGND.png"));
    background.create(s.engine(), backgroundTxtr);
    grids[0].activate(background.entity());
    grids[1].activate(background.entity());

    leftArrowTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(Properties::MenuImagePath(), "StorageSystem/storageArrowLeft.png"));
    leftArrow.create(s.engine(), leftArrowTxtr);
    leftArrow.getTransform().setPosition(BoxTitlePosition.x - leftArrowTxtr->size().x - 3.f,
                                         BoxTitlePosition.y + BoxTitleSize.y * 0.5f -
                                             leftArrowTxtr->size().y * 0.5f);
    leftArrow.setParent(background);

    rightArrowTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(Properties::MenuImagePath(), "StorageSystem/storageArrowRight.png"));
    rightArrow.create(s.engine(), rightArrowTxtr);
    rightArrow.getTransform().setPosition(BoxTitlePosition.x + BoxTitleSize.x + 3.f,
                                          BoxTitlePosition.y + BoxTitleSize.y * 0.5f -
                                              rightArrowTxtr->size().y * 0.5f);
    rightArrow.setParent(background);

    boxTitle.create(s.engine(), core::Properties::MenuFont(), "", 42, sf::Color::Black);
    boxTitle.getTransform().setPosition(BoxTitlePosition + BoxTitleSize * 0.5f);
    boxTitle.setParent(background);

    nickname.create(s.engine(), core::Properties::MenuFont(), "", 26, sf::Color(240, 40, 50));
    nickname.getTransform().setPosition(NamePosition);
    nickname.setParent(background);

    level.create(s.engine(), core::Properties::MenuFont(), "", 22, sf::Color(35, 160, 245));
    level.getTransform().setPosition(LevelPosition);
    level.setParent(background);

    itemLabel.create(
        s.engine(), core::Properties::MenuFont(), "Hold item:", 16, sf::Color(200, 255, 255));
    itemLabel.addSection("", 16, sf::Color(165, 255, 255));
    itemLabel.getTransform().setPosition(ItemLabelPosition);
    itemLabel.setParent(background);

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
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {20.f, 2.f, 4.f, 4.f});

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
    contextMenu.configureBackground(sf::Color::White, sf::Color::Black, 2.f, {20.f, 2.f, 4.f, 4.f});

    pageSlideFailSound = core::Properties::MenuMoveFailSound();
    pageSlideSound     = bl::audio::AudioSystem::getOrLoadSound(
        joinPath(Properties::SoundPath(), "Menu/storageBoxChange.mp3"));
    cursorMoveSound = bl::audio::AudioSystem::getOrLoadSound(
        joinPath(Properties::SoundPath(), "Menu/storageCursorMove.mp3"));
}

const char* StorageSystem::name() const { return "StorageSystem"; }

void StorageSystem::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    leftArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    rightArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    boxTitle.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    nickname.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    level.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    itemLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    if (thumbnail.entity() != bl::ecs::InvalidEntity) {
        thumbnail.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    }
    actionMenu.addToOverlay(background.entity());
    contextMenu.addToOverlay(background.entity());
    cursor.activate(background.entity());
    activeGrid->activate(background.entity());
    slidingOutGrid->activate(background.entity());

    hovered = systems.player().state().storage.get(currentBox, cursor.getPosition());
    if (state == MenuState::WaitingDeposit) {
        if (depositedPeoplemon >= 0) {
            actionMenu.setSelectedItem(withdrawActionItem.get());
            const auto& ppl = systems.player().state().peoplemon[depositedPeoplemon];
            hovered         = nullptr;
            updatePeoplemonInfo(ppl);
            cursor.setHolding(ppl.id());
            enterState(MenuState::PlacingPeoplemon);
            showPeoplemonInfo(true);
        }
        else {
            showPeoplemonInfo(false);
            enterState(MenuState::ChooseAction);
        }
    }
    else {
        showPeoplemonInfo(false);
        enterState(MenuState::ChooseAction);
    }

    finishBoxChange();
    if (hovered != nullptr) { updatePeoplemonInfo(hovered->peoplemon); }

    contextMenu.setHidden(true);
}

void StorageSystem::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    bl::event::Dispatcher::dispatch<core::event::StorageSystemClosed>({});
    cursor.deactivate();
    activeGrid->deactivate();
    engine.renderer().getObserver().popScene();
    overlay = nullptr;
}

void StorageSystem::update(bl::engine::Engine&, float dt, float) {
    cursor.update(dt);

    switch (state) {
    case MenuState::BoxSliding:
        slideOffset += slideVel * dt;
        activeGrid->notifyOffset(slideOffset + menu::StorageGrid::BoxSize.x * slideVel /
                                                   std::abs(slideVel) * -1.f);
        slidingOutGrid->notifyOffset(slideOffset);
        if (std::abs(slideOffset) >= menu::StorageGrid::BoxSize.x) {
            slidingOutGrid->deactivate();
            activeGrid->notifyOffset(0.f);
            enterState(prevState);
        }
        break;

    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        break;

    case MenuState::CursorMoving:
        if (!cursor.moving()) { enterState(prevState); }
        break;

    default:
        break;
    }
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
        updatePeoplemonInfo(ppl->peoplemon);
        showPeoplemonInfo(true);
    }
    else { showPeoplemonInfo(false); }
}

void StorageSystem::updatePeoplemonInfo(const core::pplmn::OwnedPeoplemon& ppl) {
    nickname.getSection().setString(ppl.name());
    nickname.getTransform().setOrigin(nickname.getLocalBounds().width * 0.5f, 0.f);
    level.getSection().setString("Level " + std::to_string(ppl.currentLevel()));
    itemLabel.getSection(1).setString(ppl.holdItem() == core::item::Id::None ?
                                          "None" :
                                          core::item::Item::getName(ppl.holdItem()));
    updateThumbnail(core::pplmn::Peoplemon::thumbnailImage(ppl.id()));
}

void StorageSystem::updateThumbnail(const std::string& src) {
    thumbTxtr = systems.engine().renderer().texturePool().getOrLoadTexture(src);
    if (thumbnail.entity() == bl::ecs::InvalidEntity) {
        thumbnail.create(systems.engine(), thumbTxtr);
        thumbnail.getTransform().setPosition(ThumbnailPosition);
        thumbnail.setParent(background);
        if (overlay) { thumbnail.addToScene(overlay, bl::rc::UpdateSpeed::Static); }
    }
    else { thumbnail.setTexture(thumbTxtr); }
    thumbnail.scaleToSize(ThumbnailSize);
}

void StorageSystem::onSelect(const sf::Vector2i& pos) {
    selectPos = pos;

    switch (state) {
    case MenuState::PlacingPeoplemon:
        if (!systems.player().state().storage.get(currentBox, pos)) {
            systems.player().state().storage.add(
                currentBox, pos, systems.player().state().peoplemon[depositedPeoplemon]);
            systems.player().state().peoplemon.erase(systems.player().state().peoplemon.begin() +
                                                     depositedPeoplemon);
            activeGrid->update(systems.player().state().storage.getBox(currentBox));
            cursor.setHolding(core::pplmn::Id::Unknown);
            enterState(MenuState::BrowsingBox);
            onCursor(cursor.getPosition());
        }
        else { bl::audio::AudioSystem::playOrRestartSound(core::Properties::MenuMoveFailSound()); }
        break;
    case MenuState::BrowsingBox:
        if (hovered != nullptr) { enterState(MenuState::BrowseMenuOpen); }
        break;
    case MenuState::MovingPeoplemon:
        if (hovered) {
            hovered->position = ogMovePos; // to properly move
            hovered           = systems.player().state().storage.move(*hovered, currentBox, pos);
            activeGrid->update(systems.player().state().storage.getBox(currentBox));
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
    boxTitle.getSection().setString("Storage Box " + std::to_string(currentBox + 1));
    boxTitle.getTransform().setOrigin(boxTitle.getLocalBounds().width * 0.5f,
                                      boxTitle.getLocalBounds().height * 0.5f);
    slideOffset = 0.f;
    leftArrow.setHidden(currentBox == 0);
    rightArrow.setHidden(currentBox == 13);

    std::swap(activeGrid, slidingOutGrid);
    activeGrid->update(systems.player().state().storage.getBox(currentBox));
    activeGrid->notifyOffset(0.f);
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
        activeGrid->update(systems.player().state().storage.getBox(currentBox));
    }
}

void StorageSystem::onStartMove() {
    ogMovePos         = cursor.getPosition();
    hovered->position = {-5, -5};
    cursor.setHolding(hovered->peoplemon.id());
    activeGrid->update(systems.player().state().storage.getBox(currentBox));
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
    else { showContextMessage(hovered->peoplemon.name() + " isn't holding anything!", false); }
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
                activeGrid->update(systems.player().state().storage.getBox(currentBox));
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
        if (cmd == core::input::Control::Interact) {
            if (eventTriggered) { onSelect(cursor.getPosition()); }
        }
        else if (cmd == core::input::Control::MoveLeft && cursor.getPosition().x == 0) {
            if (currentBox > 0) {
                boxLeft();
                bl::audio::AudioSystem::playOrRestartSound(pageSlideSound);
            }
            else { bl::audio::AudioSystem::playOrRestartSound(pageSlideFailSound); }
        }
        else if (cmd == core::input::Control::MoveRight &&
                 cursor.getPosition().x == core::player::StorageSystem::BoxWidth - 1) {
            if (currentBox < core::player::StorageSystem::BoxCount - 1) {
                boxRight();
                bl::audio::AudioSystem::playOrRestartSound(pageSlideSound);
            }
            else { bl::audio::AudioSystem::playOrRestartSound(pageSlideFailSound); }
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

    cursor.setHidden(ns == MenuState::ChooseAction || ns == MenuState::BoxSliding);
    contextMenu.setHidden(true);

    switch (ns) {
    case MenuState::BrowseMenuOpen:
        positionContextMenu();
        [[fallthrough]];

    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        contextMenu.setHidden(false);
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
        activeGrid->update(systems.player().state().storage.getBox(currentBox));
    }
    else { enterState(MenuState::BrowseMenuOpen); }
}

void StorageSystem::onMessageDone() {
    enterState(closeMenuAfterMessage ? MenuState::BrowsingBox : MenuState::BrowseMenuOpen);
    onCursor(cursor.getPosition());
}

void StorageSystem::showPeoplemonInfo(bool s) {
    const bool hidden = !s;
    nickname.setHidden(hidden);
    level.setHidden(hidden);
    itemLabel.setHidden(hidden);
    if (hidden) {
        updateThumbnail(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                     "StorageSystem/question.png"));
    }
}

void StorageSystem::positionContextMenu() {
    constexpr float Padding = 5.f;

    glm::vec2 pos(cursor.getPosition().x + 1, cursor.getPosition().y + 1);
    pos *= menu::StorageCursor::TileSize();
    pos += menu::StorageGrid::BoxPosition + glm::vec2{Padding, Padding};
    if (pos.x + contextMenu.getBounds().width > core::Properties::WindowSize().x) {
        pos.x -= contextMenu.getBounds().width + menu::StorageCursor::TileSize() + Padding * 2.f;
    }
    if (pos.y + contextMenu.getBounds().height > core::Properties::WindowSize().y) {
        pos.y -= contextMenu.getBounds().height + menu::StorageCursor::TileSize() + Padding * 4.f;
    }

    contextMenu.setPosition(pos);
}

} // namespace state
} // namespace game
