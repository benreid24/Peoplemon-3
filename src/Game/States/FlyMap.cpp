#include <Game/States/FlyMap.hpp>

#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float FlashOn  = 0.3f;
constexpr float FlashOff = 0.4f;
} // namespace

bl::engine::State::Ptr FlyMap::create(core::system::Systems& s, bool& up) {
    return Ptr{new FlyMap(s, up)};
}

FlyMap::FlyMap(core::system::Systems& s, bool& up)
: State(s, bl::engine::StateMask::Menu)
, unpause(up)
, selected(std::numeric_limits<unsigned int>::max()) {
    const auto& joinPath = bl::util::FileUtil::joinPath;
    const auto& ImgPath  = core::Properties::MenuImagePath();
    auto& tp             = s.engine().renderer().texturePool();

    townMenu.create(s.engine(),
                    s.engine().renderer().getObserver(),
                    bl::menu::ArrowSelector::create(8.f, sf::Color::Black));

    mapTxtr = tp.getOrLoadTexture(joinPath(ImgPath, "FlyMap/background.png"));
    map.create(s.engine(), mapTxtr);

    townTxtr = tp.getOrLoadTexture(joinPath(ImgPath, "FlyMap/town.png"));
    towns.create(s.engine(), townTxtr);
    towns.setParent(map);

    cursorTxtr = tp.getOrLoadTexture(joinPath(ImgPath, "FlyMap/cursor.png"));
    cursor.create(s.engine(), cursorTxtr);
    cursor.getTransform().setOrigin(cursorTxtr->size() * 0.5f);
    cursor.setParent(map);

    if (systems.world().activeMap().canFlyFromHere()) {
        const sf::Vector2f ws = systems.world().activeMap().sizePixels();
        const glm::vec2 pp =
            systems.player().position().getWorldPosition(core::Properties::PixelsPerTile());
        const glm::vec2 ms(mapTxtr->size());
        const std::string path =
            std::string("FlyMap/") + (systems.player().state().sex == core::player::Gender::Boy ?
                                          "boyHead.png" :
                                          "girlHead.png");
        playerTxtr = tp.getOrLoadTexture(joinPath(ImgPath, path));
        player.create(s.engine(), playerTxtr);
        player.getTransform().setOrigin(playerTxtr->size() * 0.5f);
        player.getTransform().setPosition(pp.x / ws.x * ms.x, pp.y / ws.y * ms.y);
        player.setParent(map);
    }

    panelTxtr = tp.getOrLoadTexture(joinPath(ImgPath, "FlyMap/sidePanel.png"));
    panel.create(s.engine(), panelTxtr);
    panel.getTransform().setPosition(mapTxtr->size().x, 0.f);

    townName.create(s.engine(), core::Properties::MenuFont(), "", 20, sf::Color(20, 40, 65));
    townName.getTransform().setPosition(100.f, 375.f);
    townName.wordWrap(180.f);
    townName.setParent(panel);

    townDesc.create(s.engine(), core::Properties::MenuFont(), "", 16, sf::Color::Black);
    townDesc.wordWrap(180.f);
    townDesc.getTransform().setPosition(10.f, 415.f);
    townDesc.setParent(panel);

    const auto& visited = systems.player().state().visitedTowns;
    bl::menu::Item::Ptr back =
        bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 18);
    back->getSignal(bl::menu::Item::Selected).willAlwaysCall(std::bind(&FlyMap::clearHover, this));
    back->getSignal(bl::menu::Item::Activated).willAlwaysCall(std::bind(&FlyMap::close, this));
    townMenu.setRootItem(back);

    bl::menu::Item* prev = back.get();
    bl::gfx::BatchSprite townSprite;
    townSprite.disableAutoCommit(true);
    const sf::FloatRect src(0.f, 0.f, townTxtr->size().x, townTxtr->size().y);
    for (unsigned int i = 0; i < core::map::Map::FlyMapTowns().size(); ++i) {
        const auto& t = core::map::Map::FlyMapTowns()[i];
        if (visited.find(t.name) != visited.end() && t.mapPos != sf::Vector2i{} && t.pcSpawn > 0) {
            bl::menu::Item::Ptr it = bl::menu::TextItem::create(
                t.name, core::Properties::MenuFont(), sf::Color::Black, 18);
            it->getSignal(bl::menu::Item::Selected)
                .willAlwaysCall(std::bind(&FlyMap::hoverTown, this, i));
            it->getSignal(bl::menu::Item::Activated)
                .willAlwaysCall(std::bind(&FlyMap::selectTown, this, i));
            townMenu.addItem(it, prev, bl::menu::Item::Top);
            prev     = it.get();
            selected = i;
        }

        townSprite.create(s.engine(), towns, src);
        townSprite.getLocalTransform().setPosition(t.mapPos.x, t.mapPos.y);
        townSprite.orphan();
    }
    if (prev != back.get()) { townMenu.attachExisting(prev, back.get(), bl::menu::Item::Bottom); }
    townMenu.setSelectedItem(prev);
    townMenu.setPosition({8.f, 4.f});
    townMenu.setMaximumSize({200.f, 316.f});
    inputDriver.drive(&townMenu);
}

const char* FlyMap::name() const { return "FlyMap"; }

void FlyMap::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    map.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    towns.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    if (player.entity() != bl::ecs::InvalidEntity) {
        player.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    }
    cursor.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    panel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    townName.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    townDesc.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    townMenu.addToOverlay(panel.entity());
    if (selected < core::map::Map::FlyMapTowns().size()) { hoverTown(selected); }
}

void FlyMap::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    engine.renderer().getObserver().popScene();
}

void FlyMap::update(bl::engine::Engine&, float, float) {}

bool FlyMap::observe(const bl::input::Actor&, unsigned int activatedControl,
                     bl::input::DispatchType, bool fromEvent) {
    if (activatedControl == core::input::Control::Back) { systems.engine().popState(); }
    else { inputDriver.sendControl(activatedControl, fromEvent); }
    return true;
}

void FlyMap::clearHover() {
    townName.getSection().setString("");
    townDesc.getSection().setString("Select a location to fly to.");
    cursor.stopFlashing();
    cursor.setHidden(true);
}

void FlyMap::hoverTown(unsigned int i) {
    const auto& t = core::map::Map::FlyMapTowns()[i];
    townName.getSection().setString(t.name);
    townDesc.getSection().setString(t.description);
    cursor.getTransform().setPosition(t.mapPos.x, t.mapPos.y);
    cursor.setHidden(false);
    cursor.flash(FlashOn, FlashOff);
    townName.getTransform().setOrigin(townName.getLocalBounds().width * 0.5f,
                                      townName.getLocalBounds().height * 0.5f);
}

void FlyMap::selectTown(unsigned int i) {
    const auto t = core::map::Map::FlyMapTowns()[i];
    if (systems.world().activeMap().canFlyFromHere()) {
        if (systems.player().state().bag.hasItem(core::item::Id::Teleporter)) {
            systems.hud().promptUser(
                "Fly to " + t.name + "?",
                {"Yes", "No"},
                std::bind(&FlyMap::onFlyChoice, this, std::placeholders::_1, t));
        }
        else {
            systems.hud().displayMessage("One day you may be able to quickly travel if you acquire "
                                         "the right item... one day.",
                                         std::bind(&FlyMap::messageDone, this));
        }
    }
    else {
        systems.hud().displayMessage("You cannot fly from here!",
                                     std::bind(&FlyMap::messageDone, this));
    }
}

void FlyMap::onFlyChoice(const std::string& c, const core::map::Town& town) {
    if (c == "Yes") {
        if (systems.flight().startFlight(town.pcSpawn)) {
            unpause = true;
            close();
        }
        else {
            systems.hud().displayMessage("You cannot fly from here!",
                                         std::bind(&FlyMap::messageDone, this));
        }
    }
}

void FlyMap::messageDone() {}

void FlyMap::close() { systems.engine().popState(); }

} // namespace state
} // namespace game
