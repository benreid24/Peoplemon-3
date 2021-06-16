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
                return true;
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
                return true;
            }
        }
    }
    return false;
}

const char* EditMap::SetTileAction::description() const { return "tet tile"; }

} // namespace component
} // namespace editor
