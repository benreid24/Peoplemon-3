#include "MapActions.hpp"

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

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

void shiftLevelDown(std::vector<core::map::LayerSet>& levels, core::map::Tileset& ts,
                    unsigned int i) {
    std::swap(levels[i], levels[i + 1]);
    levels[i].activate(ts);
    levels[i + 1].activate(ts);
}

void shiftLevelUp(std::vector<core::map::LayerSet>& levels, core::map::Tileset& ts,
                  unsigned int i) {
    std::swap(levels[i], levels[i - 1]);
    levels[i].activate(ts);
    levels[i - 1].activate(ts);
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

EditMap::Action::Ptr EditMap::SetNameAction::create(const std::string& name, const EditMap& map) {
    return Ptr(new SetNameAction(map.nameField.getValue(), name));
}

EditMap::SetNameAction::SetNameAction(const std::string& orig, const std::string& name)
: orig(orig)
, name(name) {}

bool EditMap::SetNameAction::apply(EditMap& map) {
    map.nameField = name;
    return true;
}

bool EditMap::SetNameAction::undo(EditMap& map) {
    map.nameField = orig;
    return true;
}

const char* EditMap::SetNameAction::description() const { return "set name"; }

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

EditMap::Action::Ptr EditMap::ShiftLevelAction::create(unsigned int level, bool up) {
    return Ptr(new ShiftLevelAction(level, up));
}

EditMap::ShiftLevelAction::ShiftLevelAction(unsigned int level, bool up)
: level(level)
, up(up) {}

bool EditMap::ShiftLevelAction::apply(EditMap& map) {
    if (up) {
        shiftLevelUp(map.levels, *map.tileset, level);
        std::swap(map.levelFilter[level], map.levelFilter[level - 1]);
    }
    else {
        shiftLevelDown(map.levels, *map.tileset, level);
        std::swap(map.levelFilter[level], map.levelFilter[level + 1]);
    }
    return true;
}

bool EditMap::ShiftLevelAction::undo(EditMap& map) {
    if (!up) {
        shiftLevelUp(map.levels, *map.tileset, level + 1);
        std::swap(map.levelFilter[level], map.levelFilter[level + 1]);
    }
    else {
        shiftLevelDown(map.levels, *map.tileset, level - 1);
        std::swap(map.levelFilter[level], map.levelFilter[level - 1]);
    }
    return true;
}

const char* EditMap::ShiftLevelAction::description() const { return "shift level"; }

EditMap::Action::Ptr EditMap::AppendLevelAction::create() { return Ptr(new AppendLevelAction()); }

bool EditMap::AppendLevelAction::apply(EditMap& map) {
    map.levelFilter.push_back(true);
    map.layerFilter.emplace_back(5, true);
    map.levels.emplace_back();
    map.levels.back().init(map.sizeTiles().x, map.sizeTiles().y, 2, 1, 1);
    map.systems->position().editorPushLevel();
    for (auto& level : map.levels) { level.activate(*map.tileset); }
    return true;
}

bool EditMap::AppendLevelAction::undo(EditMap& map) {
    map.levels.pop_back();
    map.levelFilter.pop_back();
    map.layerFilter.pop_back();
    map.systems->position().editorPopLevel();
    return true;
}

const char* EditMap::AppendLevelAction::description() const { return "add level"; }

EditMap::Action::Ptr EditMap::SetScriptAction::create(bool l, const std::string& s,
                                                      const std::string& p) {
    return Ptr(new SetScriptAction(l, s, p));
}

EditMap::SetScriptAction::SetScriptAction(bool l, const std::string& s, const std::string& p)
: load(l)
, s(s)
, p(p) {}

bool EditMap::SetScriptAction::apply(EditMap& map) {
    if (load) { map.loadScriptField = s; }
    else {
        map.unloadScriptField = s;
    }
    return true;
}

bool EditMap::SetScriptAction::undo(EditMap& map) {
    if (load) { map.loadScriptField = p; }
    else {
        map.unloadScriptField = p;
    }
    return true;
}

const char* EditMap::SetScriptAction::description() const {
    if (load) { return "set load script"; }
    return "set unload script";
}

EditMap::Action::Ptr EditMap::AddEventAction::create(const core::map::Event& e, unsigned int i) {
    return Ptr(new AddEventAction(e, i));
}

EditMap::AddEventAction::AddEventAction(const core::map::Event& e, unsigned int i)
: event(e)
, i(i) {}

bool EditMap::AddEventAction::apply(EditMap& map) {
    map.eventsField.getValue().emplace_back(event);
    return false;
}

bool EditMap::AddEventAction::undo(EditMap& map) {
    map.eventsField.getValue().erase(map.eventsField.getValue().begin() + i);
    return false;
}

const char* EditMap::AddEventAction::description() const { return "add event"; }

EditMap::Action::Ptr EditMap::EditEventAction::create(const core::map::Event& o,
                                                      const core::map::Event& e, unsigned int i) {
    return Ptr(new EditEventAction(o, e, i));
}

EditMap::EditEventAction::EditEventAction(const core::map::Event& o, const core::map::Event& e,
                                          unsigned int i)
: orig(o)
, val(e)
, i(i) {}

bool EditMap::EditEventAction::apply(EditMap& map) {
    map.eventsField.getValue()[i] = val;
    return false;
}

bool EditMap::EditEventAction::undo(EditMap& map) {
    map.eventsField.getValue()[i] = orig;
    return false;
}

const char* EditMap::EditEventAction::description() const { return "edit event"; }

EditMap::Action::Ptr EditMap::RemoveEventAction::create(const core::map::Event& e, unsigned int i) {
    return Ptr(new RemoveEventAction(e, i));
}

EditMap::RemoveEventAction::RemoveEventAction(const core::map::Event& e, unsigned int i)
: event(e)
, i(i) {}

bool EditMap::RemoveEventAction::apply(EditMap& map) {
    map.eventsField.getValue().erase(map.eventsField.getValue().begin() + i);
    return false;
}

bool EditMap::RemoveEventAction::undo(EditMap& map) {
    map.eventsField.getValue().insert(map.eventsField.getValue().begin() + i, event);
    return false;
}

const char* EditMap::RemoveEventAction::description() const { return "remove event"; }

EditMap::Action::Ptr EditMap::AddSpawnAction::create(unsigned int l, const sf::Vector2i& pos,
                                                     unsigned int id,
                                                     core::component::Direction dir) {
    return Ptr(new AddSpawnAction(l, pos, id, dir));
}

EditMap::AddSpawnAction::AddSpawnAction(unsigned int l, const sf::Vector2i& p, unsigned int id,
                                        core::component::Direction dir)
: level(l)
, pos(p)
, id(id)
, dir(dir) {}

bool EditMap::AddSpawnAction::apply(EditMap& map) {
    map.spawnField.getValue()[id] =
        core::map::Spawn(id, core::component::Position(level, pos, dir));
    return false;
}

bool EditMap::AddSpawnAction::undo(EditMap& map) {
    map.spawnField.getValue().erase(id);
    return false;
}

const char* EditMap::AddSpawnAction::description() const { return "add spwwn"; }

EditMap::Action::Ptr EditMap::RotateSpawnAction::create(unsigned int id) {
    return Ptr(new RotateSpawnAction(id));
}

EditMap::RotateSpawnAction::RotateSpawnAction(unsigned int id)
: id(id) {}

bool EditMap::RotateSpawnAction::apply(EditMap& map) {
    using namespace core::component;

    auto& d = map.spawnField.getValue()[id].position.getValue().direction;
    switch (d) {
    case Direction::Up:
        d = Direction::Right;
        break;
    case Direction::Right:
        d = Direction::Down;
        break;
    case Direction::Down:
        d = Direction::Left;
        break;
    case Direction::Left:
    default:
        d = Direction::Up;
    }
    return false;
}

bool EditMap::RotateSpawnAction::undo(EditMap& map) {
    using namespace core::component;

    auto& d = map.spawnField.getValue()[id].position.getValue().direction;
    switch (d) {
    case Direction::Up:
        d = Direction::Left;
        break;
    case Direction::Right:
        d = Direction::Up;
        break;
    case Direction::Down:
        d = Direction::Right;
        break;
    case Direction::Left:
    default:
        d = Direction::Down;
    }
    return false;
}

const char* EditMap::RotateSpawnAction::description() const { return "rotate spwwn"; }

EditMap::Action::Ptr EditMap::RemoveSpawnAction::create(unsigned int id,
                                                        const core::map::Spawn& spawn) {
    return Ptr(new RemoveSpawnAction(id, spawn));
}

EditMap::RemoveSpawnAction::RemoveSpawnAction(unsigned int id, const core::map::Spawn& spawn)
: id(id)
, spawn(spawn) {}

bool EditMap::RemoveSpawnAction::apply(EditMap& map) {
    map.spawnField.getValue().erase(id);
    return false;
}

bool EditMap::RemoveSpawnAction::undo(EditMap& map) {
    map.spawnField.getValue()[id] = spawn;
    return false;
}

const char* EditMap::RemoveSpawnAction::description() const { return "delete spawn"; }

EditMap::Action::Ptr EditMap::AddNpcSpawnAction::create(const core::map::CharacterSpawn& s,
                                                        unsigned int i) {
    return Ptr(new AddNpcSpawnAction(s, i));
}

EditMap::AddNpcSpawnAction::AddNpcSpawnAction(const core::map::CharacterSpawn& s, unsigned int i)
: spawn(s)
, i(i)
, spawned(bl::entity::InvalidEntity) {}

bool EditMap::AddNpcSpawnAction::apply(EditMap& map) {
    map.characterField.getValue().push_back(spawn);
    spawned = map.systems->entity().spawnCharacter(spawn);
    return false;
}

bool EditMap::AddNpcSpawnAction::undo(EditMap& map) {
    map.characterField.getValue().erase(map.characterField.getValue().begin() + i);
    if (spawned != bl::entity::InvalidEntity) {
        map.systems->engine().entities().destroyEntity(spawned);
        spawned = bl::entity::InvalidEntity;
    }
    return false;
}

const char* EditMap::AddNpcSpawnAction::description() const { return "add character"; }

EditMap::Action::Ptr EditMap::EditNpcSpawnAction::create(const core::map::CharacterSpawn* spawn,
                                                         const core::map::CharacterSpawn& s,
                                                         bl::entity::Entity id) {
    return Ptr(new EditNpcSpawnAction(spawn, s, id));
}

EditMap::EditNpcSpawnAction::EditNpcSpawnAction(const core::map::CharacterSpawn* spawn,
                                                const core::map::CharacterSpawn& s,
                                                bl::entity::Entity id)
: spawn(const_cast<core::map::CharacterSpawn*>(spawn))
, orig(*spawn)
, value(s)
, latestId(id) {}

bool EditMap::EditNpcSpawnAction::apply(EditMap& map) {
    map.systems->engine().entities().destroyEntity(latestId);
    latestId = map.systems->entity().spawnCharacter(value);
    *spawn   = value;
    return false;
}

bool EditMap::EditNpcSpawnAction::undo(EditMap& map) {
    map.systems->engine().entities().destroyEntity(latestId);
    latestId = map.systems->entity().spawnCharacter(orig);
    *spawn   = orig;
    return false;
}

const char* EditMap::EditNpcSpawnAction::description() const { return "edit character"; }

EditMap::Action::Ptr EditMap::RemoveNpcSpawnAction::create(const core::map::CharacterSpawn& orig,
                                                           unsigned int i,
                                                           bl::entity::Entity spawned) {
    return Ptr(new RemoveNpcSpawnAction(orig, i, spawned));
}

EditMap::RemoveNpcSpawnAction::RemoveNpcSpawnAction(const core::map::CharacterSpawn& orig,
                                                    unsigned int i, bl::entity::Entity spawned)
: orig(orig)
, i(i)
, spawned(spawned) {}

bool EditMap::RemoveNpcSpawnAction::apply(EditMap& map) {
    map.characterField.getValue().erase(map.characterField.getValue().begin() + i);
    map.systems->engine().entities().destroyEntity(spawned);
    return false;
}

bool EditMap::RemoveNpcSpawnAction::undo(EditMap& map) {
    map.characterField.getValue().insert(map.characterField.getValue().begin() + i, orig);
    spawned = map.systems->entity().spawnCharacter(orig);
    return false;
}

const char* EditMap::RemoveNpcSpawnAction::description() const { return "remove character"; }

EditMap::Action::Ptr EditMap::AddOrEditItemAction::create(unsigned int i, unsigned int level,
                                                          const sf::Vector2i& pos,
                                                          core::item::Id item, bool visible,
                                                          const core::map::Item& orig, bool a) {
    return Action::Ptr(new AddOrEditItemAction(i, level, pos, item, visible, orig, a));
}

EditMap::AddOrEditItemAction::AddOrEditItemAction(unsigned int i, unsigned int level,
                                                  const sf::Vector2i& pos, core::item::Id item,
                                                  bool visible, const core::map::Item& orig, bool a)
: i(i)
, level(level)
, position(pos)
, item(item)
, visible(visible)
, orig(orig)
, add(a) {}

bool EditMap::AddOrEditItemAction::apply(EditMap& map) {
    if (add) {
        map.itemsField.getValue().emplace_back(
            static_cast<std::uint16_t>(item), map.nextItemId, position, level, visible);
        ++map.nextItemId;
        map.systems->entity().spawnItem(
            core::map::Item(static_cast<std::uint16_t>(item), 0, position, level, visible));
    }
    else {
        map.itemsField.getValue()[i].id      = static_cast<std::uint16_t>(item);
        map.itemsField.getValue()[i].visible = visible;
    }
    return false;
}

bool EditMap::AddOrEditItemAction::undo(EditMap& map) {
    if (add) {
        map.itemsField.getValue().pop_back();
        const auto ent = map.systems->position().getEntity(
            {static_cast<std::uint8_t>(level), position, core::component::Direction::Up});
        if (ent != bl::entity::InvalidEntity) {
            map.systems->engine().entities().destroyEntity(ent);
        }
    }
    else {
        map.itemsField.getValue()[i].id      = orig.id;
        map.itemsField.getValue()[i].visible = orig.visible;
    }
    return false;
}

const char* EditMap::AddOrEditItemAction::description() const {
    return add ? "spawn item" : "edit item";
}

EditMap::Action::Ptr EditMap::RemoveItemAction::create(unsigned int i, unsigned int level,
                                                       const sf::Vector2i& pos,
                                                       const core::map::Item& orig) {
    return Action::Ptr(new RemoveItemAction(i, level, pos, orig));
}

EditMap::RemoveItemAction::RemoveItemAction(unsigned int i, unsigned int level,
                                            const sf::Vector2i& pos, const core::map::Item& orig)
: i(i)
, level(level)
, position(pos)
, orig(orig) {}

bool EditMap::RemoveItemAction::apply(EditMap& map) {
    map.itemsField.getValue().erase(map.itemsField.getValue().begin() + i);
    const auto ent = map.systems->position().getEntity(
        {static_cast<std::uint8_t>(level), position, core::component::Direction::Up});
    if (ent != bl::entity::InvalidEntity) { map.systems->engine().entities().destroyEntity(ent); }
    return false;
}

bool EditMap::RemoveItemAction::undo(EditMap& map) {
    map.itemsField.getValue().insert(map.itemsField.getValue().begin() + i, orig);
    map.systems->entity().spawnItem(orig);
    return false;
}

const char* EditMap::RemoveItemAction::description() const { return "remove item"; }

} // namespace component
} // namespace editor
