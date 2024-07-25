#include <Editor/Components/EditMap.hpp>

#include "MapActions.hpp"
#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Pages/Subpages/Catchables.hpp>
#include <Editor/Pages/Subpages/Towns.hpp>

namespace editor
{
namespace component
{
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
, renderOverlay(RenderOverlay::None)
, overlayLevel(0)
, nextItemId(0)
, setRenderTarget(false) {
    systems = &s;
    loadResources();

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
    if (renderOverlay != ro || overlayLevel != l) {
        renderOverlay = ro;
        overlayLevel  = l;

        townSquareBatch.setHidden(renderOverlay != RenderOverlay::Towns);

        const bool hideSpawns = renderOverlay != RenderOverlay::Spawns;
        for (auto& spawn : spawnSprites) {
            spawn.second.arrow.setHidden(hideSpawns);
            spawn.second.label.setHidden(hideSpawns);
        }
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
    const auto cleanLayer = [this, id, anim](core::map::TileLayer& layer) -> bool {
        bool mod = false;
        for (unsigned int x = 0; x < layer.width(); ++x) {
            for (unsigned int y = 0; y < layer.height(); ++y) {
                auto& tile = layer.getRef(x, y);
                if (tile.id() == id && tile.isAnimation() == anim) {
                    mod = true;
                    /*tile.set(*tileset, core::map::Tile::Blank, false);*/
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
    camera().setNearAndFarPlanes(-owner->getMinDepth(), 0.f);
    zoomAmount = 1.5f;
}

void EditMap::EditCameraController::zoom(float z) {
    zoomAmount += z;
    if (zoomAmount < 0.1f) { zoomAmount = 0.1f; }
}

sf::Vector2f EditMap::minimumRequisition() const { return {100.f, 100.f}; }

bl::gui::rdr::Component* EditMap::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return renderer.createComponent<EditMap>(*this);
}

// void EditMap::doRender(sf::RenderTarget& target, sf::RenderStates,
//                        const bl::gui::Renderer& renderer) const {
//     const sf::View oldView = target.getView();
//     renderView =
//         bl::interface::ViewUtil::computeSubView(getAcquisition(), renderer.getOriginalView());
//     target.setView(renderView);
//     camera.apply(target);
//     systems->render().render(target, *this, 0.f);
//     target.setView(oldView);
// }

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

// void EditMap::render(sf::RenderTarget& target, float residual,
//                      const EntityRenderCallback& entityCb) const {
//     const sf::View& view = target.getView();
//     renderView           = view;
//     cover.setPosition(view.getCenter());
//     cover.setSize(view.getSize());
//     cover.setOrigin(view.getSize() * 0.5f);
//     target.draw(cover, {sf::BlendNone});
//
//     refreshRenderRange(target.getView());
//
//     const auto renderRow = [&target, residual, this](const core::map::TileLayer& layer, int row)
//     {
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             layer.get(x, row).render(target, residual);
//         }
//     };
//
//     const auto renderSorted = [&target, residual, this](const core::map::SortedLayer& layer,
//                                                         int row) {
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             core::map::Tile* t = layer(x, row);
//             if (t) t->render(target, residual);
//         }
//     };
//
//     for (unsigned int i = 0; i < levels.size(); ++i) {
//         if (!levelFilter[i]) continue;
//         const core::map::LayerSet& level = levels[i];
//
//         unsigned int li = 0;
//         for (const core::map::TileLayer& layer : level.bottomLayers()) {
//             if (!layerFilter[i][li++]) continue;
//
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 renderRow(layer, y);
//             }
//         }
//         for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//             li = level.bottomLayers().size();
//             for (const core::map::SortedLayer& layer : level.renderSortedLayers()) {
//                 if (!layerFilter[i][li++]) continue;
//                 renderSorted(layer, y);
//             }
//             entityCb(i, y, renderRange.left, renderRange.left + renderRange.width);
//         }
//         li = level.bottomLayers().size() + level.ysortLayers().size();
//         for (const core::map::TileLayer& layer : level.topLayers()) {
//             if (!layerFilter[i][li++]) continue;
//
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 renderRow(layer, y);
//             }
//         }
//     }
//
//     weather.render(target, residual);
//     const_cast<EditMap*>(this)->lighting.render(target);
//
//     switch (renderOverlay) {
//     case RenderOverlay::Collisions:
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 overlaySprite.setTexture(*colGfx[static_cast<unsigned int>(
//                                              levels[overlayLevel].collisionLayer().get(x, y))],
//                                          true);
//                 overlaySprite.setPosition(x * core::Properties::PixelsPerTile(),
//                                           y * core::Properties::PixelsPerTile());
//                 target.draw(overlaySprite);
//             }
//         }
//         break;
//
//     case RenderOverlay::CatchTiles: {
//         const float p = core::Properties::PixelsPerTile();
//         sf::RectangleShape ct(sf::Vector2f(p, p));
//         ct.setOutlineThickness(-0.5f);
//         ct.setOutlineColor(sf::Color::Black);
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 ct.setFillColor(
//                     page::Catchables::getColor(levels[overlayLevel].catchLayer().get(x, y)));
//                 ct.setPosition(x * core::Properties::PixelsPerTile(),
//                                y * core::Properties::PixelsPerTile());
//                 target.draw(ct);
//             }
//         }
//     } break;
//
//     case RenderOverlay::Towns: {
//         const float p = core::Properties::PixelsPerTile();
//         sf::RectangleShape ct(sf::Vector2f(p, p));
//         ct.setOutlineThickness(-0.5f);
//         ct.setOutlineColor(sf::Color::Black);
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 ct.setFillColor(page::Towns::getColor(townTiles(x, y)));
//                 ct.setPosition(x * core::Properties::PixelsPerTile(),
//                                y * core::Properties::PixelsPerTile());
//                 target.draw(ct);
//             }
//         }
//     } break;
//
//     case RenderOverlay::Events: {
//         static sf::Clock timer;
//         sf::RectangleShape area;
//         const std::uint8_t a =
//             static_cast<int>(timer.getElapsedTime().asSeconds()) % 2 == 0 ? 165 : 100;
//         area.setFillColor(sf::Color(0, 0, 0, a));
//         for (const auto& event : eventsField) {
//             const auto& pos  = event.position;
//             const auto& size = event.areaSize;
//             area.setPosition(static_cast<float>(pos.x) * core::Properties::PixelsPerTile(),
//                              static_cast<float>(pos.y) * core::Properties::PixelsPerTile());
//             area.setSize({static_cast<float>(size.x) * core::Properties::PixelsPerTile(),
//                           static_cast<float>(size.y) * core::Properties::PixelsPerTile()});
//             target.draw(area);
//         }
//     } break;
//
//     case RenderOverlay::Spawns: {
//         sf::Text id;
//         // id.setFont(core::Properties::MenuFont());
//         id.setCharacterSize(24);
//         id.setFillColor(sf::Color::Red);
//         id.setOutlineColor(sf::Color::Black);
//         id.setOutlineThickness(1.f);
//         const sf::Vector2f ht(core::Properties::PixelsPerTile() / 2,
//                               core::Properties::PixelsPerTile() / 2);
//
//         sf::Sprite spr(*arrowGfx);
//         const sf::Vector2f so(sf::Vector2f(arrowGfx->getSize()) * 0.5f);
//         spr.setOrigin(so);
//
//         for (const auto& spawn : spawns) {
//             if (spawn.second.position.level == overlayLevel) {
//                 const sf::Vector2f p(spawn.second.position.position);
//                 const sf::Vector2f pos = p *
//                 static_cast<float>(core::Properties::PixelsPerTile()); spr.setPosition(pos + so);
//                 spr.setRotation(static_cast<int>(spawn.second.position.direction) * 90);
//                 target.draw(spr);
//
//                 id.setString(std::to_string(spawn.first));
//                 const sf::Vector2f ts(id.getGlobalBounds().width + id.getLocalBounds().left,
//                                       id.getGlobalBounds().height + id.getLocalBounds().top);
//                 const sf::Vector2f to(ht - ts * 0.5f);
//                 id.setPosition(pos + to);
//                 target.draw(id);
//             }
//         }
//     } break;
//
//     case RenderOverlay::LevelTransitions:
//         for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
//             for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
//                 if (transitionField(x, y) != core::map::LevelTransition::None) {
//                     overlaySprite.setTexture(
//                         *ltGfx[static_cast<unsigned int>(transitionField(x, y)) - 1], true);
//                     overlaySprite.setPosition(x * core::Properties::PixelsPerTile(),
//                                               y * core::Properties::PixelsPerTile());
//                     target.draw(overlaySprite);
//                 }
//             }
//         }
//         break;
//
//     case RenderOverlay::None:
//     default:
//         break;
//     }
//
//     selectRect.setPosition(static_cast<float>(selection.left *
//     core::Properties::PixelsPerTile()),
//                            static_cast<float>(selection.top *
//                            core::Properties::PixelsPerTile()));
//     if (selection.width > 0) {
//         selectRect.setFillColor(sf::Color(180, 160, 20, 165));
//         selectRect.setSize(
//             {static_cast<float>(selection.width * core::Properties::PixelsPerTile()),
//              static_cast<float>(selection.height * core::Properties::PixelsPerTile())});
//         target.draw(selectRect);
//     }
//     else if (selection.width < 0) {
//         selectRect.setFillColor(sf::Color(20, 70, 220, 165));
//         selectRect.setSize({static_cast<float>(core::Properties::PixelsPerTile()),
//                             static_cast<float>(core::Properties::PixelsPerTile())});
//         target.draw(selectRect);
//     }
//
//     // if (renderGrid) { target.draw(grid); }
// }

void EditMap::staticRender(const RenderMapWindow& params) {
    /*constexpr int PatchSize = 100;
    BL_LOG_INFO << "Rendering map to " << params.outputPath();

    BL_LOG_DEBUG << "Creating RAM and VRAM buffers";
    sf::Image result;
    result.create(size.x * core::Properties::PixelsPerTile(),
                  size.y * core::Properties::PixelsPerTile());
    sf::RenderTexture patch;
    patch.create(PatchSize * core::Properties::PixelsPerTile(),
                 PatchSize * core::Properties::PixelsPerTile());

    int pw = size.x / PatchSize;
    if (size.x % PatchSize > 0) ++pw;
    int ph = size.y / PatchSize;
    if (size.y % PatchSize > 0) ++ph;
    BL_LOG_DEBUG << "Performing render in " << (pw * ph) << " patches";

    const std::uint8_t oldLow  = lighting.getMinLightLevel();
    const std::uint8_t oldHigh = lighting.getMaxLightLevel();
    lighting.setAmbientLevel(params.lightLevel(), params.lightLevel());

    for (int px = 0; px < pw; ++px) {
        for (int py = 0; py < ph; ++py) {
            BL_LOG_DEBUG << "Rendering patch " << (px * ph + py);

            const sf::View view(
                sf::Vector2f{sf::Vector2i{px * PatchSize * core::Properties::PixelsPerTile() +
                                              PatchSize * core::Properties::PixelsPerTile() / 2,
                                          py * PatchSize * core::Properties::PixelsPerTile() +
                                              PatchSize * core::Properties::PixelsPerTile() / 2}},
                sf::Vector2f{sf::Vector2i{PatchSize * core::Properties::PixelsPerTile(),
                                          PatchSize * core::Properties::PixelsPerTile()}});
            patch.setView(view);
            patch.clear(sf::Color::Black);

            refreshRenderRange(view);

            const auto renderRow = [&patch, this](const core::map::TileLayer& layer, int row) {
                for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
                    layer.get(x, row).render(patch, 0.f);
                }
            };

            const auto renderSorted = [&patch, this](const core::map::SortedLayer& layer, int row) {
                for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
                    core::map::Tile* t = layer(x, row);
                    if (t) t->render(patch, 0.f);
                }
            };

            for (unsigned int i = 0; i < levels.size(); ++i) {
                const core::map::LayerSet& level = levels[i];

                for (const core::map::TileLayer& layer : level.bottomLayers()) {
                    for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                        renderRow(layer, y);
                    }
                }
                for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                    for (const core::map::SortedLayer& layer : level.renderSortedLayers()) {
                        renderSorted(layer, y);
                    }
                    if (params.renderCharacters()) {
                        systems->render().renderEntities(patch,
                                                         0.f,
                                                         i,
                                                         y,
                                                         renderRange.left,
                                                         renderRange.left + renderRange.width);
                    }
                }
                for (const core::map::TileLayer& layer : level.topLayers()) {
                    for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                        renderRow(layer, y);
                    }
                }
            }

            BL_LOG_DEBUG << "Rendering lighting for patch";
            lighting.render(patch);

            BL_LOG_DEBUG << "Copying patch to memory";
            patch.display();
            sf::Image rp = patch.getTexture().copyToImage();
            result.copy(
                rp,
                renderRange.left * core::Properties::PixelsPerTile(),
                renderRange.top * core::Properties::PixelsPerTile(),
                {0,
                 0,
                 (renderRange.left + renderRange.width) * core::Properties::PixelsPerTile(),
                 (renderRange.top + renderRange.height) * core::Properties::PixelsPerTile()});
        }
    }
    BL_LOG_DEBUG << "Completed rendering, saving to file";
    lighting.setAmbientLevel(oldLow, oldHigh);

    std::string out = params.outputPath();
    if (!result.saveToFile(out)) {
        out = "properOutputFileBozo.png";
        BL_LOG_DEBUG << "Failed to save, saving to " << out;
        result.saveToFile(out);
    }
    BL_LOG_INFO << "Map rendering complete";

    bl::dialog::tinyfd_messageBox("Rendering Complete",
                                  std::string("Rendered map saved to: " + out).c_str(),
                                  "ok",
                                  "info",
                                  1);*/
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

void EditMap::loadResources() {
    // TODO - use renderer texture pool
    colGfx = {TextureManager::load("EditorResources/Collisions/none.png"),
              TextureManager::load("EditorResources/Collisions/all.png"),
              TextureManager::load("EditorResources/Collisions/top.png"),
              TextureManager::load("EditorResources/Collisions/right.png"),
              TextureManager::load("EditorResources/Collisions/bottom.png"),
              TextureManager::load("EditorResources/Collisions/left.png"),
              TextureManager::load("EditorResources/Collisions/topRight.png"),
              TextureManager::load("EditorResources/Collisions/bottomRight.png"),
              TextureManager::load("EditorResources/Collisions/bottomLeft.png"),
              TextureManager::load("EditorResources/Collisions/topLeft.png"),
              TextureManager::load("EditorResources/Collisions/topBottom.png"),
              TextureManager::load("EditorResources/Collisions/leftRight.png"),
              TextureManager::load("EditorResources/Collisions/noTop.png"),
              TextureManager::load("EditorResources/Collisions/noRight.png"),
              TextureManager::load("EditorResources/Collisions/noBottom.png"),
              TextureManager::load("EditorResources/Collisions/noLeft.png"),
              TextureManager::load("EditorResources/Collisions/water.png"),
              TextureManager::load("EditorResources/Collisions/fall.png"),
              TextureManager::load("EditorResources/Collisions/ledge.png")};

    arrowGfx = TextureManager::load("EditorResources/arrow.png");

    ltGfx = {TextureManager::load("EditorResources/LevelTransitions/horUpRight.png"),
             TextureManager::load("EditorResources/LevelTransitions/horUpLeft.png"),
             TextureManager::load("EditorResources/LevelTransitions/vertUpUp.png"),
             TextureManager::load("EditorResources/LevelTransitions/vertUpDown.png")};
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
            square.setOutlineThickness(1.f);
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

} // namespace component
} // namespace editor
