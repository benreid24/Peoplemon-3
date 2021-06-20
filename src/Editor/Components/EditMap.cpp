#include <Editor/Components/EditMap.hpp>

#include "MapActions.hpp"
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>

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
: bl::gui::Element("maps", "editmap")
, Map()
, historyHead(0)
, clickCb(cb)
, moveCb(mcb)
, actionCb(actionCb)
, syncCb(syncCb)
, camera(EditCamera::Ptr(new EditCamera()))
, changedSinceSave(false)
, controlsEnabled(false) {
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

    // TODO - init stuff
}

bool EditMap::editorLoad(const std::string& file) {
    if (!doLoad(file)) {
        doLoad(savefile);
        return false;
    }
    savefile = file;
    history.clear();
    historyHead = 0;
    actionCb();
    return true;
}

bool EditMap::editorSave() {
    if (save(savefile)) {
        changedSinceSave = false;
        return true;
    }
    return false;
}

bool EditMap::doLoad(const std::string& file) {
    changedSinceSave = false;

    clear();
    systems->engine().entities().clear();
    if (!Map::load(file)) return false;

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

    return true;
}

bool EditMap::unsavedChanges() const { return changedSinceSave; }

void EditMap::update(float dt) { Map::update(*systems, dt); }

void EditMap::setControlsEnabled(bool e) {
    controlsEnabled = e;
    camera->enabled = e;
}

void EditMap::setVisibleLevels(const std::vector<bool>& filter) { levelFilter = filter; }

void EditMap::setVisibleLayers(const std::vector<std::vector<bool>>& filter) {
    layerFilter = filter;
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
        changedSinceSave = true;
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
        changedSinceSave = true;
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
    history.back()->apply(*this);
    historyHead = history.size();
    actionCb();
    changedSinceSave = true;
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

void EditMap::appendBottomLayer(unsigned int) {
    // TODO
}

void EditMap::appendYsortLayer(unsigned int) {
    // TODO
}

void EditMap::appendTopLayer(unsigned int) {
    // TODO
}

void EditMap::removeLayer(unsigned int, unsigned int) {
    // TODO
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
    if (corner.x + wsize.x >= size.x) wsize.x = size.x - corner.x - 1;
    if (corner.y + wsize.y >= size.y) wsize.y = size.y - corner.y - 1;
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
            if (!layerFilter[i][li]) continue;
            ++li;

            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
        for (int y = corner.y; y < corner.y + wsize.y; ++y) {
            li = level.bottomLayers().size();
            for (const core::map::SortedLayer& layer : level.renderSortedLayers()) {
                if (!layerFilter[i][li]) continue;
                ++li;
                renderSorted(layer, y);
            }
            entityCb(i, y, corner.x, corner.x + wsize.x);
        }
        li = level.bottomLayers().size() + level.ysortLayers().size();
        for (const core::map::TileLayer& layer : level.topLayers()) {
            if (!layerFilter[i][li]) continue;
            ++li;

            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
    }

    weather.render(target, residual);
    const_cast<EditMap*>(this)->lighting.render(target);
}

} // namespace component
} // namespace editor
