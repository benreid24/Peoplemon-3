#include <Game/States/StorageSystem.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Events/StorageSystem.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Game/States/PeoplemonMenu.hpp>

namespace game
{
namespace state
{
namespace
{
const sf::Vector2f BoxTitlePosition(365.f, 70.f);
const sf::Vector2f BoxTitleSize(701.f - BoxTitlePosition.x, 130.f - BoxTitlePosition.y);
const sf::Vector2f BoxPosition(283.f, 148.f);
const sf::Vector2f BoxSize(784.f - BoxPosition.x, 581.f - BoxPosition.y);
const sf::Vector2f ThumbnailPosition(68.f, 248.f);
const sf::Vector2f ThumbnailSize(218.f - ThumbnailPosition.x, 434.f - ThumbnailPosition.y);
const sf::Vector2f NamePosition(ThumbnailPosition.x + ThumbnailSize.x * 0.5f, ThumbnailPosition.y);
const sf::Vector2f LevelPosition(ThumbnailPosition.x + 4.f, 480.f);
const sf::Vector2f ItemPosition(LevelPosition.x, LevelPosition.y + 50.f);
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
, contextMenu(bl::menu::ArrowSelector::create(7.f, sf::Color::Black)) {
    auto& textures = bl::engine::Resources::textures();
    auto joinPath  = bl::util::FileUtil::joinPath;
    using bl::menu::Item;
    using bl::menu::TextItem;
    using core::Properties;

    backgroundTxtr =
        textures.load(joinPath(Properties::MenuImagePath(), "StorageSystem/storageBGND.png")).data;
    background.setTexture(*backgroundTxtr, true);

    leftArrowTxtr =
        textures.load(joinPath(Properties::MenuImagePath(), "StorageMenu/storageArrowLeft.png"))
            .data;
    leftArrow.setTexture(*leftArrowTxtr, true);
    rightArrowTxtr =
        textures.load(joinPath(Properties::MenuImagePath(), "StorageMenu/storageArrowRight.png"))
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

    nickname.setFont(Properties::MenuFont());
    nickname.setFillColor(sf::Color(240, 40, 50));
    nickname.setPosition(NamePosition);
    nickname.setCharacterSize(30);
    level.setFont(Properties::MenuFont());
    level.setFillColor(sf::Color(30, 130, 245));
    level.setPosition(LevelPosition);
    level.setCharacterSize(26);
    item.setFont(Properties::MenuFont());
    item.setFillColor(sf::Color(200, 255, 255));
    item.setPosition(ItemPosition);
    item.setCharacterSize(26);

    TextItem::Ptr depositItem =
        TextItem::create("Deposit", Properties::MenuFont(), sf::Color::Black, 34);
    depositItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::startDeposit, this));
    TextItem::Ptr browseItem =
        TextItem::create("Withdraw", Properties::MenuFont(), sf::Color::Black, 34);
    browseItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&StorageSystem::startBrowse, this));
    TextItem::Ptr closeItem =
        TextItem::create("Close", Properties::MenuFont(), sf::Color::Black, 34);
    closeItem->getSignal(Item::Activated).willAlwaysCall(std::bind(&StorageSystem::close, this));
    actionMenu.setRootItem(depositItem);
    actionMenu.addItem(browseItem, depositItem.get(), Item::Bottom);
    actionMenu.addItem(closeItem, browseItem.get(), Item::Bottom);
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
    withdrawItem->getSignal(Item::Activated)
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
}

const char* StorageSystem::name() const { return "StorageSystem"; }

void StorageSystem::activate(bl::engine::Engine& engine) {
    systems.player().inputSystem().addListener(*this);

    // create view for menu
    oldView = engine.window().getView();
    sf::View view(oldView);
    const sf::Vector2f size(background.getGlobalBounds().width,
                            background.getGlobalBounds().height);
    view.setCenter(size * 0.5f);
    view.setSize(size);
    engine.window().setView(view);
    boxView = bl::interface::ViewUtil::computeSubView({BoxPosition, BoxSize}, view);

    if (state == MenuState::WaitingDeposit) {
        // TODO - check if deposit chosen and enter Placing state
        updatePeoplemonInfo(systems.player().state().peoplemon[depositedPeoplemon]);
        enterState(MenuState::ChooseAction);
    }
    else {
        // TODO - first time setup?
    }
}

void StorageSystem::deactivate(bl::engine::Engine&) {
    systems.player().inputSystem().removeListener(*this);
    systems.engine().eventBus().dispatch<core::event::StorageSystemClosed>({});
}

void StorageSystem::update(bl::engine::Engine&, float dt) {
    systems.player().update();
    // TODO - update cursor

    switch (state) {
    case MenuState::BoxSliding:
        // TODO - update slide
        break;

    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        systems.hud().update(dt);
        break;

    default:
        break;
    }
}

void StorageSystem::render(bl::engine::Engine& engine, float) {
    engine.window().clear();

    engine.window().draw(background);
    engine.window().draw(boxTitle);
    if (currentBox > 0) { engine.window().draw(leftArrow); }
    if (currentBox < 13) { engine.window().draw(rightArrow); }

    engine.window().draw(thumbnail);
    if (hovered != nullptr) {
        engine.window().draw(nickname);
        engine.window().draw(level);
        engine.window().draw(item);
    }

    // TODO - draw box contents and cursor

    actionMenu.render(engine.window());
    switch (state) {
    case MenuState::BoxSliding:
        // TODO - draw sliding out box state
        break;

    case MenuState::BrowseMenuOpen:
    case MenuState::WaitingContextMessage:
    case MenuState::WaitingReleaseConfirm:
        contextMenu.render(engine.window());
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

void StorageSystem::onHover(core::pplmn::StoredPeoplemon* ppl) {
    if (state == MenuState::PlacingPeoplemon) return;

    hovered = ppl;
    if (ppl != nullptr) { updatePeoplemonInfo(ppl->peoplemon); }
    else {
        thumbTxtr = bl::engine::Resources::textures()
                        .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                           "StorageMenu/question.png"))
                        .data;
        thumbnail.setTexture(*thumbTxtr, true);
        thumbnail.setScale(ThumbnailSize.x / static_cast<float>(thumbTxtr->getSize().x),
                           ThumbnailSize.y / static_cast<float>(thumbTxtr->getSize().y));
    }
}

void StorageSystem::updatePeoplemonInfo(const core::pplmn::OwnedPeoplemon& ppl) {
    nickname.setString(ppl.name());
    level.setString("Level " + std::to_string(ppl.currentLevel()));
    item.setString("Hold item: " + (ppl.holdItem() == core::item::Id::None ?
                                        "None" :
                                        core::item::Item::getName(ppl.holdItem())));
    thumbnail.setTexture(*thumbTxtr, true);
    thumbTxtr = bl::engine::Resources::textures()
                    .load(core::pplmn::Peoplemon::thumbnailImage(ppl.id()))
                    .data;
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
        enterState(MenuState::BrowsingBox);
        break;
    case MenuState::BrowsingBox:
        if (hovered != nullptr) { enterState(MenuState::BrowseMenuOpen); }
        break;
    default:
        break;
    }
}

void StorageSystem::boxLeft() {
    if (currentBox == 0) return;
    // TODO - start slide
    enterState(MenuState::BoxSliding);
    finishBoxChange();
}

void StorageSystem::boxRight() {
    if (currentBox == 0) return;
    // TODO - start slide
    enterState(MenuState::BoxSliding);
    finishBoxChange();
}

void StorageSystem::finishBoxChange() {
    boxTitle.setString("Storage Box " + std::to_string(currentBox + 1));
    boxTitle.setOrigin(boxTitle.getGlobalBounds().width * 0.5f,
                       boxTitle.getGlobalBounds().height * 0.5f);
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
    }
}

void StorageSystem::onStartMove() { enterState(MenuState::MovingPeoplemon); }

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
    // TODO - play sound?
    enterState(MenuState::BrowsingBox);
}

void StorageSystem::process(core::component::Command cmd) {
    // TODO - handle it
}

void StorageSystem::enterState(MenuState ns) {
    state = ns;
    // TODO - other stuff?
    // like position context menu
    // also update ppl info if diff/no longer there
}

void StorageSystem::onReleaseConfirm(const std::string& c) {
    if (c == "Yes") {
        showContextMessage(hovered->peoplemon.name() +
                           " was released and will probably end up on the streets.");
        systems.player().state().storage.remove(currentBox, selectPos);
    }
    else {
        enterState(MenuState::BrowseMenuOpen);
    }
}

void StorageSystem::onMessageDone() {
    enterState(closeMenuAfterMessage ? MenuState::BrowsingBox : MenuState::BrowseMenuOpen);
}

} // namespace state
} // namespace game
