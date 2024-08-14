#include <Editor/Components/EditMap.hpp>

#include "MapActions.hpp"
#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Events/MapRender.hpp>
#include <Editor/Pages/Subpages/Catchables.hpp>
#include <Editor/Pages/Subpages/Towns.hpp>

namespace editor
{
namespace component
{
namespace
{
const std::initializer_list<std::string_view> CollisionTileTextures{
    "EditorResources/Collisions/none.png",
    "EditorResources/Collisions/all.png",
    "EditorResources/Collisions/top.png",
    "EditorResources/Collisions/right.png",
    "EditorResources/Collisions/bottom.png",
    "EditorResources/Collisions/left.png",
    "EditorResources/Collisions/topRight.png",
    "EditorResources/Collisions/bottomRight.png",
    "EditorResources/Collisions/bottomLeft.png",
    "EditorResources/Collisions/topLeft.png",
    "EditorResources/Collisions/topBottom.png",
    "EditorResources/Collisions/leftRight.png",
    "EditorResources/Collisions/noTop.png",
    "EditorResources/Collisions/noRight.png",
    "EditorResources/Collisions/noBottom.png",
    "EditorResources/Collisions/noLeft.png",
    "EditorResources/Collisions/water.png",
    "EditorResources/Collisions/fall.png",
    "EditorResources/Collisions/ledge.png"};

const std::initializer_list<std::string_view> LevelTransitionTextures{
    "EditorResources/LevelTransitions/horUpRight.png",
    "EditorResources/LevelTransitions/horUpLeft.png",
    "EditorResources/LevelTransitions/vertUpUp.png",
    "EditorResources/LevelTransitions/vertUpDown.png"};

unsigned int computePatchSize(unsigned int total, unsigned int maxPatch) {
    if (total < maxPatch) { return total; }

    unsigned int patch = maxPatch;
    while (total % patch > 0 && patch > 0) { --patch; }
    return patch;
}
} // namespace

EditMap::Ptr EditMap::create(const PositionCb& clickCb, const PositionCb& moveCb,
                             const ActionCb& actionCb, const ActionCb& syncCb,
                             core::system::Systems& systems) {
    return Ptr(new EditMap(clickCb, moveCb, actionCb, syncCb, systems));
}

EditMap::EditMap(const PositionCb& cb, const PositionCb& mcb, const ActionCb& actionCb,
                 const ActionCb& syncCb, core::system::Systems& s)
: bl::gui::Element()
, Map()
, historyHead(0)
, saveHead(0)
, clickCb(cb)
, moveCb(mcb)
, actionCb(actionCb)
, syncCb(syncCb)
, camera(nullptr)
, controlsEnabled(false)
, currentEventFillColor(0, 0, 0, 100)
, eventColorTime(0.f)
, renderOverlay(RenderOverlay::None)
, overlayLevel(0)
, nextItemId(0)
, setRenderTarget(false) {
    systems = &s;

    static const auto callCb = [this, &s](const PositionCb& cb) {
        static const float PixelRatio = 1.f / static_cast<float>(core::Properties::PixelsPerTile());

        const sf::Vector2i mouse = sf::Mouse::getPosition(s.engine().window().getSfWindow());
        sf::Vector2f pixels(mouse);
        pixels -= sf::Vector2f(getAcquisition().left, getAcquisition().top);

        rdr::EditMapComponent* com = dynamic_cast<rdr::EditMapComponent*>(getComponent());
        if (com) {
            bl::rc::RenderTarget* rt = com->getTarget();
            if (rt) {
                const glm::vec2 wpos = rt->transformToWorldSpace({pixels.x, pixels.y});
                pixels.x             = wpos.x;
                pixels.y             = wpos.y;
            }
        }

        const sf::Vector2i tiles(std::floor(pixels.x * PixelRatio),
                                 std::floor(pixels.y * PixelRatio));
        cb(pixels, tiles);

        for (unsigned int level = 0; level < levels.size(); ++level) {
            triggerAnimation(
                bl::tmap::Position(level, glm::i32vec2(tiles.x, tiles.y), bl::tmap::Direction::Up));
        }
    };

    getSignal(bl::gui::Event::LeftClicked).willAlwaysCall([this](const bl::gui::Event&, Element*) {
        if (controlsEnabled) { callCb(clickCb); }
    });

    getSignal(bl::gui::Event::MouseMoved).willAlwaysCall([this](const bl::gui::Event&, Element*) {
        callCb(moveCb);
    });

    setOutlineThickness(0.f);
}

EditMap::~EditMap() {
    if (townSquareBatch.exists()) {
        townSquareBatch.destroy();
        townSquares.clear();
    }
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
        return tileset->save(tilesetField);
    }
    return false;
}

const std::string& EditMap::currentFile() const { return savefile; }

void EditMap::newMap(const std::string& filename, const std::string& name,
                     const std::string& tileset, unsigned int width, unsigned int height) {
    clear();
    savefile     = filename;
    nameField    = name;
    tilesetField = tileset;
    levels.resize(1);
    levels.front().init(width, height, 2, 2, 1);
    transitionField.setSize(width, height, core::map::LevelTransition::None);
    editorActivate();
    syncCb();
}

bool EditMap::doLoad(const std::string& file) {
    clear();
    if (!MapManager::initializeExisting(getMapFile(file), static_cast<Map&>(*this))) return false;
    nextItemId = 0;
    for (const auto& item : itemsField) {
        if (nextItemId <= item.mapId) { nextItemId = item.mapId + 1; }
    }
    return editorActivate();
}

bool EditMap::editorActivate() {
    history.clear();
    historyHead = 0;
    saveHead    = 0;

    systems->engine().ecs().destroyAllEntitiesWithFlags(bl::ecs::Flags::WorldObject);

    size = {static_cast<int>(levels.front().bottomLayers().front().width()),
            static_cast<int>(levels.front().bottomLayers().front().height())};
    bl::event::Dispatcher::dispatch<core::event::MapSwitch>({*this});

    tileset = TilesetManager::load(core::map::Tileset::getFullPath(tilesetField));
    if (!tileset) return false;
    tileset->activate(systems->engine());
    Map::prepareRender();
    if (getComponent()) { getComponent()->onElementUpdated(); }
    camera = nullptr;

    weather.activate(*systems, *this);
    weather.set(weatherField);
    lighting.activate(getSceneLighting());

    core::script::LegacyWarn::warn(loadScriptField);
    core::script::LegacyWarn::warn(unloadScriptField);

    if (!activated) {
        activated = true;
        lighting.subscribe();
    }

    for (const core::map::CharacterSpawn& spawn : characterField) {
        if (systems->entity().spawnCharacter(spawn, *this) == bl::ecs::InvalidEntity) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file;
        }
    }

    for (const core::map::Item& item : itemsField) { systems->entity().spawnItem(item, *this); }

    bl::event::Dispatcher::dispatch<core::event::MapEntered>({*this});

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

void EditMap::update(float dt) {
    Map::update(dt);
    if (scene && !camera) {
        rdr::EditMapComponent* com = dynamic_cast<rdr::EditMapComponent*>(getComponent());
        if (com) {
            bl::rc::RenderTarget* rt = com->getTarget();
            if (rt) {
                bl::cam::Camera2D* cam = rt->setCamera<bl::cam::Camera2D>(
                    glm::vec2{sizePixels().x, sizePixels().y},
                    glm::vec2{getAcquisition().width, getAcquisition().height});
                camera          = cam->setController<EditCameraController>(this);
                camera->enabled = controlsEnabled;
                camera->reset(sizeTiles());
                setupOverlay();
            }
        }
    }
    if (!setRenderTarget) {
        rdr::EditMapComponent* com = dynamic_cast<rdr::EditMapComponent*>(getComponent());
        if (com) {
            bl::rc::RenderTarget* rt = com->getTarget();
            if (rt) {
                systems->render().setMainRenderTarget(*rt);
                setRenderTarget = true;
            }
        }
    }

    const sf::Color newEventColor = nextEventFillColor(dt);
    if (currentEventFillColor != newEventColor) {
        currentEventFillColor = newEventColor;
        for (auto& rect : eventsOverlay) { rect.setFillColor(newEventColor); }
    }

    if (exportState.exportComplete) {
        exportState.exportComplete = false;

        // reset render overlay and selection
        setRenderOverlay(exportState.prevRenderOverlay, exportState.prevOverlayLevel);
        showSelection(exportState.prevSelection);

        // reset layer visibility
        for (unsigned int level = 0; level < levels.size(); ++level) {
            for (unsigned int layer = 0; layer < levels[level].layerCount(); ++layer) {
                updateLayerVisibility(
                    level, layer, !levelFilter[level] || !layerFilter[level][layer]);
            }
        }

        // reset lighting
        lighting.setAmbientLevel(exportState.prevAmbientLightLow, exportState.prevAmbientLightHigh);

        // reset entities
        if (exportState.entitiesHidden) {
            // TODO - unhide
        }

        // reset camera and controls
        camera->zoomAmount = exportState.prevZoom;
        camera->getCamera().setCenter(exportState.prevCenter);
        controlsEnabled = exportState.prevEnabled;
        bl::event::Dispatcher::dispatch<event::MapRenderCompleted>({});

        // release resources
        exportState.renderTexture.release();
    }
}

void EditMap::setControlsEnabled(bool e) {
    controlsEnabled = e;
    if (camera) { camera->enabled = controlsEnabled; }
}

void EditMap::setLevelVisible(unsigned int level, bool v) {
    levelFilter[level] = v;

    auto it = renderLevels.begin();
    std::advance(it, level);
    auto& rl = *it;

    unsigned int layer = 0;
    for (auto zone : rl.zones) {
        const bool hide = !v || !layerFilter[level][layer];

        updateLayerVisibility(level, layer, hide);
        ++layer;
    }
}

void EditMap::setLayerVisible(unsigned int level, unsigned int layer, bool v) {
    layerFilter[level][layer] = v;
    updateLayerVisibility(level, layer, !v || !levelFilter[level]);
}

void EditMap::updateLayerVisibility(unsigned int level, unsigned int layer, bool hide) {
    auto it = renderLevels.begin();
    std::advance(it, level);
    auto& rl = *it;

    auto* zone = rl.zones[layer];
    zone->tileSprites.setHidden(hide);
    zone->tileAnims.setHidden(hide);

    auto& tileLayer = levels[level].getLayer(layer);
    for (unsigned int x = 0; x < size.x; ++x) {
        for (unsigned int y = 0; y < size.y; ++y) {
            auto& tile = tileLayer.getRef(x, y);
            auto* anim = std::get_if<std::shared_ptr<bl::gfx::Animation2D>>(&tile.renderObject);
            if (anim) { (*anim)->setHidden(hide); }
        }
    }
}

void EditMap::setRenderOverlay(RenderOverlay ro, unsigned int l) {
    if (exportState.exportInProgress && ro != RenderOverlay::None) { return; }

    if (renderOverlay != ro || overlayLevel != l) {
        renderOverlay = ro;
        overlayLevel  = l;

        townSquareBatch.setHidden(renderOverlay != RenderOverlay::Towns);

        const bool hideSpawns = renderOverlay != RenderOverlay::Spawns;
        for (auto& spawn : spawnSprites) {
            spawn.second.arrow.setHidden(hideSpawns);
            spawn.second.label.setHidden(hideSpawns);
        }

        unsigned int level = 0;
        for (auto& catchLayer : catchTileOverlay) {
            catchLayer.shapeBatch.setHidden(renderOverlay != RenderOverlay::CatchTiles ||
                                            overlayLevel != level);
            ++level;
        }

        level = 0;
        for (auto& colLayer : collisionTileOverlay) {
            colLayer.batch.setHidden(renderOverlay != RenderOverlay::Collisions ||
                                     overlayLevel != level);
            ++level;
        }

        levelTransitionsOverlay.value().batch.setHidden(renderOverlay !=
                                                        RenderOverlay::LevelTransitions);

        const bool hideEvents = renderOverlay != RenderOverlay::Events;
        for (auto& eventRect : eventsOverlay) { eventRect.setHidden(hideEvents); }
    }
}

void EditMap::showGrid(bool s) { grid.setHidden(!s); }

void EditMap::showSelection(const sf::IntRect& s) {
    selection = s;

    const float PixelsPerTile = core::Properties::PixelsPerTile();
    selectRect.getTransform().setPosition(s.left * PixelsPerTile, s.top * PixelsPerTile);
    selectRect.setHidden(s.width == 0);
    if (s.width > 0) {
        selectRect.setFillColor(sf::Color(180, 160, 20, 165));
        selectRect.scaleToSize(glm::vec2(s.width * PixelsPerTile, s.height * PixelsPerTile));
    }
    else {
        selectRect.scaleToSize({PixelsPerTile, PixelsPerTile});
        selectRect.setFillColor(sf::Color(20, 70, 220, 165));
    }
}

void EditMap::removeAllTiles(core::map::Tile::IdType id, bool anim) {
    const auto cleanLayer = [this, id, anim](core::map::TileLayer& layer,
                                             unsigned int levelIndex,
                                             unsigned int layerIndex) -> bool {
        bool mod = false;
        for (unsigned int x = 0; x < layer.width(); ++x) {
            for (unsigned int y = 0; y < layer.height(); ++y) {
                auto& tile = layer.getRef(x, y);
                if (tile.id() == id && tile.isAnimation() == anim) {
                    mod = true;
                    tile.set(core::map::Tile::Blank, false);
                    setupTile(levelIndex, layerIndex, {x, y});
                }
            }
        }
        return mod;
    };

    unsigned int levelIndex = 0;
    for (auto& level : levels) {
        unsigned int layerIndex = 0;
        for (auto& layer : level.bottomLayers()) {
            cleanLayer(layer, levelIndex, layerIndex);
            ++layerIndex;
        }
        for (auto& layer : level.ysortLayers()) {
            cleanLayer(layer, levelIndex, layerIndex);
            ++layerIndex;
        }
        for (auto& layer : level.topLayers()) {
            cleanLayer(layer, levelIndex, layerIndex);
            ++layerIndex;
        }

        ++levelIndex;
    }
}

EditMap::EditCameraController::EditCameraController(EditMap* owner)
: enabled(false)
, owner(owner) {}

void EditMap::EditCameraController::update(float dt) {
    if (enabled) {
        float PixelsPerSecond =
            0.5f * zoomAmount * static_cast<float>(core::Properties::WindowWidth());
        constexpr float ZoomPerSecond = 1.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { PixelsPerSecond *= 5.f; }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            camera().move({0.f, -PixelsPerSecond * dt});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            camera().move({PixelsPerSecond * dt, 0.f});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            camera().move({0.f, PixelsPerSecond * dt});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            camera().move({-PixelsPerSecond * dt, 0.f});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) { zoom(-ZoomPerSecond * dt); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { zoom(ZoomPerSecond * dt); }
    }

    // always constrain
    const float w      = camera().getSize().x;
    const float h      = camera().getSize().y;
    glm::vec2 position = camera().getCenter();
    if (mapSize.x > w) {
        if (position.x - w * 0.5f < 0.f) { position.x = w * 0.5f; }
        if (position.x + w * 0.5f > mapSize.x) { position.x = mapSize.x - w * 0.5f; }
    }
    else { position.x = mapSize.x * 0.5f; }
    if (mapSize.y > h) {
        if (position.y - h * 0.5f < 0.f) { position.y = h * 0.5f; }
        if (position.y + h * 0.5f > mapSize.y) { position.y = mapSize.y - h * 0.5f; }
    }
    else { position.y = mapSize.y * 0.5f; }
    camera().setCenter(position);

    // always set size
    const sf::FloatRect& acq = owner->getAcquisition();
    camera().setSize(glm::vec2(acq.width, acq.height) * zoomAmount);
}

void EditMap::EditCameraController::reset(const sf::Vector2i& t) {
    mapSize = glm::vec2(t.x, t.y) * static_cast<float>(core::Properties::PixelsPerTile());
    camera().setCenter(mapSize * 0.5f);
    updateDepthPlanes();
    zoomAmount = 1.5f;
}

void EditMap::EditCameraController::zoom(float z) {
    zoomAmount += z;
    if (zoomAmount < 0.1f) { zoomAmount = 0.1f; }
}

void EditMap::EditCameraController::updateDepthPlanes() {
    camera().setNearAndFarPlanes(-owner->getMinDepth(), 0.f);
}

sf::Vector2f EditMap::minimumRequisition() const { return {100.f, 100.f}; }

bl::gui::rdr::Component* EditMap::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return renderer.createComponent<EditMap>(*this);
}

bool EditMap::handleScroll(const bl::gui::Event& event) {
    const bool c = getAcquisition().contains(event.mousePosition());
    if (controlsEnabled && c) { camera->zoom(-event.scrollDelta() * 0.1f); }
    return c;
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

void EditMap::setName(const std::string& n) { addAction(SetNameAction::create(n, *this)); }

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

void EditMap::fillTile(unsigned int level, unsigned int layer, const sf::Vector2i& pos,
                       core::map::Tile::IdType id, bool isAnim) {
    addAction(FillTileAction::create(level, layer, pos, id, isAnim, *this));
}

void EditMap::setCollision(unsigned int level, const sf::Vector2i& pos, core::map::Collision val) {
    addAction(SetCollisionAction::create(level, pos, val, *this));
}

void EditMap::setCollisionArea(unsigned int level, const sf::IntRect& area,
                               core::map::Collision val) {
    addAction(SetCollisionAreaAction::create(level, area, val, *this));
}

void EditMap::fillCollision(unsigned int level, const sf::Vector2i& pos, core::map::Collision col) {
    addAction(FillCollisionAction::create(level, pos, col, *this));
}

void EditMap::setCatch(unsigned int level, const sf::Vector2i& pos, std::uint8_t value) {
    addAction(SetCatchAction::create(level, pos, value, *this));
}

void EditMap::setCatchArea(unsigned int level, const sf::IntRect& area, std::uint8_t value) {
    addAction(SetCatchAreaAction::create(level, area, value, *this));
}

void EditMap::fillCatch(unsigned int level, const sf::Vector2i& pos, std::uint8_t id) {
    addAction(FillCatchAction::create(level, pos, id, *this));
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
    addAction(SetScriptAction::create(true, s, loadScriptField));
}

const std::string& EditMap::getOnEnterScript() const { return loadScriptField; }

void EditMap::setOnExitScript(const std::string& s) {
    addAction(SetScriptAction::create(false, s, unloadScriptField));
}

const std::string& EditMap::getOnExitScript() const { return unloadScriptField; }

bool EditMap::spawnIdUnused(unsigned int i) const { return spawns.find(i) == spawns.end(); }

void EditMap::addSpawn(unsigned int l, const sf::Vector2i& pos, unsigned int id,
                       bl::tmap::Direction dir) {
    addAction(AddSpawnAction::create(l, pos, id, dir));
}

void EditMap::rotateSpawn(unsigned int l, const sf::Vector2i& pos) {
    const glm::i32vec2 gpos(pos.x, pos.y);
    for (const auto& pair : spawns) {
        if (pair.second.position.level == l) {
            if (pair.second.position.position == gpos) {
                addAction(RotateSpawnAction::create(pair.first));
            }
        }
    }
}

void EditMap::removeSpawn(unsigned int l, const sf::Vector2i& pos) {
    const glm::i32vec2 gpos(pos.x, pos.y);
    for (const auto& pair : spawns) {
        if (pair.second.position.level == l) {
            if (pair.second.position.position == gpos) {
                addAction(RemoveSpawnAction::create(pair.first, pair.second));
            }
        }
    }
}

void EditMap::addNpcSpawn(const core::map::CharacterSpawn& s) {
    addAction(AddNpcSpawnAction::create(s, characterField.size()));
}

const core::map::CharacterSpawn* EditMap::getNpcSpawn(unsigned int level,
                                                      const sf::Vector2i& pos) const {
    const glm::i32vec2 gpos(pos.x, pos.y);
    for (const auto& spawn : characterField) {
        if (spawn.position.level == level) {
            if (spawn.position.position == gpos) { return &spawn; }
        }
    }
    return nullptr;
}

void EditMap::editNpcSpawn(const core::map::CharacterSpawn* orig,
                           const core::map::CharacterSpawn& val) {
    const bl::ecs::Entity e = systems->position().getEntity(orig->position);
    if (e != bl::ecs::InvalidEntity) {
        addAction(EditNpcSpawnAction::create(orig - characterField.data(), *orig, val));
    }
    else {
        const auto& pos = orig->position.position;
        BL_LOG_WARN << "Failed to get entity id at location: (" << pos.x << ", " << pos.y << ")";
    }
}

void EditMap::removeNpcSpawn(const core::map::CharacterSpawn* s) {
    unsigned int i = 0;
    for (; i < characterField.size(); ++i) {
        if (&characterField[i] == s) { break; }
    }
    const bl::ecs::Entity e = systems->position().getEntity(s->position);
    if (e != bl::ecs::InvalidEntity) { addAction(RemoveNpcSpawnAction::create(*s, i)); }
    else {
        const auto& pos = s->position.position;
        BL_LOG_WARN << "Failed to get entity id at location: (" << pos.x << ", " << pos.y << ")";
    }
}

void EditMap::staticRender(const RenderMapWindow& params) {
    exportState.exportInProgress = true;
    exportState.exportComplete   = false;
    exportState.outputPath       = params.outputPath();

    // unhide all layers
    for (unsigned int level = 0; level < levels.size(); ++level) {
        for (unsigned int layer = 0; layer < levels[level].layerCount(); ++layer) {
            updateLayerVisibility(level, layer, false);
        }
    }

    // hide all overlays
    exportState.prevRenderOverlay = renderOverlay;
    exportState.prevOverlayLevel  = overlayLevel;
    setRenderOverlay(RenderOverlay::None, 0);

    // hide selection
    exportState.prevSelection = selection;
    showSelection({0, 0, 0, 0});

    // set lighting
    exportState.prevAmbientLightLow  = lighting.getMinLightLevel();
    exportState.prevAmbientLightHigh = lighting.getMaxLightLevel();
    lighting.setAmbientLevel(params.lightLevel(), params.lightLevel());

    // create render texture
    const sf::Vector2u sp(size * core::Properties::PixelsPerTile());
    const unsigned int ms = systems->engine()
                                .renderer()
                                .vulkanState()
                                .physicalDeviceProperties.limits.maxImageDimension2D;
    exportState.size   = glm::u32vec2(computePatchSize(sp.x, ms), computePatchSize(sp.y, ms));
    exportState.center = glm::u32vec2(exportState.size.x / 2, exportState.size.y / 2);
    exportState.renderTexture = systems->engine().renderer().createRenderTexture(exportState.size);
    exportState.renderTexture->pushScene(scene);
    exportState.camera = exportState.renderTexture->setCamera<bl::cam::Camera2D>(
        glm::vec2(exportState.center), glm::vec2(exportState.size));
    exportState.camera->setNearAndFarPlanes(-getMinDepth(), 0.f);

    // TODO - hide all entities if required
    exportState.entitiesHidden = !params.renderCharacters();
    if (exportState.entitiesHidden) {
        // TODO - hide
    }

    // show entire map
    const glm::vec2 spf(sp.x, sp.y);
    exportState.prevCenter = camera->getCamera().getCenter();
    exportState.prevZoom   = camera->zoomAmount;
    camera->getCamera().setCenter(spf * 0.5f);
    camera->zoomAmount = std::max(spf.x / getAcquisition().width, spf.y / getAcquisition().height);

    // disable all controls
    exportState.prevEnabled = controlsEnabled;
    controlsEnabled         = false;
    bl::event::Dispatcher::dispatch<event::MapRenderStarted>({});

    // start initial export (deferred for descriptors to update)
    systems->engine().renderer().vulkanState().cleanupManager.add([this]() {
        exportState.exportJob = systems->engine().renderer().textureExporter().exportTexture(
            exportState.renderTexture->getTexture());
        systems->engine().renderer().vulkanState().cleanupManager.add([this]() {
            systems->engine().longRunningThreadpool().queueTask([this]() { exportRendering(); });
        });
    });
}

void EditMap::createEvent(const core::map::Event& event) {
    addAction(AddEventAction::create(event, eventsField.size()));
}

const core::map::Event* EditMap::getEvent(const sf::Vector2i& tiles) {
    for (const core::map::Event& event : eventsField) {
        const sf::IntRect area(event.position, event.areaSize);
        if (area.contains(tiles)) return &event;
    }
    return nullptr;
}

void EditMap::editEvent(const core::map::Event* orig, const core::map::Event& val) {
    unsigned int i = 0;
    while (&eventsField[i] != orig) { ++i; }
    addAction(EditEventAction::create(*orig, val, i));
}

void EditMap::removeEvent(const core::map::Event* e) {
    unsigned int i = 0;
    while (&eventsField[i] != e) { ++i; }
    addAction(RemoveEventAction::create(*e, i));
}

void EditMap::addOrEditItem(unsigned int level, const sf::Vector2i& tiles, core::item::Id item,
                            bool visible) {
    unsigned int i = 0;
    bool found     = false;
    for (; i < itemsField.size(); ++i) {
        if (itemsField[i].level == level) {
            if (itemsField[i].position == tiles) {
                found = true;
                break;
            }
        }
    }
    if (!found) i = itemsField.size();
    addAction(AddOrEditItemAction::create(
        i, level, tiles, item, visible, found ? itemsField[i] : core::map::Item(), !found));
}

std::pair<core::item::Id, bool> EditMap::getItem(unsigned int level, const sf::Vector2i& tiles) {
    for (const auto& item : itemsField) {
        if (item.level == level && item.position == tiles) {
            return {core::item::Item::cast(item.id), item.visible};
        }
    }
    return {core::item::Id::Unknown, true};
}

void EditMap::removeItem(unsigned int level, const sf::Vector2i& tiles) {
    unsigned int i = 0;
    for (const auto& item : itemsField) {
        if (item.level == level && item.position == tiles) {
            addAction(RemoveItemAction::create(i, level, tiles, item));
            break;
        }
        ++i;
    }
}

void EditMap::setLight(const sf::Vector2i& pos, unsigned int rad) {
    const core::map::LightingSystem::Handle h = lighting.getClosestLight(pos);
    const core::map::Light orig =
        h != core::map::LightingSystem::None ? lighting.getLight(h) : core::map::Light(rad, pos);
    addAction(SetLightAction::create(pos, rad, orig));
}

void EditMap::removeLight(const sf::Vector2i& pos) {
    const core::map::LightingSystem::Handle h = lighting.getClosestLight(pos);
    if (h != core::map::LightingSystem::None) {
        addAction(RemoveLightAction::create(lighting.getLight(h)));
    }
}

void EditMap::addCatchRegion() { addAction(AddCatchRegionAction::create()); }

const std::vector<core::map::CatchRegion>& EditMap::catchRegions() const {
    return catchRegionsField;
}

void EditMap::editCatchRegion(std::uint8_t index, const core::map::CatchRegion& value) {
    addAction(EditCatchRegionAction::create(index, value, catchRegionsField[index]));
}

void EditMap::removeCatchRegion(std::uint8_t index) {
    addAction(RemoveCatchRegionAction::create(index, catchRegionsField[index]));
}

void EditMap::setAmbientLight(std::uint8_t lower, std::uint8_t upper, bool sun) {
    addAction(SetAmbientLightAction::create(sun, upper, lower, lightingSystem()));
}

void EditMap::addTown() { addAction(AddTownAction::create()); }

void EditMap::editTown(std::uint8_t i, const core::map::Town& town) {
    addAction(EditTownAction::create(i, towns[i], town));
}

void EditMap::removeTown(std::uint8_t i) { addAction(RemoveTownAction::create(i, towns[i])); }

void EditMap::setTownTile(const sf::Vector2i& pos, std::uint8_t id) {
    addAction(SetTownTileAction::create(pos, id, townTiles(pos.x, pos.y)));
}

void EditMap::setTownTileArea(const sf::IntRect& area, std::uint8_t id) {
    addAction(SetTownTileAreaAction::create(area, id, *this));
}

void EditMap::fillTownTiles(const sf::Vector2i& pos, std::uint8_t id) {
    addAction(FillTownTileAction::create(pos, id, *this));
}

void EditMap::setLevelTile(const sf::Vector2i& pos, core::map::LevelTransition lt) {
    addAction(SetLevelTileAction::create(pos, lt, transitionField(pos.x, pos.y)));
}

void EditMap::setLevelTileArea(const sf::IntRect& area, core::map::LevelTransition lt) {
    addAction(SetLevelTileAreaAction::create(area, lt, *this));
}

void EditMap::setupOverlay() {
    BL_LOG_INFO << "Preparing map editor overlays...";
    const float PixelsPerTile = core::Properties::PixelsPerTile();

    // towns
    BL_LOG_INFO << "Generating town geometry...";
    if (townSquareBatch.exists()) {
        townSquareBatch.destroy();
        townSquares.clear();
    }
    townSquareBatch.create(systems->engine(), size.x * size.y * 4);
    townSquares.setSize(size.x, size.y);
    townSquareBatch.getTransform().setDepth(getMinDepth());
    for (int x = 0; x < size.x; ++x) {
        for (int y = 0; y < size.y; ++y) {
            auto& square = townSquares(x, y);
            square.create(systems->engine(), townSquareBatch, {PixelsPerTile, PixelsPerTile});
            square.setFillColor(page::Towns::getColor(townTiles(x, y)));
            square.setOutlineColor(sf::Color::Black);
            square.setOutlineThickness(-1.f);
            square.getLocalTransform().setPosition(x * PixelsPerTile, y * PixelsPerTile);
            square.commit();
        }
    }
    townSquareBatch.component().containsTransparency = true;
    townSquareBatch.addToScene(scene, bl::rc::UpdateSpeed::Static);
    townSquareBatch.setHidden(renderOverlay != RenderOverlay::Towns);

    // spawns
    BL_LOG_INFO << "Generating spawn icons...";
    spawnSprites.clear();
    for (auto& spawn : spawns) { addSpawnGfx(spawn.second); }

    // catch tiles
    BL_LOG_INFO << "Generating catch tile geometry...";
    catchTileOverlay.clear();
    for (unsigned int level = 0; level < levels.size(); ++level) {
        auto& overlay = catchTileOverlay.emplace_back();
        overlay.shapeBatch.create(systems->engine(), size.x * size.y * 4);
        overlay.shapeBatch.getTransform().setDepth(getMinDepth());
        overlay.tiles.setSize(size.x, size.y);
        for (unsigned int x = 0; x < size.x; ++x) {
            for (unsigned int y = 0; y < size.y; ++y) {
                auto& tile = overlay.tiles(x, y);
                tile.create(systems->engine(), overlay.shapeBatch, {PixelsPerTile, PixelsPerTile});
                tile.getLocalTransform().setPosition(x * PixelsPerTile, y * PixelsPerTile);
                tile.setOutlineColor(sf::Color::Black);
                tile.setOutlineThickness(-1.f);
                tile.setFillColor(page::Catchables::getColor(levels[level].catchLayer().get(x, y)));
                tile.commit();
            }
        }
        overlay.shapeBatch.component().containsTransparency = true;
        overlay.shapeBatch.addToScene(scene, bl::rc::UpdateSpeed::Static);
        overlay.shapeBatch.setHidden(renderOverlay != RenderOverlay::CatchTiles ||
                                     overlayLevel != level);
    }

    // stitched textures
    if (!collisionTilesTexture) { stitchOverlayTextures(); }

    // collisions
    BL_LOG_INFO << "Generating collision tile geometry...";
    collisionTileOverlay.clear();
    for (unsigned int level = 0; level < levels.size(); ++level) {
        auto& overlay = collisionTileOverlay.emplace_back();
        overlay.batch.create(systems->engine(), collisionTilesTexture, size.x * size.y);
        overlay.batch.getTransform().setDepth(getMinDepth());
        overlay.tiles.setSize(size.x, size.y);
        for (unsigned int x = 0; x < size.x; ++x) {
            for (unsigned int y = 0; y < size.y; ++y) {
                auto& tile = overlay.tiles(x, y);
                tile.create(systems->engine(), overlay.batch, {0.f, 0.f, 32.f, 32.f});
                tile.getLocalTransform().setPosition(x * PixelsPerTile, y * PixelsPerTile);
                updateCollisionTileTexture(level, x, y);
            }
        }
        overlay.batch.component().containsTransparency = true;
        overlay.batch.addToScene(scene, bl::rc::UpdateSpeed::Static);
        overlay.batch.setHidden(renderOverlay != RenderOverlay::Collisions ||
                                overlayLevel != level);
    }

    // level transitions
    BL_LOG_INFO << "Generating level transition geometry...";
    levelTransitionsOverlay.reset();
    auto& ltOverlay = levelTransitionsOverlay.emplace();
    ltOverlay.batch.create(systems->engine(), levelTransitionsTexture, size.x * size.y);
    ltOverlay.batch.getTransform().setDepth(getMinDepth());
    ltOverlay.tiles.setSize(size.x, size.y);
    for (unsigned int x = 0; x < size.x; ++x) {
        for (unsigned int y = 0; y < size.y; ++y) {
            auto& tile = ltOverlay.tiles(x, y);
            tile.create(systems->engine(), ltOverlay.batch, {0.f, 0.f, 32.f, 32.f});
            tile.getLocalTransform().setPosition(x * PixelsPerTile, y * PixelsPerTile);
            updateLevelTransitionTexture(x, y);
        }
    }
    ltOverlay.batch.component().containsTransparency = true;
    ltOverlay.batch.addToScene(scene, bl::rc::UpdateSpeed::Static);
    ltOverlay.batch.setHidden(renderOverlay != RenderOverlay::LevelTransitions);

    // events
    BL_LOG_INFO << "Generating events geometry...";
    eventsOverlay.clear();
    for (unsigned int i = 0; i < eventsField.size(); ++i) { addEventGfx(i); }

    // selection
    if (!selectRect.exists()) { selectRect.create(systems->engine(), {100.f, 100.f}); }
    selectRect.getTransform().setDepth(getMinDepth());
    selectRect.component().containsTransparency = true;
    selectRect.setHidden(true);
    selectRect.addToScene(scene, bl::rc::UpdateSpeed::Static);

    // grid
    BL_LOG_INFO << "Generating grid geometry...";
    constexpr glm::vec4 Black(0.f, 0.f, 0.f, 1.f);
    if (grid.exists()) { grid.resize((size.x + size.y + 2) * 2, false); }
    else {
        grid.create(systems->engine(), (size.x + size.y + 2) * 2);
        grid.setHidden(true);
    }
    for (int x = 0; x <= size.x * 2; x += 2) {
        grid[x].color     = Black;
        grid[x + 1].color = Black;
        grid[x].pos       = glm::vec3(x / 2 * core::Properties::PixelsPerTile(), 0.f, 0.f);
        grid[x + 1].pos = glm::vec3(x / 2 * core::Properties::PixelsPerTile(), sizePixels().y, 0.f);
    }
    const int o = size.x * 2;
    for (int y = 0; y <= size.y * 2; y += 2) {
        grid[y + o].color     = Black;
        grid[y + 1 + o].color = Black;
        grid[y + o].pos       = glm::vec3(0.f, y / 2 * core::Properties::PixelsPerTile(), 0.f);
        grid[y + 1 + o].pos =
            glm::vec3(sizePixels().x, y / 2 * core::Properties::PixelsPerTile(), 0.f);
    }
    grid.component().containsTransparency = true;
    grid.getTransform().setDepth(getMinDepth());
    grid.commit();
    grid.addToSceneWithCustomPipeline(
        scene, bl::rc::UpdateSpeed::Static, bl::rc::Config::PipelineIds::Lines2D);

    BL_LOG_INFO << "Map editor overlays initialized";
}

void EditMap::addSpawnGfx(const core::map::Spawn& spawn) {
    const float PixelsPerTile = core::Properties::PixelsPerTile();
    auto spawnArrow =
        systems->engine().renderer().texturePool().getOrLoadTexture("EditorResources/arrow.png");

    auto& gfx = spawnSprites.try_emplace(spawn.id).first->second;
    gfx.arrow.create(systems->engine(), spawnArrow);
    gfx.arrow.getTransform().setOrigin(spawnArrow->size() * 0.5f);
    gfx.arrow.getTransform().setPosition(spawn.position.getWorldPosition(PixelsPerTile) +
                                         glm::vec2(PixelsPerTile, PixelsPerTile) * 0.5f);
    gfx.arrow.getTransform().setRotation(90.f * static_cast<float>(spawn.position.direction));
    gfx.arrow.component().containsTransparency = true;
    gfx.arrow.getTransform().setDepth(getMinDepth() + 0.1f);
    gfx.arrow.setHidden(renderOverlay != RenderOverlay::Spawns);
    gfx.arrow.addToScene(scene, bl::rc::UpdateSpeed::Static);

    gfx.label.create(systems->engine(),
                     core::Properties::MenuFont(),
                     std::to_string(spawn.id),
                     24,
                     sf::Color::Red);
    gfx.label.getSection().setOutlineColor(sf::Color::Black);
    gfx.label.getSection().setOutlineThickness(1.f);
    gfx.label.getTransform().setOrigin(gfx.label.getLocalSize() * 0.5f);
    gfx.label.setParent(gfx.arrow);
    gfx.label.getTransform().setDepth(-0.1f);
    gfx.label.setHidden(renderOverlay != RenderOverlay::Spawns);
    gfx.label.addToScene(scene, bl::rc::UpdateSpeed::Static);
    gfx.label.commit(); // need to call manually otherwise we render before commit occurs
}

void EditMap::updateSpawnRotation(std::uint16_t id) {
    const auto srcIt = spawns.find(id);
    const auto it    = spawnSprites.find(id);
    if (srcIt != spawns.end() && it != spawnSprites.end()) {
        it->second.arrow.getTransform().setRotation(
            90.f * static_cast<float>(srcIt->second.position.direction));
    }
}

EditMap::CatchTileLayerGraphics::~CatchTileLayerGraphics() {
    if (shapeBatch.exists()) { shapeBatch.destroy(); }
}

EditMap::BatchSpriteOverlayLayer::~BatchSpriteOverlayLayer() {
    if (batch.exists()) { batch.destroy(); }
}

void EditMap::updateCatchTileColor(unsigned int level, unsigned int x, unsigned int y) {
    auto it = catchTileOverlay.begin();
    std::advance(it, level);
    it->tiles(x, y).setFillColor(page::Catchables::getColor(levels[level].catchLayer().get(x, y)));
    it->tiles(x, y).commit();
}

void EditMap::updateTownTileColor(unsigned int x, unsigned int y) {
    townSquares(x, y).setFillColor(page::Towns::getColor(townTiles(x, y)));
    townSquares(x, y).commit();
}

void EditMap::updateCollisionTileTexture(unsigned int level, unsigned int x, unsigned int y) {
    const auto col = levels[level].collisionLayer().get(x, y);
    auto it        = collisionTileOverlay.begin();
    std::advance(it, level);
    auto& tile      = it->tiles(x, y);
    const auto& src = collisionTextureCoords[static_cast<unsigned int>(col)];
    const float Px  = core::Properties::PixelsPerTile();
    tile.updateSourceRect({src.x, src.y, Px, Px});
    tile.commit();
}

void EditMap::updateLevelTransitionTexture(unsigned int x, unsigned int y) {
    const auto lt   = transitionField(x, y);
    auto& tile      = levelTransitionsOverlay.value().tiles(x, y);
    const auto& src = levelTransitionsTextureCoords[static_cast<unsigned int>(lt)];
    const float Px  = core::Properties::PixelsPerTile();
    tile.updateSourceRect({src.x, src.y, Px, Px});
    tile.commit();
}

void EditMap::addEventGfx(unsigned int i) {
    auto it = eventsOverlay.begin();
    std::advance(it, i);

    const auto& event = eventsField[i];
    auto& rect        = *eventsOverlay.emplace(it);
    rect.create(systems->engine(),
                glm::vec2(event.areaSize.x * core::Properties::PixelsPerTile(),
                          event.areaSize.y * core::Properties::PixelsPerTile()));
    rect.setFillColor(currentEventFillColor);
    rect.getTransform().setPosition(event.position.x * core::Properties::PixelsPerTile(),
                                    event.position.y * core::Properties::PixelsPerTile());
    rect.getTransform().setDepth(getMinDepth());
    rect.component().containsTransparency = true;
    rect.addToScene(scene, bl::rc::UpdateSpeed::Static);
    rect.setHidden(renderOverlay != RenderOverlay::Events);
}

void EditMap::removeEventGfx(unsigned int i) {
    auto it = eventsOverlay.begin();
    std::advance(it, i);
    eventsOverlay.erase(it);
}

sf::Color EditMap::nextEventFillColor(float dt) {
    constexpr float Period = 1.2f;

    eventColorTime += dt;
    if (eventColorTime >= Period) {
        eventColorTime -= Period;
        return currentEventFillColor.a == 100 ? sf::Color(0, 0, 0, 165) : sf::Color(0, 0, 0, 100);
    }
    return currentEventFillColor;
}

void EditMap::stitchOverlayTextures() {
    // collision tiles
    BL_LOG_INFO << "Stitching collision tile textures...";
    colStitcher.emplace(core::Properties::PixelsPerTile() * 8);
    collisionTextureCoords.reserve(CollisionTileTextures.size());
    for (const auto& src : CollisionTileTextures) {
        auto img         = bl::resource::ResourceManager<sf::Image>::load(src.data());
        const auto coord = colStitcher.value().addImage(*img);
        collisionTextureCoords.emplace_back(glm::vec2(coord));
    }
    collisionTilesTexture = systems->engine().renderer().texturePool().createTexture(
        colStitcher.value().getStitchedImage());

    // level transitions
    BL_LOG_INFO << "Stitching level transition textures...";
    levelTransitionsStitcher.emplace(core::Properties::PixelsPerTile() * 6);
    levelTransitionsTextureCoords.reserve(LevelTransitionTextures.size() + 1);
    sf::Image empty;
    empty.create(core::Properties::PixelsPerTile(),
                 core::Properties::PixelsPerTile(),
                 sf::Color::Transparent);
    levelTransitionsTextureCoords.emplace_back(
        glm::vec2(levelTransitionsStitcher.value().addImage(empty)));
    for (const auto& src : LevelTransitionTextures) {
        auto img         = bl::resource::ResourceManager<sf::Image>::load(src.data());
        const auto coord = levelTransitionsStitcher.value().addImage(*img);
        levelTransitionsTextureCoords.emplace_back(glm::vec2(coord));
    }
    levelTransitionsTexture = systems->engine().renderer().texturePool().createTexture(
        levelTransitionsStitcher.value().getStitchedImage());
}

void EditMap::updateLayerDepths(unsigned int level, unsigned int li) {
    auto& layer = levels[level].getLayer(li);
    const bool isYsort =
        li >= levels[level].bottomLayers().size() &&
        li < levels[level].bottomLayers().size() + levels[level].ysortLayers().size();

    for (unsigned int x = 0; x < size.x; ++x) {
        for (unsigned int y = 0; y < size.y; ++y) {
            auto& tile = layer.getRef(x, y);

            const auto getBottomDepth = [this, y, li, level, &tile]() {
                const unsigned int th = tileset->tileHeight(tile.id(), tile.isAnimation());
                unsigned int size     = th / core::Properties::PixelsPerTile();
                if (th % core::Properties::PixelsPerTile() != 0) { ++size; }
                return getDepthForPosition(level, y + size, li);
            };

            switch (tile.renderObject.index()) {
            case 1: {
                const float baseDepth = getDepthForPosition(level, y, li);
                const float depth = !isYsort ? baseDepth : (baseDepth + getBottomDepth()) * 0.5f;
                for (auto& v :
                     std::get<bl::gfx::BatchSpriteSimple>(tile.renderObject).getVertices()) {
                    v.pos.z = depth;
                }
            } break;
            case 2: {
                const float topDepth    = getDepthForPosition(level, y, li);
                const float bottomDepth = getBottomDepth();
                unsigned int i          = 0;
                for (auto& v :
                     std::get<bl::gfx::BatchSlideshowSimple>(tile.renderObject).getVertices()) {
                    v.pos.z = isYsort ? (i < 2 ? topDepth : bottomDepth) : topDepth;
                }
            } break;
            case 3:
                std::get<std::shared_ptr<bl::gfx::Animation2D>>(tile.renderObject)
                    ->getTransform()
                    .setDepth(isYsort ?
                                  ((getDepthForPosition(level, y, li) + getBottomDepth()) * 0.5f) :
                                  getDepthForPosition(level, y, li));
                break;

            case 0:
            default:
                break;
            }
        }
    }

    auto it = renderLevels.begin();
    std::advance(it, level);
    it->zones[li]->tileSprites.component().getBuffer().commit();
    it->zones[li]->tileAnims.component().indexBuffer.commit();
}

void EditMap::updateLevelDepths(unsigned int level) {
    for (unsigned int layer = 0; layer < levels[level].layerCount(); ++layer) {
        updateLayerDepths(level, layer);
    }
}

void EditMap::updateAllDepths() {
    if (camera) { camera->updateDepthPlanes(); }
    for (unsigned int level = 0; level < levels.size(); ++level) { updateLevelDepths(level); }
}

void EditMap::swapRenderLevels(unsigned int i1, unsigned int i2) {
    const bool isGt = i1 > i2;
    const auto it1  = std::next(renderLevels.begin(), i1);
    const auto it2  = std::next(renderLevels.begin(), i2);
    const auto s1   = isGt ? it1 : it2;
    const auto s2   = isGt ? it2 : it1;
    renderLevels.splice(s2, renderLevels, s1);

    updateLevelDepths(i1);
    updateLevelDepths(i2);
}

void EditMap::swapRenderLayers(unsigned int level, unsigned int l1, unsigned int l2) {
    std::next(renderLevels.begin(), level)->swapLayers(l1, l2);
    updateLayerDepths(level, l1);
    updateLayerDepths(level, l2);
}

EditMap::ExportState::ExportState()
: exportInProgress(false)
, exportJob(nullptr)
, exportComplete(false) {}

void EditMap::exportRendering() {
    const sf::Vector2u sp(size * core::Properties::PixelsPerTile());

    // create images while main thread renders
    sf::Image patch;
    sf::Image stitched;
    patch.create(exportState.size.x, exportState.size.y, sf::Color::Transparent);
    stitched.create(sp.x, sp.y, sf::Color::Transparent);

    // determine how many rows & cols to render
    const unsigned int rows = sp.x / exportState.size.x;
    const unsigned int cols = sp.y / exportState.size.y;

    // render each patch
    bl::engine::Systems::TaskHandle queueTask;
    for (unsigned int row = 0; row < rows; ++row) {
        for (unsigned int col = 0; col < cols; ++col) {
            const bool hasNext = row < rows || col < cols;

            // wait for patch render to complete and copy to local memory
            exportState.exportJob->wait();
            exportState.exportJob->copyImage(patch);

            // release and queue next export if there is one
            exportState.exportJob->release();
            if (hasNext) {
                // update camera center
                const bool sameRow   = col < cols - 1;
                const unsigned int x = (sameRow ? row : (row + 1)) * exportState.size.x;
                const unsigned int y = (sameRow ? (col + 1) : 0) * exportState.size.y;
                exportState.camera->setCenter(glm::vec2(x, y));

                // queue next export in sync with engine
                queueTask = systems->engine().systems().addFrameTask(
                    bl::engine::FrameStage::Update2, [this]() {
                        exportState.exportJob =
                            systems->engine().renderer().textureExporter().exportTexture(
                                exportState.renderTexture->getTexture());
                    });
            }

            // copy result into stitched image
            stitched.copy(patch, row * exportState.size.x, col * exportState.size.y);

            // wait for export queue if there is one
            if (hasNext) { queueTask.wait(); }
        }
    }

    // signal completion
    exportState.exportComplete = true;

    // save result
    stitched.saveToFile(exportState.outputPath);

    bl::dialog::tinyfd_messageBox(
        "Rendering Complete",
        std::string("Rendered map saved to: " + exportState.outputPath).c_str(),
        "ok",
        "info",
        1);
}

} // namespace component
} // namespace editor
