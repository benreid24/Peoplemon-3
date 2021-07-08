#include "MapActions.hpp"

#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
namespace
{
const core::map::Tile& getTile(std::vector<core::map::LayerSet>& levels, unsigned int level,
                               unsigned int layer, const sf::Vector2i& pos) {
    core::map::LayerSet& l = levels[level];
    if (layer >= l.bottomLayers().size() + l.ysortLayers().size()) {
        const unsigned int i = layer - l.ysortLayers().size() - l.bottomLayers().size();
        return l.topLayers()[i].get(pos.x, pos.y);
    }
    else if (layer >= l.bottomLayers().size()) {
        const unsigned int i = layer - l.bottomLayers().size();
        return l.ysortLayers()[i].get(pos.x, pos.y);
    }
    else {
        return l.bottomLayers()[layer].get(pos.x, pos.y);
    }
}

void setSingleTile(std::vector<core::map::LayerSet>& levels, core::map::Tileset& tileset,
                   unsigned int level, unsigned int layer, const sf::Vector2i& pos,
                   core::map::Tile::IdType value, bool isAnim) {
    core::map::LayerSet& l = levels[level];
    core::map::Tile* tile;
    if (layer >= l.bottomLayers().size() + l.ysortLayers().size()) {
        const unsigned int i = layer - l.ysortLayers().size() - l.bottomLayers().size();
        tile                 = &l.topLayers()[i].getRef(pos.x, pos.y);
        tile->set(tileset, value, isAnim);
    }
    else if (layer >= l.bottomLayers().size()) {
        const unsigned int i                       = layer - l.bottomLayers().size();
        tile                                       = &l.ysortLayers()[i].getRef(pos.x, pos.y);
        *l.getSortedTile(tileset, i, pos.x, pos.y) = nullptr;
        tile->set(tileset, value, isAnim);
        *l.getSortedTile(tileset, i, pos.x, pos.y) = tile;
    }
    else {
        tile = &l.bottomLayers()[layer].getRef(pos.x, pos.y);
        tile->set(tileset, value, isAnim);
    }
}

void shiftLayerUp(core::map::LayerSet& level, unsigned int layer, core::map::Tileset& tileset,
                  std::vector<bool>& filter) {
    if (layer < level.bottomLayers().size()) {
        std::swap(level.bottomLayers()[layer], level.bottomLayers()[layer - 1]);
        std::swap(filter[layer], filter[layer - 1]);
    }
    else {
        layer -= level.bottomLayers().size();
        if (layer == 0) {
            core::map::TileLayer temp(std::move(level.ysortLayers()[layer]));
            level.ysortLayers().erase(level.ysortLayers().begin() + layer);
            level.bottomLayers().emplace_back(std::move(temp));
            level.renderSortedLayers().clear();
            level.activate(tileset);
        }
        else if (layer < level.ysortLayers().size()) {
            std::swap(level.ysortLayers()[layer], level.ysortLayers()[layer - 1]);
            std::swap(filter[layer], filter[layer - 1]);
            level.renderSortedLayers().clear();
            level.activate(tileset);
        }
        else {
            layer -= level.ysortLayers().size();
            if (layer == 0) {
                core::map::TileLayer temp(std::move(level.topLayers()[layer]));
                level.topLayers().erase(level.topLayers().begin() + layer);
                level.ysortLayers().emplace_back(std::move(temp));
                level.renderSortedLayers().clear();
                level.activate(tileset);
            }
            else {
                std::swap(level.topLayers()[layer], level.topLayers()[layer - 1]);
                std::swap(filter[layer], filter[layer - 1]);
            }
        }
    }
}

void shiftLayerDown(core::map::LayerSet& level, unsigned int layer, core::map::Tileset& tileset,
                    std::vector<bool>& filter) {
    if (layer < level.bottomLayers().size() - 1) {
        std::swap(level.bottomLayers()[layer], level.bottomLayers()[layer + 1]);
        std::swap(filter[layer], filter[layer + 1]);
    }
    else if (layer == level.bottomLayers().size() - 1) {
        core::map::TileLayer temp(std::move(level.bottomLayers()[layer]));
        level.bottomLayers().erase(level.bottomLayers().begin() + layer);
        level.ysortLayers().emplace(level.ysortLayers().begin(), std::move(temp));
        level.renderSortedLayers().clear();
        level.activate(tileset);
    }
    else {
        layer -= level.bottomLayers().size();
        if (layer < level.ysortLayers().size() - 1) {
            std::swap(level.ysortLayers()[layer], level.ysortLayers()[layer + 1]);
            std::swap(filter[layer], filter[layer + 1]);
            level.renderSortedLayers().clear();
            level.activate(tileset);
        }
        else if (layer == level.ysortLayers().size() - 1) {
            core::map::TileLayer temp(std::move(level.ysortLayers()[layer]));
            level.ysortLayers().erase(level.ysortLayers().begin() + layer);
            level.topLayers().emplace(level.topLayers().begin(), std::move(temp));
            level.renderSortedLayers().clear();
            level.activate(tileset);
        }
        else {
            layer -= level.ysortLayers().size();
            std::swap(level.topLayers()[layer], level.topLayers()[layer + 1]);
            std::swap(filter[layer], filter[layer + 1]);
        }
    }
}

} // namespace

EditMap::Action::Ptr EditMap::SetTileAction::create(unsigned int level, unsigned int layer,
                                                    const sf::Vector2i& pos, bool isAnim,
                                                    core::map::Tile::IdType value,
                                                    const EditMap& map) {
    core::map::Tile::IdType prev = core::map::Tile::Blank;
    bool wasAnim                 = false;
    if (pos.x >= 0 && pos.y >= 0 && pos.x < map.sizeTiles().x && pos.y < map.sizeTiles().y) {
        if (level < map.levels.size()) {
            core::map::LayerSet& l = map.levels[level];
            if (layer < l.layerCount()) {
                const core::map::Tile& tile = getTile(map.levels, level, layer, pos);
                prev                        = tile.id();
                wasAnim                     = tile.isAnimation();
            }
        }
    }
    return EditMap::Action::Ptr(
        new EditMap::SetTileAction(level, layer, pos, prev, wasAnim, value, isAnim));
}

EditMap::SetTileAction::SetTileAction(unsigned int level, unsigned int layer,
                                      const sf::Vector2i& pos, core::map::Tile::IdType prev,
                                      bool wasAnim, core::map::Tile::IdType value, bool isAnim)
: level(level)
, layer(layer)
, position(pos)
, prev(prev)
, wasAnim(wasAnim)
, updated(value)
, isAnim(isAnim) {}

bool EditMap::SetTileAction::apply(EditMap& map) {
    if (position.x >= 0 && position.y >= 0 && position.x < map.sizeTiles().x &&
        position.y < map.sizeTiles().y) {
        if (level < map.levels.size()) {
            core::map::LayerSet& l = map.levels[level];
            if (layer < l.layerCount()) {
                setSingleTile(map.levels, *map.tileset, level, layer, position, updated, isAnim);
            }
        }
    }
    return false;
}

bool EditMap::SetTileAction::undo(EditMap& map) {
    if (position.x >= 0 && position.y >= 0 && position.x < map.sizeTiles().x &&
        position.y < map.sizeTiles().y) {
        if (level < map.levels.size()) {
            core::map::LayerSet& l = map.levels[level];
            if (layer < l.layerCount()) {
                setSingleTile(map.levels, *map.tileset, level, layer, position, prev, wasAnim);
            }
        }
    }
    return false;
}

const char* EditMap::SetTileAction::description() const { return "set tile"; }

EditMap::Action::Ptr EditMap::SetTileAreaAction::create(unsigned int level, unsigned int layer,
                                                        const sf::IntRect& area, bool isAnim,
                                                        core::map::Tile::IdType value,
                                                        const EditMap& map) {
    sf::Vector2f size(core::Properties::PixelsPerTile(), core::Properties::PixelsPerTile());
    if (value != core::map::Tile::Blank) {
        if (isAnim) {
            auto anim = map.tileset->getAnim(value);
            if (anim) { size = sf::Vector2f(anim->getMaxSize()); }
        }
        else {
            auto tile = map.tileset->getTile(value);
            if (tile) { size = sf::Vector2f(tile->getSize()); }
        }
    }
    const float ts = static_cast<float>(core::Properties::PixelsPerTile());

    bl::container::Vector2D<core::map::Tile::IdType> prev;
    bl::container::Vector2D<std::uint8_t> wasAnim;
    prev.setSize(area.width, area.height, core::map::Tile::Blank);
    wasAnim.setSize(area.width, area.height, 0);

    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            const auto& tile                     = getTile(map.levels, level, layer, {x, y});
            prev(x - area.left, y - area.top)    = tile.id();
            wasAnim(x - area.left, y - area.top) = tile.isAnimation() ? 1 : 0;
        }
    }

    return Ptr(new SetTileAreaAction(level,
                                     layer,
                                     area,
                                     std::move(prev),
                                     std::move(wasAnim),
                                     value,
                                     isAnim,
                                     std::max(1.f, std::floor(size.x / ts) - 1.f),
                                     std::max(1.f, std::floor(size.y / ts) - 1.f)));
}

EditMap::SetTileAreaAction::SetTileAreaAction(
    unsigned int level, unsigned int layer, const sf::IntRect& area,
    bl::container::Vector2D<core::map::Tile::IdType>&& prev,
    bl::container::Vector2D<std::uint8_t>&& wasAnim, core::map::Tile::IdType value, bool isAnim,
    int w, int h)
: level(level)
, layer(layer)
, area(area)
, prev(prev)
, wasAnim(wasAnim)
, updated(value)
, isAnim(isAnim)
, w(w)
, h(h) {}

bool EditMap::SetTileAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; x += w) {
        for (int y = area.top; y < area.top + area.height; y += h) {
            setSingleTile(map.levels, *map.tileset, level, layer, {x, y}, updated, isAnim);
        }
    }
    return false;
}

bool EditMap::SetTileAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            setSingleTile(map.levels,
                          *map.tileset,
                          level,
                          layer,
                          {x, y},
                          prev(x - area.left, y - area.top),
                          wasAnim(x - area.left, y - area.top) == 1);
        }
    }
    return false;
}

const char* EditMap::SetTileAreaAction::description() const { return "set tile area"; }

EditMap::Action::Ptr EditMap::SetCollisionAction::create(unsigned int level,
                                                         const sf::Vector2i& pos,
                                                         core::map::Collision value,
                                                         const EditMap& map) {
    return Ptr(new SetCollisionAction(
        level, pos, value, map.levels[level].collisionLayer().get(pos.x, pos.y)));
}

EditMap::SetCollisionAction::SetCollisionAction(unsigned int level, const sf::Vector2i& pos,
                                                core::map::Collision value,
                                                core::map::Collision ogVal)
: level(level)
, pos(pos)
, value(value)
, ogVal(ogVal) {}

bool EditMap::SetCollisionAction::apply(EditMap& map) {
    map.levels[level].collisionLayer().set(pos.x, pos.y, value);
    return false;
}

bool EditMap::SetCollisionAction::undo(EditMap& map) {
    map.levels[level].collisionLayer().set(pos.x, pos.y, ogVal);
    return false;
}

const char* EditMap::SetCollisionAction::description() const { return "set collision"; }

EditMap::Action::Ptr EditMap::SetCollisionAreaAction::create(unsigned int level,
                                                             const sf::IntRect& area,
                                                             core::map::Collision value,
                                                             const EditMap& map) {
    bl::container::Vector2D<core::map::Collision> ogcols;
    ogcols.setSize(area.width, area.height, core::map::Collision::Open);
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            ogcols(x - area.left, y - area.top) = map.levels[level].collisionLayer().get(x, y);
        }
    }
    return Ptr(new SetCollisionAreaAction(level, area, value, std::move(ogcols)));
}

EditMap::SetCollisionAreaAction::SetCollisionAreaAction(
    unsigned int level, const sf::IntRect& area, core::map::Collision value,
    bl::container::Vector2D<core::map::Collision>&& ogcols)
: level(level)
, area(area)
, value(value)
, ogVals(ogcols) {}

bool EditMap::SetCollisionAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].collisionLayer().set(x, y, value);
        }
    }
    return false;
}

bool EditMap::SetCollisionAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].collisionLayer().set(x, y, ogVals(x - area.left, y - area.top));
        }
    }
    return false;
}

const char* EditMap::SetCollisionAreaAction::description() const { return "set col area"; }

EditMap::Action::Ptr EditMap::SetCatchAction::create(unsigned int level, const sf::Vector2i& pos,
                                                     core::map::Catch value, const EditMap& map) {
    return Ptr(
        new SetCatchAction(level, pos, value, map.levels[level].catchLayer().get(pos.x, pos.y)));
}

EditMap::SetCatchAction::SetCatchAction(unsigned int level, const sf::Vector2i& pos,
                                        core::map::Catch value, core::map::Catch ogVal)
: level(level)
, pos(pos)
, value(value)
, ogVal(ogVal) {}

bool EditMap::SetCatchAction::apply(EditMap& map) {
    map.levels[level].catchLayer().set(pos.x, pos.y, value);
    return false;
}

bool EditMap::SetCatchAction::undo(EditMap& map) {
    map.levels[level].catchLayer().set(pos.x, pos.y, ogVal);
    return false;
}

const char* EditMap::SetCatchAction::description() const { return "set catch tile"; }

EditMap::Action::Ptr EditMap::SetCatchAreaAction::create(unsigned int level,
                                                         const sf::IntRect& area,
                                                         core::map::Catch value,
                                                         const EditMap& map) {
    bl::container::Vector2D<core::map::Catch> ogcols;
    ogcols.setSize(area.width, area.height, core::map::Catch::NoEncounter);
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            ogcols(x - area.left, y - area.top) = map.levels[level].catchLayer().get(x, y);
        }
    }
    return Ptr(new SetCatchAreaAction(level, area, value, std::move(ogcols)));
}

EditMap::SetCatchAreaAction::SetCatchAreaAction(unsigned int level, const sf::IntRect& area,
                                                core::map::Catch value,
                                                bl::container::Vector2D<core::map::Catch>&& ogcols)
: level(level)
, area(area)
, value(value)
, ogVals(ogcols) {}

bool EditMap::SetCatchAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].catchLayer().set(x, y, value);
        }
    }
    return false;
}

bool EditMap::SetCatchAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].catchLayer().set(x, y, ogVals(x - area.left, y - area.top));
        }
    }
    return false;
}

const char* EditMap::SetCatchAreaAction::description() const { return "set catch area"; }

EditMap::Action::Ptr EditMap::SetPlaylistAction::create(const std::string& playlist,
                                                        const EditMap& map) {
    return Ptr(new SetPlaylistAction(map.playlistField.getValue(), playlist));
}

EditMap::SetPlaylistAction::SetPlaylistAction(const std::string& orig, const std::string& playlist)
: orig(orig)
, playlist(playlist) {}

bool EditMap::SetPlaylistAction::apply(EditMap& map) {
    map.playlistField = playlist;
    return true;
}

bool EditMap::SetPlaylistAction::undo(EditMap& map) {
    map.playlistField = orig;
    return true;
}

const char* EditMap::SetPlaylistAction::description() const { return "set playlist"; }

EditMap::Action::Ptr EditMap::SetWeatherAction::create(core::map::Weather::Type type,
                                                       const EditMap& map) {
    return Ptr(new SetWeatherAction(type, map.weatherField.getValue()));
}

EditMap::SetWeatherAction::SetWeatherAction(core::map::Weather::Type type,
                                            core::map::Weather::Type orig)
: type(type)
, orig(orig) {}

bool EditMap::SetWeatherAction::apply(EditMap& map) {
    map.weatherField = type;
    map.weatherSystem().set(type, false);
    return true;
}

bool EditMap::SetWeatherAction::undo(EditMap& map) {
    map.weatherField = orig;
    map.weatherSystem().set(orig, false);
    return true;
}

const char* EditMap::SetWeatherAction::description() const { return "set weather"; }

EditMap::Action::Ptr EditMap::AppendLayerAction::create(unsigned int level, Location l) {
    return Ptr(new AppendLayerAction(level, l));
}

EditMap::AppendLayerAction::AppendLayerAction(unsigned int lv, Location l)
: level(lv)
, location(l) {}

bool EditMap::AppendLayerAction::apply(EditMap& map) {
    core::map::LayerSet& lv               = map.levels[level];
    std::vector<core::map::TileLayer>* ls = nullptr;
    switch (location) {
    case Bottom:
        ls = &lv.bottomLayers();
        break;
    case YSort:
        ls = &lv.ysortLayers();
        break;
    default:
        ls = &lv.topLayers();
        break;
    }
    const unsigned int i = ls->size();

    ls->emplace_back();
    ls->back().create(map.sizeTiles().x, map.sizeTiles().y, core::map::Tile::Blank);
    lv.activate(*map.tileset);
    map.layerFilter[level].insert(map.layerFilter[level].begin() + i, true);
    return true;
}

bool EditMap::AppendLayerAction::undo(EditMap& map) {
    core::map::LayerSet& lv               = map.levels[level];
    std::vector<core::map::TileLayer>* ls = nullptr;
    switch (location) {
    case Bottom:
        ls = &lv.bottomLayers();
        break;
    case YSort:
        ls = &lv.ysortLayers();
        break;
    default:
        ls = &lv.topLayers();
        break;
    }

    ls->pop_back();
    map.layerFilter[level].pop_back();
    return true;
}

const char* EditMap::AppendLayerAction::description() const { return "add layer"; }

EditMap::Action::Ptr EditMap::RemoveLayerAction::create(unsigned int level, unsigned int layer,
                                                        const EditMap& map) {
    const core::map::TileLayer* ly = nullptr;
    const core::map::LayerSet& lv  = map.levels[level];
    unsigned int ay                = layer;
    if (ay < lv.bottomLayers().size()) { ly = &lv.bottomLayers()[ay]; }
    else {
        ay -= lv.bottomLayers().size();
        if (ay < lv.ysortLayers().size()) { ly = &lv.ysortLayers()[ay]; }
        else {
            ay -= lv.ysortLayers().size();
            ly = &lv.topLayers()[ay];
        }
    }
    return Ptr(new RemoveLayerAction(level, layer, *ly));
}

EditMap::RemoveLayerAction::RemoveLayerAction(unsigned int level, unsigned int layer,
                                              const core::map::TileLayer& rm)
: level(level)
, layer(layer)
, removedLayer(rm) {}

bool EditMap::RemoveLayerAction::apply(EditMap& map) {
    unsigned int ay                        = layer;
    core::map::LayerSet& lv                = map.levels[level];
    std::vector<core::map::TileLayer>* set = nullptr;
    if (ay < lv.bottomLayers().size()) { set = &lv.bottomLayers(); }
    else {
        ay -= lv.bottomLayers().size();
        if (ay < lv.ysortLayers().size()) { set = &lv.ysortLayers(); }
        else {
            ay -= lv.ysortLayers().size();
            set = &lv.topLayers();
        }
    }
    set->erase(set->begin() + ay);
    map.layerFilter[level].erase(map.layerFilter[level].begin() + layer);
    lv.renderSortedLayers().clear();
    lv.activate(*map.tileset);
    return true;
}

bool EditMap::RemoveLayerAction::undo(EditMap& map) {
    unsigned int ay                        = layer;
    core::map::LayerSet& lv                = map.levels[level];
    std::vector<core::map::TileLayer>* set = nullptr;
    if (ay <= lv.bottomLayers().size()) { set = &lv.bottomLayers(); }
    else {
        ay -= lv.bottomLayers().size();
        if (ay <= lv.ysortLayers().size()) { set = &lv.ysortLayers(); }
        else {
            ay -= lv.ysortLayers().size();
            set = &lv.topLayers();
        }
    }
    set->insert(set->begin() + ay, removedLayer);
    map.layerFilter[level].insert(map.layerFilter[level].begin() + layer, true);
    lv.renderSortedLayers().clear();
    map.levels[level].activate(*map.tileset);
    return true;
}

const char* EditMap::RemoveLayerAction::description() const { return "delete layer"; }

EditMap::Action::Ptr EditMap::ShiftLayerAction::create(unsigned int level, unsigned int layer,
                                                       bool up) {
    return Ptr(new ShiftLayerAction(level, layer, up));
}

EditMap::ShiftLayerAction::ShiftLayerAction(unsigned int level, unsigned int layer, bool up)
: level(level)
, layer(layer)
, up(up) {}

bool EditMap::ShiftLayerAction::apply(EditMap& map) {
    if (up) { shiftLayerUp(map.levels[level], layer, *map.tileset, map.layerFilter[level]); }
    else {
        shiftLayerDown(map.levels[level], layer, *map.tileset, map.layerFilter[level]);
    }
    return true;
}

bool EditMap::ShiftLayerAction::undo(EditMap& map) {
    if (up) {
        unsigned int ay = layer - 1;
        if (layer == map.levels[level].bottomLayers().size() - 1 ||
            layer == map.levels[level].bottomLayers().size() +
                         map.levels[level].ysortLayers().size() - 1) {
            ay = layer;
        }
        shiftLayerDown(map.levels[level], ay, *map.tileset, map.layerFilter[level]);
    }
    else {
        unsigned int ay = layer + 1;
        if (layer == map.levels[level].bottomLayers().size() ||
            layer ==
                map.levels[level].bottomLayers().size() + map.levels[level].ysortLayers().size()) {
            ay = layer;
        }
        shiftLayerUp(map.levels[level], ay, *map.tileset, map.layerFilter[level]);
    }
    return true;
}

const char* EditMap::ShiftLayerAction::description() const { return "shift layer"; }

} // namespace component
} // namespace editor
