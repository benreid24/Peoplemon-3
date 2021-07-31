#include <Editor/Components/EditMap.hpp>

#include "MapActions.hpp"
#include <BLIB/Engine/Resources.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>

namespace editor
{
namespace component
{
namespace
{
const bl::resource::Resource<sf::Texture>::Ref colGfx[] = {
    bl::engine::Resources::textures().load("EditorResources/Collisions/none.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/all.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/top.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/right.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/bottom.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/left.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/topRight.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/bottomRight.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/bottomLeft.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/topLeft.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/topBottom.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/leftRight.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/noTop.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/noRight.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/noBottom.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/noLeft.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/water.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/fall.png").data};

const bl::resource::Resource<sf::Texture>::Ref catchGfx[] = {
    bl::engine::Resources::textures().load("EditorResources/Collisions/none.png").data,
    bl::engine::Resources::textures().load("EditorResources/Collisions/all.png").data};

const bl::resource::Resource<sf::Texture>::Ref arrowGfx =
    bl::engine::Resources::textures().load("EditorResources/arrow.png").data;

} // namespace

EditMap::Ptr EditMap::create(const PositionCb& clickCb, const PositionCb& moveCb,
                             const ActionCb& actionCb, const ActionCb& syncCb,
                             core::system::Systems& systems) {
    return Ptr(new EditMap(clickCb, moveCb, actionCb, syncCb, systems));
}

EditMap::EditMap(const PositionCb& cb, const PositionCb& mcb, const ActionCb& actionCb,
                 const ActionCb& syncCb, core::system::Systems& s)
: bl::gui::Element("maps", "editmap")
, Map()
, historyHead(0)
, saveHead(0)
, clickCb(cb)
, moveCb(mcb)
, actionCb(actionCb)
, syncCb(syncCb)
, camera(EditCamera::Ptr(new EditCamera()))
, controlsEnabled(false)
, renderOverlay(RenderOverlay::None)
, overlayLevel(0) {
    systems = &s;

    static const auto callCb = [this, &s](const PositionCb& cb) {
        static const float PixelRatio = 1.f / static_cast<float>(core::Properties::PixelsPerTile());

        const sf::Vector2i mouse  = sf::Mouse::getPosition(s.engine().window());
        const sf::Vector2f pixels = s.engine().window().mapPixelToCoords(mouse, renderView);
        const sf::Vector2i tiles(std::floor(pixels.x * PixelRatio),
                                 std::floor(pixels.y * PixelRatio));
        cb(pixels, tiles);

        for (unsigned int level = 0; level < levels.size(); ++level) {
            triggerAnimation(
                core::component::Position(level, tiles, core::component::Direction::Up));
        }
    };

    getSignal(bl::gui::Action::LeftClicked)
        .willAlwaysCall([this](const bl::gui::Action&, Element*) {
            if (controlsEnabled) { callCb(clickCb); }
        });

    getSignal(bl::gui::Action::MouseMoved).willAlwaysCall([this](const bl::gui::Action&, Element*) {
        callCb(moveCb);
    });
}

bool EditMap::editorLoad(const std::string& file) {
    if (!doLoad(file)) {
        doLoad(savefile);
        return false;
    }
    savefile = file;
    return true;
}

bool EditMap::editorSave() {
    if (save(savefile)) {
        saveHead = historyHead;
        return tileset->save(tilesetField.getValue());
    }
    return false;
}

void EditMap::newMap(const std::string& filename, const std::string& name,
                     const std::string& tileset, unsigned int width, unsigned int height) {
    clear();
    savefile     = filename;
    nameField    = name;
    tilesetField = tileset;
    levels.resize(1);
    levels.front().init(width, height, 2, 2, 1);
    transitionField.getValue().setSize(width, height, core::map::LevelTransition::None);
    editorActivate();
    syncCb();
}

bool EditMap::doLoad(const std::string& file) {
    clear();
    if (!Map::load(file)) return false;
    return editorActivate();
}

bool EditMap::editorActivate() {
    history.clear();
    historyHead = 0;
    saveHead    = 0;

    systems->engine().entities().clear();

    size = {static_cast<int>(levels.front().bottomLayers().front().width()),
            static_cast<int>(levels.front().bottomLayers().front().height())};
    systems->engine().eventBus().dispatch<core::event::MapSwitch>({*this});

    camera->reset(size);
    systems->cameras().clearAndReplace(camera);
    systems->cameras().update(0.f);
    weather.activate(systems->cameras().getArea());

    tileset = core::Resources::tilesets().load(tilesetField).data;
    if (!tileset) return false;
    tileset->activate();
    for (core::map::LayerSet& level : levels) { level.activate(*tileset); }

    weather.set(weatherField.getValue());
    lighting.activate(size);
    for (core::map::CatchZone& zone : catchZonesField.getValue()) { zone.activate(); }

    core::script::LegacyWarn::warn(loadScriptField);
    core::script::LegacyWarn::warn(unloadScriptField);

    if (!activated) {
        activated = true;
        lighting.subscribe(systems->engine().eventBus());
    }

    for (const core::map::CharacterSpawn& spawn : characterField.getValue()) {
        if (!systems->entity().spawnCharacter(spawn)) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file.getValue();
        }
    }

    for (const core::map::Item& item : itemsField.getValue()) { systems->entity().spawnItem(item); }

    systems->engine().eventBus().dispatch<core::event::MapEntered>({*this});

    levelFilter.clear();
    levelFilter.resize(levels.size(), true);
    layerFilter.clear();
    layerFilter.resize(levels.size());
    for (unsigned int i = 0; i < levels.size(); ++i) {
        layerFilter[i].resize(levels[i].layerCount(), true);
    }

    actionCb();
    return true;
}

bool EditMap::unsavedChanges() const { return saveHead != historyHead; }

void EditMap::update(float dt) { Map::update(*systems, dt); }

void EditMap::setControlsEnabled(bool e) {
    controlsEnabled = e;
    camera->enabled = e;
}

void EditMap::setLevelVisible(unsigned int level, bool v) { levelFilter[level] = v; }

void EditMap::setLayerVisible(unsigned int level, unsigned int layer, bool v) {
    layerFilter[level][layer] = v;
}

void EditMap::setRenderOverlay(RenderOverlay ro, unsigned int l) {
    renderOverlay = ro;
    overlayLevel  = l;
}

void EditMap::showSelection(const sf::IntRect& s) { selection = s; }

void EditMap::removeAllTiles(core::map::Tile::IdType id, bool anim) {
    const auto cleanLayer = [this, id, anim](core::map::TileLayer& layer) -> bool {
        bool mod = false;
        for (unsigned int x = 0; x < layer.width(); ++x) {
            for (unsigned int y = 0; y < layer.height(); ++y) {
                auto& tile = layer.getRef(x, y);
                if (tile.id() == id && tile.isAnimation() == anim) {
                    mod = true;
                    tile.set(*tileset, core::map::Tile::Blank, false);
                }
            }
        }
        return mod;
    };

    for (auto& level : levels) {
        bool needClean = false;

        for (auto& layer : level.bottomLayers()) { cleanLayer(layer); }
        for (auto& layer : level.ysortLayers()) {
            if (cleanLayer(layer)) { needClean = true; }
        }
        for (auto& layer : level.topLayers()) { cleanLayer(layer); }

        if (needClean) { level.activate(*tileset); }
    }
}

EditMap::EditCamera::EditCamera()
: enabled(false) {}

bool EditMap::EditCamera::valid() const { return true; }

void EditMap::EditCamera::update(core::system::Systems&, float dt) {
    if (enabled) {
        float PixelsPerSecond = 0.5f * size * static_cast<float>(core::Properties::WindowWidth());
        static const float ZoomPerSecond = 1.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) PixelsPerSecond *= 5.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            position.y -= PixelsPerSecond * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            position.x += PixelsPerSecond * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            position.y += PixelsPerSecond * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            position.x -= PixelsPerSecond * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) { zoom(-ZoomPerSecond * dt); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { zoom(ZoomPerSecond * dt); }
    }
}

void EditMap::EditCamera::reset(const sf::Vector2i& t) {
    position = sf::Vector2f(t) * static_cast<float>(core::Properties::PixelsPerTile()) * 0.5f;
    size     = 1.f;
}

void EditMap::EditCamera::zoom(float z) {
    size += z;
    if (size < 0.1f) size = 0.1f;
}

sf::Vector2i EditMap::minimumRequisition() const { return {100, 100}; }

void EditMap::doRender(sf::RenderTarget& target, sf::RenderStates, const bl::gui::Renderer&) const {
    const sf::View oldView = target.getView();
    renderView             = bl::gui::Container::computeView(oldView, getAcquisition());
    target.setView(renderView);
    systems->render().render(target, *this, 0.f);
    target.setView(oldView);
}

bool EditMap::handleScroll(const bl::gui::RawEvent& event) {
    if (controlsEnabled) camera->zoom(-event.event.mouseWheelScroll.delta * 0.1f);
    return true;
}

void EditMap::undo() {
    if (!history.empty() && historyHead > 0) {
        if (history[historyHead - 1]->undo(*this)) { syncCb(); }
        --historyHead;
        actionCb();
    }
}

const char* EditMap::undoDescription() const {
    if (!history.empty() && historyHead > 0) return history[historyHead - 1]->description();
    return nullptr;
}

void EditMap::redo() {
    if (historyHead != history.size()) {
        if (history[historyHead]->apply(*this)) { syncCb(); }
        ++historyHead;
        actionCb();
    }
}

const char* EditMap::redoDescription() const {
    if (historyHead != history.size()) return history[historyHead]->description();
    return nullptr;
}

void EditMap::addAction(const Action::Ptr& a) {
    if (historyHead < history.size()) {
        history.erase(history.begin() + historyHead, history.end());
    }

    history.emplace_back(a);
    if (history.back()->apply(*this)) { syncCb(); }
    historyHead = history.size();
    actionCb();
}

void EditMap::setPlaylist(const std::string& playlist) {
    addAction(SetPlaylistAction::create(playlist, *this));
}

void EditMap::setWeather(core::map::Weather::Type type) {
    addAction(SetWeatherAction::create(type, *this));
}

void EditMap::setTile(unsigned int level, unsigned int layer, const sf::Vector2i& pos,
                      core::map::Tile::IdType id, bool isAnim) {
    addAction(SetTileAction::create(level, layer, pos, isAnim, id, *this));
}

void EditMap::setTileArea(unsigned int level, unsigned int layer, const sf::IntRect& area,
                          core::map::Tile::IdType value, bool isAnim) {
    addAction(SetTileAreaAction::create(level, layer, area, isAnim, value, *this));
}

void EditMap::setCollision(unsigned int level, const sf::Vector2i& pos, core::map::Collision val) {
    addAction(SetCollisionAction::create(level, pos, val, *this));
}

void EditMap::setCollisionArea(unsigned int level, const sf::IntRect& area,
                               core::map::Collision val) {
    addAction(SetCollisionAreaAction::create(level, area, val, *this));
}

void EditMap::setCatch(unsigned int level, const sf::Vector2i& pos, core::map::Catch value) {
    addAction(SetCatchAction::create(level, pos, value, *this));
}

void EditMap::setCatchArea(unsigned int level, const sf::IntRect& area, core::map::Catch value) {
    addAction(SetCatchAreaAction::create(level, area, value, *this));
}

void EditMap::appendBottomLayer(unsigned int level) {
    addAction(AppendLayerAction::create(level, AppendLayerAction::Bottom));
}

void EditMap::appendYsortLayer(unsigned int level) {
    addAction(AppendLayerAction::create(level, AppendLayerAction::YSort));
}

void EditMap::appendTopLayer(unsigned int level) {
    addAction(AppendLayerAction::create(level, AppendLayerAction::Top));
}

void EditMap::removeLayer(unsigned int level, unsigned int layer) {
    addAction(RemoveLayerAction::create(level, layer, *this));
}

void EditMap::shiftLayer(unsigned int level, unsigned int layer, bool up) {
    addAction(ShiftLayerAction::create(level, layer, up));
}

void EditMap::shiftLevel(unsigned int level, bool up) {
    addAction(ShiftLevelAction::create(level, up));
}

void EditMap::appendLevel() { addAction(AppendLevelAction::create()); }

void EditMap::setOnEnterScript(const std::string& s) {
    addAction(SetScriptAction::create(true, s, loadScriptField.getValue()));
}

const std::string& EditMap::getOnEnterScript() const { return loadScriptField.getValue(); }

void EditMap::setOnExitScript(const std::string& s) {
    addAction(SetScriptAction::create(false, s, unloadScriptField.getValue()));
}

const std::string& EditMap::getOnExitScript() const { return unloadScriptField.getValue(); }

bool EditMap::spawnIdUnused(unsigned int i) const {
    return spawnField.getValue().find(i) == spawnField.getValue().end();
}

void EditMap::addSpawn(unsigned int l, const sf::Vector2i& pos, unsigned int id,
                       core::component::Direction dir) {
    addAction(AddSpawnAction::create(l, pos, id, dir));
}

void EditMap::rotateSpawn(unsigned int l, const sf::Vector2i& pos) {
    for (const auto& pair : spawnField.getValue()) {
        if (pair.second.position.getValue().level == l) {
            if (pair.second.position.getValue().positionTiles() == pos) {
                addAction(RotateSpawnAction::create(pair.first));
            }
        }
    }
}

void EditMap::removeSpawn(unsigned int l, const sf::Vector2i& pos) {
    for (const auto& pair : spawnField.getValue()) {
        if (pair.second.position.getValue().level == l) {
            if (pair.second.position.getValue().positionTiles() == pos) {
                addAction(RemoveSpawnAction::create(pair.first, pair.second));
            }
        }
    }
}

void EditMap::render(sf::RenderTarget& target, float residual,
                     const EntityRenderCallback& entityCb) const {
    const sf::View& view = target.getView();
    renderView           = view;
    cover.setPosition(view.getCenter());
    cover.setSize(view.getSize());
    cover.setOrigin(view.getSize() * 0.5f);
    target.draw(cover, {sf::BlendNone});

    static const sf::Vector2i ExtraRender =
        sf::Vector2i(core::Properties::ExtraRenderTiles(), core::Properties::ExtraRenderTiles());

    const sf::Vector2f cornerPixels =
        target.getView().getCenter() - target.getView().getSize() / 2.f;
    sf::Vector2i corner =
        static_cast<sf::Vector2i>(cornerPixels) / core::Properties::PixelsPerTile() - ExtraRender;
    if (corner.x < 0) corner.x = 0;
    if (corner.y < 0) corner.y = 0;

    sf::Vector2i wsize =
        static_cast<sf::Vector2i>(target.getView().getSize()) / core::Properties::PixelsPerTile() +
        ExtraRender * 2;
    if (corner.x + wsize.x > size.x) wsize.x = size.x - corner.x;
    if (corner.y + wsize.y > size.y) wsize.y = size.y - corner.y;
    renderRange = {corner, wsize};

    const auto renderRow = [&target, residual, &corner, &wsize](const core::map::TileLayer& layer,
                                                                int row) {
        for (int x = corner.x; x < corner.x + wsize.x; ++x) {
            layer.get(x, row).render(target, residual);
        }
    };

    const auto renderSorted =
        [&target, residual, &corner, &wsize](const core::map::SortedLayer& layer, int row) {
            for (int x = corner.x; x < corner.x + wsize.x; ++x) {
                core::map::Tile* t = layer(x, row);
                if (t) t->render(target, residual);
            }
        };

    for (unsigned int i = 0; i < levels.size(); ++i) {
        if (!levelFilter[i]) continue;
        core::map::LayerSet& level = levels[i];

        unsigned int li = 0;
        for (const core::map::TileLayer& layer : level.bottomLayers()) {
            if (!layerFilter[i][li++]) continue;

            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
        for (int y = corner.y; y < corner.y + wsize.y; ++y) {
            li = level.bottomLayers().size();
            for (const core::map::SortedLayer& layer : level.renderSortedLayers()) {
                if (!layerFilter[i][li++]) continue;
                renderSorted(layer, y);
            }
            entityCb(i, y, corner.x, corner.x + wsize.x);
        }
        li = level.bottomLayers().size() + level.ysortLayers().size();
        for (const core::map::TileLayer& layer : level.topLayers()) {
            if (!layerFilter[i][li++]) continue;

            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
    }

    weather.render(target, residual);
    const_cast<EditMap*>(this)->lighting.render(target);

    switch (renderOverlay) {
    case RenderOverlay::Collisions:
        for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
            for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                overlaySprite.setTexture(*colGfx[static_cast<unsigned int>(
                                             levels[overlayLevel].collisionLayer().get(x, y))],
                                         true);
                overlaySprite.setPosition(x * core::Properties::PixelsPerTile(),
                                          y * core::Properties::PixelsPerTile());
                target.draw(overlaySprite);
            }
        }
        break;

    case RenderOverlay::CatchTiles:
        for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
            for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                overlaySprite.setTexture(*catchGfx[static_cast<unsigned int>(
                                             levels[overlayLevel].catchLayer().get(x, y))],
                                         true);
                overlaySprite.setPosition(x * core::Properties::PixelsPerTile(),
                                          y * core::Properties::PixelsPerTile());
                target.draw(overlaySprite);
            }
        }
        break;

    case RenderOverlay::Events: {
        static sf::Clock timer;
        sf::RectangleShape area;
        const std::uint8_t a =
            static_cast<int>(timer.getElapsedTime().asSeconds()) % 2 == 0 ? 165 : 100;
        area.setFillColor(sf::Color(0, 0, 0, a));
        for (const auto& event : eventsField.getValue()) {
            const auto& pos  = event.position.getValue();
            const auto& size = event.areaSize.getValue();
            area.setPosition(static_cast<float>(pos.x) * core::Properties::PixelsPerTile(),
                             static_cast<float>(pos.y) * core::Properties::PixelsPerTile());
            area.setSize({static_cast<float>(size.x) * core::Properties::PixelsPerTile(),
                          static_cast<float>(size.y) * core::Properties::PixelsPerTile()});
            target.draw(area);
        }
    } break;

    case RenderOverlay::Spawns: {
        sf::Text id;
        id.setFont(core::Properties::MenuFont());
        id.setCharacterSize(24);
        id.setFillColor(sf::Color::Red);
        id.setOutlineColor(sf::Color::Black);
        id.setOutlineThickness(1.f);
        const sf::Vector2f ht(core::Properties::PixelsPerTile() / 2,
                              core::Properties::PixelsPerTile() / 2);

        sf::Sprite spr(*arrowGfx);
        const sf::Vector2f so(sf::Vector2f(arrowGfx->getSize()) * 0.5f);
        spr.setOrigin(so);

        for (const auto& spawn : spawnField.getValue()) {
            if (spawn.second.position.getValue().level == overlayLevel) {
                const sf::Vector2f p(spawn.second.position.getValue().positionTiles());
                const sf::Vector2f pos = p * static_cast<float>(core::Properties::PixelsPerTile());
                spr.setPosition(pos + so);
                spr.setRotation(static_cast<int>(spawn.second.position.getValue().direction) * 90);
                target.draw(spr);

                id.setString(std::to_string(spawn.first));
                const sf::Vector2f ts(id.getGlobalBounds().width + id.getLocalBounds().left,
                                      id.getGlobalBounds().height + id.getLocalBounds().top);
                const sf::Vector2f to(ht - ts * 0.5f);
                id.setPosition(pos + to);
                target.draw(id);
            }
        }
    } break;

    case RenderOverlay::PeoplemonZones:
        // TODO
        break;

    case RenderOverlay::None:
    default:
        break;
    }

    selectRect.setPosition(static_cast<float>(selection.left * core::Properties::PixelsPerTile()),
                           static_cast<float>(selection.top * core::Properties::PixelsPerTile()));
    if (selection.width > 0) {
        selectRect.setFillColor(sf::Color(180, 160, 20, 165));
        selectRect.setSize(
            {static_cast<float>(selection.width * core::Properties::PixelsPerTile()),
             static_cast<float>(selection.height * core::Properties::PixelsPerTile())});
        target.draw(selectRect);
    }
    else if (selection.width < 0) {
        selectRect.setFillColor(sf::Color(20, 70, 220, 165));
        selectRect.setSize({static_cast<float>(core::Properties::PixelsPerTile()),
                            static_cast<float>(core::Properties::PixelsPerTile())});
        target.draw(selectRect);
    }
}

void EditMap::createEvent(const core::map::Event& event) {
    addAction(AddEventAction::create(event, eventsField.getValue().size()));
}

const core::map::Event* EditMap::getEvent(const sf::Vector2i& tiles) {
    for (const core::map::Event& event : eventsField.getValue()) {
        const sf::IntRect area(event.position, event.areaSize);
        if (area.contains(tiles)) return &event;
    }
    return nullptr;
}

void EditMap::editEvent(const core::map::Event* orig, const core::map::Event& val) {
    unsigned int i = 0;
    while (&eventsField.getValue()[i] != orig) { ++i; }
    addAction(EditEventAction::create(*orig, val, i));
}

void EditMap::removeEvent(const core::map::Event* e) {
    unsigned int i = 0;
    while (&eventsField.getValue()[i] != e) { ++i; }
    addAction(RemoveEventAction::create(*e, i));
}

} // namespace component
} // namespace editor
