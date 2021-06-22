#include "MapActions.hpp"

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
    if (layer < lv.bottomLayers().size()) { ly = &lv.bottomLayers()[layer]; }
    else {
        layer -= lv.bottomLayers().size();
        if (layer < lv.ysortLayers().size()) { ly = &lv.ysortLayers()[layer]; }
        else {
            ly = &lv.topLayers()[layer - lv.ysortLayers().size()];
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
    return true;
}

const char* EditMap::RemoveLayerAction::description() const { return "delete layer"; }

} // namespace component
} // namespace editor
