#include <Game/States/FlyMap.hpp>

#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr FlyMap::create(core::system::Systems& s, bool& up) {
    return Ptr{new FlyMap(s, up)};
}

FlyMap::FlyMap(core::system::Systems& s, bool& up)
: State(s)
, hudActive(false)
, unpause(up)
, cursorFlasher(cursor, 0.3f, 0.4f)
, townMenu(bl::menu::ArrowSelector::create(8.f, sf::Color::Black)) {
    const auto& joinPath = bl::util::FileUtil::joinPath;
    const auto& ImgPath  = core::Properties::MenuImagePath();

    mapTxtr = TextureManager::load(joinPath(ImgPath, "FlyMap/background.png"));
    map.setTexture(*mapTxtr, true);
    townTxtr = TextureManager::load(joinPath(ImgPath, "FlyMap/town.png"));
    town.setTexture(*townTxtr, true);
    town.setOrigin(sf::Vector2f(townTxtr->getSize()) * 0.5f);
    cursorTxtr = TextureManager::load(joinPath(ImgPath, "FlyMap/cursor.png"));
    cursor.setTexture(*cursorTxtr, true);
    cursor.setOrigin(sf::Vector2f(cursorTxtr->getSize()) * 0.5f);
    if (systems.world().activeMap().canFlyFromHere()) {
        const sf::Vector2f ws = systems.world().activeMap().sizePixels();
        const sf::Vector2f pp = systems.player().position().positionPixels();
        const sf::Vector2f ms(mapTxtr->getSize());
        const std::string path =
            std::string("FlyMap/") + (systems.player().state().sex == core::player::Gender::Boy ?
                                          "boyHead.png" :
                                          "girlHead.png");
        playerTxtr = TextureManager::load(joinPath(ImgPath, path));
        player.setTexture(*playerTxtr, true);
        player.setOrigin(sf::Vector2f(playerTxtr->getSize()) * 0.5f);
        player.setPosition(pp.x / ws.x * ms.x, pp.y / ws.y * ms.y);
    }

    panelTxtr = TextureManager::load(joinPath(ImgPath, "FlyMap/sidePanel.png"));
    panel.setTexture(*panelTxtr, true);
    panel.setPosition(map.getGlobalBounds().width, 0.f);
    townName.setFont(core::Properties::MenuFont());
    townName.setCharacterSize(20);
    townName.setFillColor(sf::Color(20, 40, 65));
    townName.setPosition(panel.getPosition().x + 100.f, 375.f);
    townDesc.setFont(core::Properties::MenuFont());
    townDesc.setCharacterSize(14);
    townDesc.setFillColor(sf::Color::Black);
    townDesc.setPosition(panel.getPosition().x + 10.f, 415.f);

    const auto& visited = systems.player().state().visitedTowns;
    bl::menu::Item::Ptr back =
        bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 18);
    back->getSignal(bl::menu::Item::Selected).willAlwaysCall(std::bind(&FlyMap::clearHover, this));
    back->getSignal(bl::menu::Item::Activated).willAlwaysCall(std::bind(&FlyMap::close, this));
    townMenu.setRootItem(back);

    bl::menu::Item* prev = back.get();
    townPositions.reserve(core::map::Map::FlyMapTowns().size());
    for (unsigned int i = 0; i < core::map::Map::FlyMapTowns().size(); ++i) {
        const auto& t = core::map::Map::FlyMapTowns()[i];
        if (visited.find(t.name) != visited.end() && t.mapPos != sf::Vector2i{} && t.pcSpawn > 0) {
            townPositions.emplace_back(sf::Vector2f(t.mapPos));
            bl::menu::Item::Ptr it = bl::menu::TextItem::create(
                t.name, core::Properties::MenuFont(), sf::Color::Black, 18);
            it->getSignal(bl::menu::Item::Selected)
                .willAlwaysCall(std::bind(&FlyMap::hoverTown, this, i));
            it->getSignal(bl::menu::Item::Activated)
                .willAlwaysCall(std::bind(&FlyMap::selectTown, this, i));
            townMenu.addItem(it, prev, bl::menu::Item::Top);
            prev = it.get();
        }
    }
    if (prev != back.get()) { townMenu.attachExisting(prev, back.get(), bl::menu::Item::Bottom); }
    townMenu.setSelectedItem(prev);
    townMenu.setPosition({panel.getPosition().x + 8.f, 4.f});
    townMenu.setMaximumSize({200.f, 316.f});
    inputDriver.drive(&townMenu);
}

const char* FlyMap::name() const { return "FlyMap"; }

void FlyMap::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));
}

void FlyMap::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    engine.renderSystem().cameras().popCamera();
}

void FlyMap::update(bl::engine::Engine&, float dt) {
    if (hudActive) { systems.hud().update(dt); }
    cursorFlasher.update(dt);
}

bool FlyMap::observe(const bl::input::Actor&, unsigned int activatedControl,
                     bl::input::DispatchType, bool fromEvent) {
    if (activatedControl == core::input::Control::Back) { systems.engine().popState(); }
    else { inputDriver.sendControl(activatedControl, fromEvent); }
    return true;
}

void FlyMap::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();

    engine.window().draw(map);
    for (const sf::Vector2f& pos : townPositions) {
        town.setPosition(pos);
        engine.window().draw(town);
    }
    if (!townName.getString().isEmpty()) {
        cursorFlasher.render(engine.window(), {}, lag);
        engine.window().draw(townName);
    }
    if (playerTxtr) { engine.window().draw(player); }

    engine.window().draw(panel);
    townMenu.render(engine.window());
    engine.window().draw(townDesc);
    engine.window().draw(townName);

    if (hudActive) { systems.hud().render(engine.window(), lag); }

    engine.window().display();
}

void FlyMap::clearHover() {
    townName.setString("");
    townDesc.setString("Select a location to fly to.");
    wrap();
}

void FlyMap::hoverTown(unsigned int i) {
    const auto& t = core::map::Map::FlyMapTowns()[i];
    townName.setString(t.name);
    townDesc.setString(t.description);
    cursor.setPosition(sf::Vector2f(t.mapPos));
    wrap();
    townName.setOrigin(townName.getGlobalBounds().width * 0.5f,
                       townName.getGlobalBounds().height * 0.5f);
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
    hudActive = true;
}

void FlyMap::onFlyChoice(const std::string& c, const core::map::Town& town) {
    hudActive = false;
    if (c == "Yes") {
        if (systems.flight().startFlight(town.pcSpawn)) {
            unpause = true;
            close();
        }
        else {
            systems.hud().displayMessage("You cannot fly from here!",
                                         std::bind(&FlyMap::messageDone, this));
            hudActive = true;
        }
    }
}

void FlyMap::messageDone() { hudActive = false; }

void FlyMap::close() { systems.engine().popState(); }

void FlyMap::wrap() {
    bl::interface::wordWrap(townDesc, 180.f);
    bl::interface::wordWrap(townName, 180.f);
}

} // namespace state
} // namespace game
