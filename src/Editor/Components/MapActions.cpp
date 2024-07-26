#include "MapActions.hpp"

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Pages/Subpages/Towns.hpp>

namespace editor
{
namespace component
{
namespace
{
const core::map::Tile& getTile(const std::vector<core::map::LayerSet>& levels, unsigned int level,
                               unsigned int layer, const sf::Vector2i& pos) {
    const core::map::LayerSet& l = levels[level];
    if (layer >= l.bottomLayers().size() + l.ysortLayers().size()) {
        const unsigned int i = layer - l.ysortLayers().size() - l.bottomLayers().size();
        return l.topLayers()[i].get(pos.x, pos.y);
    }
    else if (layer >= l.bottomLayers().size()) {
        const unsigned int i = layer - l.bottomLayers().size();
        return l.ysortLayers()[i].get(pos.x, pos.y);
    }
    else { return l.bottomLayers()[layer].get(pos.x, pos.y); }
}

const core::map::Tile& getTile(const std::vector<core::map::LayerSet>& levels, unsigned int level,
                               unsigned int layer, const glm::u32vec2& pos) {
    return getTile(levels, level, layer, sf::Vector2i(pos.x, pos.y));
}

void setSingleTile(std::vector<core::map::LayerSet>& levels, core::map::Tileset& tileset,
                   unsigned int level, unsigned int layer, const sf::Vector2i& pos,
                   core::map::Tile::IdType value, bool isAnim) {
    // TODO - BLIB_UPGRADE - reset render data
    core::map::LayerSet& l = levels[level];
    core::map::Tile* tile;
    if (layer >= l.bottomLayers().size() + l.ysortLayers().size()) {
        const unsigned int i = layer - l.ysortLayers().size() - l.bottomLayers().size();
        tile                 = &l.topLayers()[i].getRef(pos.x, pos.y);
        tile->set(value, isAnim);
    }
    else if (layer >= l.bottomLayers().size()) {
        const unsigned int i = layer - l.bottomLayers().size();
        tile                 = &l.ysortLayers()[i].getRef(pos.x, pos.y);
        tile->set(value, isAnim);
    }
    else {
        tile = &l.bottomLayers()[layer].getRef(pos.x, pos.y);
        tile->set(value, isAnim);
    }
}

void shiftLayerUp(core::map::LayerSet& level, unsigned int layer, core::map::Tileset& tileset,
                  std::vector<bool>& filter) {
    // TODO - BLIB_UPGRADE - reset render data
    if (layer < level.bottomLayers().size()) {
        std::swap(level.bottomLayers()[layer], level.bottomLayers()[layer - 1]);
        std::vector<bool>::swap(filter[layer], filter[layer - 1]);
    }
    else {
        layer -= level.bottomLayers().size();
        if (layer == 0) {
            core::map::TileLayer temp(std::move(level.ysortLayers()[layer]));
            level.ysortLayers().erase(level.ysortLayers().begin() + layer);
            level.bottomLayers().emplace_back(std::move(temp));
        }
        else if (layer < level.ysortLayers().size()) {
            std::swap(level.ysortLayers()[layer], level.ysortLayers()[layer - 1]);
            std::vector<bool>::swap(filter[layer], filter[layer - 1]);
        }
        else {
            layer -= level.ysortLayers().size();
            if (layer == 0) {
                core::map::TileLayer temp(std::move(level.topLayers()[layer]));
                level.topLayers().erase(level.topLayers().begin() + layer);
                level.ysortLayers().emplace_back(std::move(temp));
            }
            else {
                std::swap(level.topLayers()[layer], level.topLayers()[layer - 1]);
                std::vector<bool>::swap(filter[layer], filter[layer - 1]);
            }
        }
    }
}

void shiftLayerDown(core::map::LayerSet& level, unsigned int layer, core::map::Tileset& tileset,
                    std::vector<bool>& filter) {
    // TODO - BLIB_UPGRADE - reset render data

    if (layer < level.bottomLayers().size() - 1) {
        std::swap(level.bottomLayers()[layer], level.bottomLayers()[layer + 1]);
        std::vector<bool>::swap(filter[layer], filter[layer + 1]);
    }
    else if (layer == level.bottomLayers().size() - 1) {
        core::map::TileLayer temp(std::move(level.bottomLayers()[layer]));
        level.bottomLayers().erase(level.bottomLayers().begin() + layer);
        level.ysortLayers().emplace(level.ysortLayers().begin(), std::move(temp));
    }
    else {
        layer -= level.bottomLayers().size();
        if (layer < level.ysortLayers().size() - 1) {
            std::swap(level.ysortLayers()[layer], level.ysortLayers()[layer + 1]);
            std::vector<bool>::swap(filter[layer], filter[layer + 1]);
        }
        else if (layer == level.ysortLayers().size() - 1) {
            core::map::TileLayer temp(std::move(level.ysortLayers()[layer]));
            level.ysortLayers().erase(level.ysortLayers().begin() + layer);
            level.topLayers().emplace(level.topLayers().begin(), std::move(temp));
        }
        else {
            layer -= level.ysortLayers().size();
            std::swap(level.topLayers()[layer], level.topLayers()[layer + 1]);
            std::vector<bool>::swap(filter[layer], filter[layer + 1]);
        }
    }
}

void shiftLevelDown(std::vector<core::map::LayerSet>& levels, core::map::Tileset& ts,
                    unsigned int i) {
    // TODO - BLIB_UPGRADE - reset render data
    std::swap(levels[i], levels[i + 1]);
}

void shiftLevelUp(std::vector<core::map::LayerSet>& levels, core::map::Tileset& ts,
                  unsigned int i) {
    // TODO - BLIB_UPGRADE - reset render data
    std::swap(levels[i], levels[i - 1]);
}

void setNeighbors(const glm::i32vec2& pos, glm::i32vec2* neighbors) {
    neighbors[0] = {pos.x - 1, pos.y};
    neighbors[1] = {pos.x, pos.y - 1};
    neighbors[2] = {pos.x + 1, pos.y};
    neighbors[3] = {pos.x, pos.y + 1};
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
            const core::map::LayerSet& l = map.levels[level];
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

    bl::ctr::Vector2D<core::map::Tile::IdType> prev;
    bl::ctr::Vector2D<std::uint8_t> wasAnim;
    prev.setSize(area.width, area.height, core::map::Tile::Blank);
    wasAnim.setSize(area.width, area.height, 0);

    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            const auto& tile = getTile(map.levels, level, layer, sf::Vector2i{x, y});
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

EditMap::SetTileAreaAction::SetTileAreaAction(unsigned int level, unsigned int layer,
                                              const sf::IntRect& area,
                                              bl::ctr::Vector2D<core::map::Tile::IdType>&& prev,
                                              bl::ctr::Vector2D<std::uint8_t>&& wasAnim,
                                              core::map::Tile::IdType value, bool isAnim, int w,
                                              int h)
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

EditMap::Action::Ptr EditMap::FillTileAction::create(unsigned int level, unsigned int layer,
                                                     const sf::Vector2i& pos,
                                                     core::map::Tile::IdType id, bool isAnim,
                                                     EditMap& map) {
    const core::map::Tile& tile = getTile(map.levels, level, layer, pos);
    const unsigned int estSize  = map.sizeTiles().x * map.sizeTiles().y / 8;

    bl::ctr::Vector2D<std::uint8_t> visited;
    visited.setSize(map.sizeTiles().x, map.sizeTiles().y, 0);
    visited(pos.x, pos.y) = 1;

    std::vector<glm::i32vec2> toVisit;
    toVisit.reserve(estSize);
    toVisit.emplace_back(glm::i32vec2(pos.x, pos.y));

    std::vector<FillTile> set;
    set.reserve(estSize);

    while (!toVisit.empty()) {
        const glm::i32vec2 p = toVisit.back();
        toVisit.pop_back();
        set.emplace_back(sf::Vector2i(p.x, p.y), getTile(map.levels, level, layer, p));

        glm::i32vec2 nps[4];
        setNeighbors(p, nps);
        for (unsigned int i = 0; i < 4; ++i) {
            const auto& np = nps[i];
            if (!map.contains({0, np, bl::tmap::Direction::Up})) continue;

            if (visited(np.x, np.y) == 0) {
                visited(np.x, np.y) = 1;
                const auto& t       = getTile(map.levels, level, layer, np);
                if (t.id() == tile.id() && t.isAnimation() == tile.isAnimation()) {
                    toVisit.emplace_back(np);
                }
            }
        }
    }

    return Ptr(new FillTileAction(level, layer, id, isAnim, std::move(set)));
}

EditMap::FillTileAction::FillTileAction(unsigned int level, unsigned int layer,
                                        core::map::Tile::IdType id, bool isAnim,
                                        std::vector<FillTile>&& set)
: level(level)
, layer(layer)
, id(id)
, isAnim(isAnim)
, set(set) {}

bool EditMap::FillTileAction::apply(EditMap& map) {
    for (const auto& p : set) {
        setSingleTile(map.levels, *map.tileset, level, layer, p.position, id, isAnim);
    }
    return false;
}

bool EditMap::FillTileAction::undo(EditMap& map) {
    for (const auto& p : set) {
        setSingleTile(map.levels, *map.tileset, level, layer, p.position, p.id, p.isAnim);
    }
    return false;
}

const char* EditMap::FillTileAction::description() const { return "fill tiles"; }

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
    bl::ctr::Vector2D<core::map::Collision> ogcols;
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
    bl::ctr::Vector2D<core::map::Collision>&& ogcols)
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

EditMap::Action::Ptr EditMap::FillCollisionAction::create(unsigned int level,
                                                          const sf::Vector2i& pos,
                                                          core::map::Collision col, EditMap& map) {
    const core::map::Collision oc = map.levels[level].collisionLayer().get(pos.x, pos.y);
    const unsigned int estSize    = map.sizeTiles().x * map.sizeTiles().y / 8;

    bl::ctr::Vector2D<std::uint8_t> visited;
    visited.setSize(map.sizeTiles().x, map.sizeTiles().y, 0);
    visited(pos.x, pos.y) = 1;

    std::vector<glm::i32vec2> toVisit;
    toVisit.reserve(estSize);
    toVisit.emplace_back(glm::i32vec2(pos.x, pos.y));

    std::vector<std::pair<sf::Vector2i, core::map::Collision>> set;
    set.reserve(estSize);

    while (!toVisit.empty()) {
        const auto p = toVisit.back();
        toVisit.pop_back();
        set.emplace_back(sf::Vector2i(p.x, p.y), map.levels[level].collisionLayer().get(p.x, p.y));

        glm::i32vec2 nps[4];
        setNeighbors(p, nps);
        for (unsigned int i = 0; i < 4; ++i) {
            const auto& np = nps[i];
            if (!map.contains({0, np, bl::tmap::Direction::Up})) continue;

            if (map.levels[level].collisionLayer().get(np.x, np.y) == oc &&
                visited(np.x, np.y) == 0) {
                visited(np.x, np.y) = 1;
                toVisit.emplace_back(np);
            }
        }
    }

    return Ptr(new FillCollisionAction(level, col, std::move(set)));
}

EditMap::FillCollisionAction::FillCollisionAction(
    unsigned int level, core::map::Collision col,
    std::vector<std::pair<sf::Vector2i, core::map::Collision>>&& set)
: level(level)
, col(col)
, set(set) {}

bool EditMap::FillCollisionAction::apply(EditMap& map) {
    for (const auto& p : set) { map.levels[level].collisionLayer().set(p.first.x, p.first.y, col); }
    return false;
}

bool EditMap::FillCollisionAction::undo(EditMap& map) {
    for (const auto& p : set) {
        map.levels[level].collisionLayer().set(p.first.x, p.first.y, p.second);
    }
    return false;
}

const char* EditMap::FillCollisionAction::description() const { return "fill collisions"; }

EditMap::Action::Ptr EditMap::SetCatchAction::create(unsigned int level, const sf::Vector2i& pos,
                                                     std::uint8_t value, const EditMap& map) {
    return Ptr(
        new SetCatchAction(level, pos, value, map.levels[level].catchLayer().get(pos.x, pos.y)));
}

EditMap::SetCatchAction::SetCatchAction(unsigned int level, const sf::Vector2i& pos,
                                        std::uint8_t value, std::uint8_t ogVal)
: level(level)
, pos(pos)
, value(value)
, ogVal(ogVal) {}

bool EditMap::SetCatchAction::apply(EditMap& map) {
    map.levels[level].catchLayer().set(pos.x, pos.y, value);
    map.updateCatchTileColor(level, pos.x, pos.y);
    return false;
}

bool EditMap::SetCatchAction::undo(EditMap& map) {
    map.levels[level].catchLayer().set(pos.x, pos.y, ogVal);
    map.updateCatchTileColor(level, pos.x, pos.y);
    return false;
}

const char* EditMap::SetCatchAction::description() const { return "set catch tile"; }

EditMap::Action::Ptr EditMap::SetCatchAreaAction::create(unsigned int level,
                                                         const sf::IntRect& area,
                                                         std::uint8_t value, const EditMap& map) {
    bl::ctr::Vector2D<std::uint8_t> ogcols;
    ogcols.setSize(area.width, area.height, 0);
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            ogcols(x - area.left, y - area.top) = map.levels[level].catchLayer().get(x, y);
        }
    }
    return Ptr(new SetCatchAreaAction(level, area, value, std::move(ogcols)));
}

EditMap::SetCatchAreaAction::SetCatchAreaAction(unsigned int level, const sf::IntRect& area,
                                                std::uint8_t value,
                                                bl::ctr::Vector2D<std::uint8_t>&& ogcols)
: level(level)
, area(area)
, value(value)
, ogVals(ogcols) {}

bool EditMap::SetCatchAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].catchLayer().set(x, y, value);
            map.updateCatchTileColor(level, x, y);
        }
    }
    return false;
}

bool EditMap::SetCatchAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.levels[level].catchLayer().set(x, y, ogVals(x - area.left, y - area.top));
            map.updateCatchTileColor(level, x, y);
        }
    }
    return false;
}

const char* EditMap::SetCatchAreaAction::description() const { return "set catch area"; }

EditMap::Action::Ptr EditMap::FillCatchAction::create(unsigned int level, const sf::Vector2i& pos,
                                                      std::uint8_t id, EditMap& map) {
    const std::uint8_t og      = map.levels[level].catchLayer().get(pos.x, pos.y);
    const unsigned int estSize = map.sizeTiles().x * map.sizeTiles().y / 8;

    bl::ctr::Vector2D<std::uint8_t> visited;
    visited.setSize(map.sizeTiles().x, map.sizeTiles().y, 0);
    visited(pos.x, pos.y) = 1;

    std::vector<glm::i32vec2> toVisit;
    toVisit.reserve(estSize);
    toVisit.emplace_back(glm::i32vec2(pos.x, pos.y));

    std::vector<std::pair<sf::Vector2i, std::uint8_t>> set;
    set.reserve(estSize);

    while (!toVisit.empty()) {
        const glm::i32vec2 p = toVisit.back();
        toVisit.pop_back();
        set.emplace_back(sf::Vector2i(p.x, p.y), map.levels[level].catchLayer().get(p.x, p.y));

        glm::i32vec2 nps[4];
        setNeighbors(p, nps);
        for (unsigned int i = 0; i < 4; ++i) {
            const auto& np = nps[i];
            if (!map.contains({0, np, bl::tmap::Direction::Up})) continue;

            if (visited(np.x, np.y) == 0 && map.levels[level].catchLayer().get(np.x, np.y) == og) {
                visited(np.x, np.y) = 1;
                toVisit.emplace_back(np);
            }
        }
    }

    return Ptr(new FillCatchAction(level, id, std::move(set)));
}

EditMap::FillCatchAction::FillCatchAction(unsigned int level, std::uint8_t id,
                                          std::vector<std::pair<sf::Vector2i, std::uint8_t>>&& set)
: level(level)
, id(id)
, set(set) {}

bool EditMap::FillCatchAction::apply(EditMap& map) {
    for (const auto& p : set) {
        map.levels[level].catchLayer().set(p.first.x, p.first.y, id);
        map.updateCatchTileColor(level, p.first.x, p.first.y);
    }
    return false;
}

bool EditMap::FillCatchAction::undo(EditMap& map) {
    for (const auto& p : set) {
        map.levels[level].catchLayer().set(p.first.x, p.first.y, p.second);
        map.updateCatchTileColor(level, p.first.x, p.first.y);
    }
    return false;
}

const char* EditMap::FillCatchAction::description() const { return "fill catch tiles"; }

EditMap::Action::Ptr EditMap::SetPlaylistAction::create(const std::string& playlist,
                                                        const EditMap& map) {
    return Ptr(new SetPlaylistAction(map.playlistField, playlist));
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
    return Ptr(new SetNameAction(map.nameField, name));
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
    return Ptr(new SetWeatherAction(type, map.weatherField));
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
    // TODO - BLIB_UPGRADE - reset render data
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
    // TODO - BLIB_UPGRADE - reset render data
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
    // TODO - BLIB_UPGRADE - reset render data
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
    else { shiftLayerDown(map.levels[level], layer, *map.tileset, map.layerFilter[level]); }
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
        std::vector<bool>::swap(map.levelFilter[level], map.levelFilter[level - 1]);
    }
    else {
        shiftLevelDown(map.levels, *map.tileset, level);
        std::vector<bool>::swap(map.levelFilter[level], map.levelFilter[level + 1]);
    }
    return true;
}

bool EditMap::ShiftLevelAction::undo(EditMap& map) {
    if (!up) {
        shiftLevelUp(map.levels, *map.tileset, level + 1);
        std::vector<bool>::swap(map.levelFilter[level], map.levelFilter[level + 1]);
    }
    else {
        shiftLevelDown(map.levels, *map.tileset, level - 1);
        std::vector<bool>::swap(map.levelFilter[level], map.levelFilter[level - 1]);
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
    // TODO - BLIB_UPGRADE - reset render data
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
    else { map.unloadScriptField = s; }
    return true;
}

bool EditMap::SetScriptAction::undo(EditMap& map) {
    if (load) { map.loadScriptField = p; }
    else { map.unloadScriptField = p; }
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
    map.eventsField.emplace_back(event);
    return false;
}

bool EditMap::AddEventAction::undo(EditMap& map) {
    map.eventsField.erase(map.eventsField.begin() + i);
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
    map.eventsField[i] = val;
    return false;
}

bool EditMap::EditEventAction::undo(EditMap& map) {
    map.eventsField[i] = orig;
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
    map.eventsField.erase(map.eventsField.begin() + i);
    return false;
}

bool EditMap::RemoveEventAction::undo(EditMap& map) {
    map.eventsField.insert(map.eventsField.begin() + i, event);
    return false;
}

const char* EditMap::RemoveEventAction::description() const { return "remove event"; }

EditMap::Action::Ptr EditMap::AddSpawnAction::create(unsigned int l, const sf::Vector2i& pos,
                                                     unsigned int id, bl::tmap::Direction dir) {
    return Ptr(new AddSpawnAction(l, pos, id, dir));
}

EditMap::AddSpawnAction::AddSpawnAction(unsigned int l, const sf::Vector2i& p, unsigned int id,
                                        bl::tmap::Direction dir)
: level(l)
, pos(p)
, id(id)
, dir(dir) {}

bool EditMap::AddSpawnAction::apply(EditMap& map) {
    const auto spawn =
        core::map::Spawn(id, bl::tmap::Position(level, glm::i32vec2(pos.x, pos.y), dir));
    map.spawns[id] = spawn;
    map.addSpawnGfx(spawn);
    return false;
}

bool EditMap::AddSpawnAction::undo(EditMap& map) {
    map.spawns.erase(id);
    map.spawnSprites.erase(id);
    return false;
}

const char* EditMap::AddSpawnAction::description() const { return "add spawn"; }

EditMap::Action::Ptr EditMap::RotateSpawnAction::create(unsigned int id) {
    return Ptr(new RotateSpawnAction(id));
}

EditMap::RotateSpawnAction::RotateSpawnAction(unsigned int id)
: id(id) {}

bool EditMap::RotateSpawnAction::apply(EditMap& map) {
    using namespace core::component;

    auto& d = map.spawns[id].position.direction;
    switch (d) {
    case bl::tmap::Direction::Up:
        d = bl::tmap::Direction::Right;
        break;
    case bl::tmap::Direction::Right:
        d = bl::tmap::Direction::Down;
        break;
    case bl::tmap::Direction::Down:
        d = bl::tmap::Direction::Left;
        break;
    case bl::tmap::Direction::Left:
    default:
        d = bl::tmap::Direction::Up;
    }
    map.updateSpawnRotation(id);
    return false;
}

bool EditMap::RotateSpawnAction::undo(EditMap& map) {
    using namespace core::component;

    auto& d = map.spawns[id].position.direction;
    switch (d) {
    case bl::tmap::Direction::Up:
        d = bl::tmap::Direction::Left;
        break;
    case bl::tmap::Direction::Right:
        d = bl::tmap::Direction::Up;
        break;
    case bl::tmap::Direction::Down:
        d = bl::tmap::Direction::Right;
        break;
    case bl::tmap::Direction::Left:
    default:
        d = bl::tmap::Direction::Down;
    }
    map.updateSpawnRotation(id);
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
    map.spawns.erase(id);
    map.spawnSprites.erase(id);
    return false;
}

bool EditMap::RemoveSpawnAction::undo(EditMap& map) {
    map.spawns[id] = spawn;
    map.addSpawnGfx(spawn);
    return false;
}

const char* EditMap::RemoveSpawnAction::description() const { return "delete spawn"; }

EditMap::Action::Ptr EditMap::AddNpcSpawnAction::create(const core::map::CharacterSpawn& s,
                                                        unsigned int i) {
    return Ptr(new AddNpcSpawnAction(s, i));
}

EditMap::AddNpcSpawnAction::AddNpcSpawnAction(const core::map::CharacterSpawn& s, unsigned int i)
: spawn(s)
, i(i) {}

bool EditMap::AddNpcSpawnAction::apply(EditMap& map) {
    map.characterField.push_back(spawn);
    map.systems->entity().spawnCharacter(spawn, map);
    return false;
}

bool EditMap::AddNpcSpawnAction::undo(EditMap& map) {
    map.characterField.erase(map.characterField.begin() + i);
    bl::ecs::Entity c = map.systems->position().getEntity(spawn.position);
    if (c != bl::ecs::InvalidEntity) { map.systems->engine().ecs().destroyEntity(c); }
    return false;
}

const char* EditMap::AddNpcSpawnAction::description() const { return "add character"; }

EditMap::Action::Ptr EditMap::EditNpcSpawnAction::create(unsigned int i,
                                                         const core::map::CharacterSpawn& spawn,
                                                         const core::map::CharacterSpawn& s) {
    return Ptr(new EditNpcSpawnAction(i, spawn, s));
}

EditMap::EditNpcSpawnAction::EditNpcSpawnAction(unsigned int i,
                                                const core::map::CharacterSpawn& spawn,
                                                const core::map::CharacterSpawn& s)
: i(i)
, orig(spawn)
, value(s) {}

bool EditMap::EditNpcSpawnAction::apply(EditMap& map) {
    bl::ecs::Entity e = map.systems->position().getEntity(orig.position);
    map.systems->engine().ecs().destroyEntity(e);
    map.systems->entity().spawnCharacter(value, map);
    map.characterField[i] = value;
    return false;
}

bool EditMap::EditNpcSpawnAction::undo(EditMap& map) {
    bl::ecs::Entity e = map.systems->position().getEntity(value.position);
    map.systems->engine().ecs().destroyEntity(e);
    map.systems->entity().spawnCharacter(orig, map);
    map.characterField[i] = orig;
    return false;
}

const char* EditMap::EditNpcSpawnAction::description() const { return "edit character"; }

EditMap::Action::Ptr EditMap::RemoveNpcSpawnAction::create(const core::map::CharacterSpawn& orig,
                                                           unsigned int i) {
    return Ptr(new RemoveNpcSpawnAction(orig, i));
}

EditMap::RemoveNpcSpawnAction::RemoveNpcSpawnAction(const core::map::CharacterSpawn& orig,
                                                    unsigned int i)
: orig(orig)
, i(i) {}

bool EditMap::RemoveNpcSpawnAction::apply(EditMap& map) {
    map.characterField.erase(map.characterField.begin() + i);
    bl::ecs::Entity e = map.systems->position().getEntity(orig.position);
    map.systems->engine().ecs().destroyEntity(e);
    return false;
}

bool EditMap::RemoveNpcSpawnAction::undo(EditMap& map) {
    map.characterField.insert(map.characterField.begin() + i, orig);
    map.systems->entity().spawnCharacter(orig, map);
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
        map.itemsField.emplace_back(
            static_cast<std::uint16_t>(item), map.nextItemId, position, level, visible);
        ++map.nextItemId;
        map.systems->entity().spawnItem(
            core::map::Item(static_cast<std::uint16_t>(item), 0, position, level, visible), map);
    }
    else {
        map.itemsField[i].id      = static_cast<std::uint16_t>(item);
        map.itemsField[i].visible = visible;
    }
    return false;
}

bool EditMap::AddOrEditItemAction::undo(EditMap& map) {
    if (add) {
        map.itemsField.pop_back();
        const auto ent = map.systems->position().getEntity({static_cast<std::uint8_t>(level),
                                                            glm::i32vec2(position.x, position.y),
                                                            bl::tmap::Direction::Up});
        if (ent != bl::ecs::InvalidEntity) { map.systems->engine().ecs().destroyEntity(ent); }
    }
    else {
        map.itemsField[i].id      = orig.id;
        map.itemsField[i].visible = orig.visible;
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
    map.itemsField.erase(map.itemsField.begin() + i);
    const auto ent = map.systems->position().getEntity({static_cast<std::uint8_t>(level),
                                                        glm::i32vec2(position.x, position.y),
                                                        bl::tmap::Direction::Up});
    if (ent != bl::ecs::InvalidEntity) { map.systems->engine().ecs().destroyEntity(ent); }
    return false;
}

bool EditMap::RemoveItemAction::undo(EditMap& map) {
    map.itemsField.insert(map.itemsField.begin() + i, orig);
    map.systems->entity().spawnItem(orig, map);
    return false;
}

const char* EditMap::RemoveItemAction::description() const { return "remove item"; }

EditMap::Action::Ptr EditMap::SetLightAction::create(const sf::Vector2i& pos, unsigned int radius,
                                                     const core::map::Light& orig) {
    return Action::Ptr(new SetLightAction(pos, radius, orig));
}

EditMap::SetLightAction::SetLightAction(const sf::Vector2i& pos, unsigned int radius,
                                        const core::map::Light& orig)
: value(radius, pos)
, orig(orig) {}

bool EditMap::SetLightAction::apply(EditMap& map) {
    const core::map::LightingSystem::Handle h = map.lighting.getClosestLight(value.position);
    if (h == core::map::LightingSystem::None) {
        map.lighting.addLight(value);
        spawned = true;
    }
    else { map.lighting.updateLight(h, value); }
    return false;
}

bool EditMap::SetLightAction::undo(EditMap& map) {
    const core::map::LightingSystem::Handle h = map.lighting.getClosestLight(value.position);
    if (spawned) { map.lighting.removeLight(h); }
    else { map.lighting.updateLight(h, orig); }
    return false;
}

const char* EditMap::SetLightAction::description() const {
    return spawned ? "spawn light" : "edit light";
}

EditMap::Action::Ptr EditMap::RemoveLightAction::create(const core::map::Light& orig) {
    return Action::Ptr(new RemoveLightAction(orig));
}

EditMap::RemoveLightAction::RemoveLightAction(const core::map::Light& orig)
: orig(orig) {}

bool EditMap::RemoveLightAction::apply(EditMap& map) {
    const core::map::LightingSystem::Handle h = map.lighting.getClosestLight(orig.position);
    map.lighting.removeLight(h);
    return false;
}

bool EditMap::RemoveLightAction::undo(EditMap& map) {
    map.lighting.addLight(orig);
    return false;
}

const char* EditMap::RemoveLightAction::description() const { return "remove light"; }

EditMap::Action::Ptr EditMap::AddCatchRegionAction::create() {
    return Action::Ptr{new AddCatchRegionAction()};
}

bool EditMap::AddCatchRegionAction::apply(EditMap& map) {
    map.catchRegionsField.emplace_back();
    map.catchRegionsField.back().name = "New Region";
    return true;
}

bool EditMap::AddCatchRegionAction::undo(EditMap& map) {
    map.catchRegionsField.pop_back();
    return true;
}

const char* EditMap::AddCatchRegionAction::description() const { return "add catch region"; }

EditMap::Action::Ptr EditMap::EditCatchRegionAction::create(std::uint8_t index,
                                                            const core::map::CatchRegion& val,
                                                            const core::map::CatchRegion& orig) {
    return Action::Ptr{new EditCatchRegionAction(index, val, orig)};
}

EditMap::EditCatchRegionAction::EditCatchRegionAction(std::uint8_t i,
                                                      const core::map::CatchRegion& v,
                                                      const core::map::CatchRegion& o)
: index(i)
, value(v)
, orig(o) {}

bool EditMap::EditCatchRegionAction::apply(EditMap& map) {
    map.catchRegionsField[index] = value;
    return true;
}

bool EditMap::EditCatchRegionAction::undo(EditMap& map) {
    map.catchRegionsField[index] = orig;
    return true;
}

const char* EditMap::EditCatchRegionAction::description() const { return "edit catch region"; }

EditMap::Action::Ptr EditMap::RemoveCatchRegionAction::create(std::uint8_t index,
                                                              const core::map::CatchRegion& orig) {
    return Action::Ptr{new RemoveCatchRegionAction(index, orig)};
}

EditMap::RemoveCatchRegionAction::RemoveCatchRegionAction(std::uint8_t i,
                                                          const core::map::CatchRegion& o)
: index(i)
, orig(o) {}

bool EditMap::RemoveCatchRegionAction::apply(EditMap& map) {
    const std::uint8_t i = index + 1;

    map.catchRegionsField.erase(map.catchRegionsField.begin() + index);

    if (cleared.empty()) {
        for (unsigned int level = 0; level < map.levels.size(); ++level) {
            for (unsigned int x = 0; x < map.levels[level].catchLayer().width(); ++x) {
                for (unsigned int y = 0; y < map.levels[level].catchLayer().height(); ++y) {
                    if (map.levels[level].catchLayer().get(x, y) == i) {
                        cleared.emplace_back(level, sf::Vector2i(x, y));
                    }
                }
            }
        }
    }

    for (const auto& pos : cleared) {
        map.levels[pos.first].catchLayer().set(pos.second.x, pos.second.y, 0);
    }

    for (unsigned int level = 0; level < map.levels.size(); ++level) {
        for (unsigned int x = 0; x < map.levels[level].catchLayer().width(); ++x) {
            for (unsigned int y = 0; y < map.levels[level].catchLayer().height(); ++y) {
                const std::uint8_t j = map.levels[level].catchLayer().get(x, y);
                if (j > i) { map.levels[level].catchLayer().set(x, y, j - 1); }
            }
        }
    }

    return true;
}

bool EditMap::RemoveCatchRegionAction::undo(EditMap& map) {
    const std::uint8_t i = index + 1;

    map.catchRegionsField.insert(map.catchRegionsField.begin() + index, orig);

    for (unsigned int level = 0; level < map.levels.size(); ++level) {
        for (unsigned int x = 0; x < map.levels[level].catchLayer().width(); ++x) {
            for (unsigned int y = 0; y < map.levels[level].catchLayer().height(); ++y) {
                const std::uint8_t j = map.levels[level].catchLayer().get(x, y);
                if (j >= i) { map.levels[level].catchLayer().set(x, y, j + 1); }
            }
        }
    }

    for (const auto& pos : cleared) {
        map.levels[pos.first].catchLayer().set(pos.second.x, pos.second.y, i);
    }

    return true;
}

const char* EditMap::RemoveCatchRegionAction::description() const { return "remove catch region"; }

EditMap::Action::Ptr EditMap::SetAmbientLightAction::create(
    bool sun, std::uint8_t upper, std::uint8_t lower, const core::map::LightingSystem& lighting) {
    return Ptr(new SetAmbientLightAction(sun, upper, lower, lighting));
}

EditMap::SetAmbientLightAction::SetAmbientLightAction(bool sun, std::uint8_t upper,
                                                      std::uint8_t lower,
                                                      const core::map::LightingSystem& lighting)
: sunlight(sun)
, origSunlight(lighting.adjustsForSunlight())
, origLower(lighting.getMinLightLevel())
, lower(lower)
, origUpper(lighting.getMaxLightLevel())
, upper(upper) {}

bool EditMap::SetAmbientLightAction::apply(EditMap& map) {
    map.lightingSystem().adjustForSunlight(sunlight);
    map.lightingSystem().setAmbientLevel(lower, upper);
    return true;
}

bool EditMap::SetAmbientLightAction::undo(EditMap& map) {
    map.lightingSystem().adjustForSunlight(origSunlight);
    map.lightingSystem().setAmbientLevel(origLower, origUpper);
    return true;
}

const char* EditMap::SetAmbientLightAction::description() const { return "set lighting"; }

EditMap::Action::Ptr EditMap::AddTownAction::create() { return Ptr(new AddTownAction()); }

bool EditMap::AddTownAction::apply(EditMap& map) {
    core::map::Town t;
    t.name    = "New Town";
    t.weather = core::map::Weather::None;
    map.towns.emplace_back(std::move(t));
    return true;
}

bool EditMap::AddTownAction::undo(EditMap& map) {
    map.towns.pop_back();
    return true;
}

const char* EditMap::AddTownAction::description() const { return "add town"; }

EditMap::Action::Ptr EditMap::EditTownAction::create(std::uint8_t i, const core::map::Town& orig,
                                                     const core::map::Town& val) {
    return Ptr(new EditTownAction(i, orig, val));
}

EditMap::EditTownAction::EditTownAction(std::uint8_t i, const core::map::Town& orig,
                                        const core::map::Town& val)
: i(i)
, orig(orig)
, val(val) {}

bool EditMap::EditTownAction::apply(EditMap& map) {
    map.towns[i] = val;
    return val.name != orig.name;
}

bool EditMap::EditTownAction::undo(EditMap& map) {
    map.towns[i] = orig;
    return val.name != orig.name;
}

const char* EditMap::EditTownAction::description() const { return "edit town"; }

EditMap::Action::Ptr EditMap::RemoveTownAction::create(std::uint8_t i,
                                                       const core::map::Town& orig) {
    return Ptr(new RemoveTownAction(i, orig));
}

EditMap::RemoveTownAction::RemoveTownAction(std::uint8_t i, const core::map::Town& t)
: i(i)
, orig(t) {}

bool EditMap::RemoveTownAction::apply(EditMap& map) {
    map.towns.erase(map.towns.begin() + i);
    const bool add       = tiles.empty();
    const std::uint8_t j = i + 1;
    for (int x = 0; x < map.sizeTiles().x; ++x) {
        for (int y = 0; y < map.sizeTiles().y; ++y) {
            if (map.townTiles(x, y) == j && add) {
                tiles.emplace_back(x, y);
                map.townTiles(x, y) = 0;
            }
            else if (map.townTiles(x, y) > j) { map.townTiles(x, y) -= 1; }
        }
    }
    return true;
}

bool EditMap::RemoveTownAction::undo(EditMap& map) {
    map.towns.insert(map.towns.begin() + i, orig);
    const std::uint8_t j = i + 1;

    for (int x = 0; x < map.sizeTiles().x; ++x) {
        for (int y = 0; y < map.sizeTiles().y; ++y) {
            if (map.townTiles(x, y) >= j) { map.townTiles(x, y) += 1; }
        }
    }
    for (const auto& pos : tiles) { map.townTiles(pos.x, pos.y) = j; }

    return true;
}

const char* EditMap::RemoveTownAction::description() const { return "remove town"; }

EditMap::Action::Ptr EditMap::SetTownTileAction::create(const sf::Vector2i& pos, std::uint8_t id,
                                                        std::uint8_t orig) {
    return Ptr(new SetTownTileAction(pos, id, orig));
}

EditMap::SetTownTileAction::SetTownTileAction(const sf::Vector2i& pos, std::uint8_t id,
                                              std::uint8_t orig)
: pos(pos)
, id(id)
, orig(orig) {}

bool EditMap::SetTownTileAction::apply(EditMap& map) {
    map.townTiles(pos.x, pos.y) = id;
    map.updateTownTileColor(pos.x, pos.y);
    return false;
}

bool EditMap::SetTownTileAction::undo(EditMap& map) {
    map.townTiles(pos.x, pos.y) = orig;
    map.updateTownTileColor(pos.x, pos.y);
    return false;
}

const char* EditMap::SetTownTileAction::description() const { return "set town tile"; }

EditMap::Action::Ptr EditMap::SetTownTileAreaAction::create(const sf::IntRect& area,
                                                            std::uint8_t id, EditMap& map) {
    bl::ctr::Vector2D<std::uint8_t> set;
    set.setSize(area.width, area.height, 0);
    for (int x = 0; x < area.width; ++x) {
        for (int y = 0; y < area.height; ++y) {
            set(x, y) = map.townTiles(area.left + x, area.top + y);
        }
    }
    return Ptr(new SetTownTileAreaAction(area, id, std::move(set)));
}

EditMap::SetTownTileAreaAction::SetTownTileAreaAction(const sf::IntRect& area, std::uint8_t id,
                                                      bl::ctr::Vector2D<std::uint8_t>&& set)
: area(area)
, id(id)
, orig(set) {}

bool EditMap::SetTownTileAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.townTiles(x, y) = id;
            map.updateTownTileColor(x, y);
        }
    }
    return false;
}

bool EditMap::SetTownTileAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.townTiles(x, y) = orig(x - area.left, y - area.top);
            map.updateTownTileColor(x, y);
        }
    }
    return false;
}

const char* EditMap::SetTownTileAreaAction::description() const { return "set town selection"; }

EditMap::Action::Ptr EditMap::FillTownTileAction::create(const sf::Vector2i& pos, std::uint8_t fill,
                                                         EditMap& map) {
    const std::uint8_t val     = map.townTiles(pos.x, pos.y);
    const unsigned int estSize = map.sizeTiles().x * map.sizeTiles().y / 8;

    bl::ctr::Vector2D<std::uint8_t> visited;
    visited.setSize(map.sizeTiles().x, map.sizeTiles().y, 0);
    visited(pos.x, pos.y) = 1;

    std::vector<glm::i32vec2> toVisit;
    toVisit.reserve(estSize);
    toVisit.emplace_back(glm::i32vec2(pos.x, pos.y));

    std::vector<std::pair<sf::Vector2i, std::uint8_t>> set;
    set.reserve(estSize);

    while (!toVisit.empty()) {
        const glm::i32vec2 p = toVisit.back();
        toVisit.pop_back();
        set.emplace_back(sf::Vector2i(p.x, p.y), map.townTiles(p.x, p.y));

        glm::i32vec2 nps[4];
        setNeighbors(p, nps);
        for (unsigned int i = 0; i < 4; ++i) {
            if (!map.contains({0, nps[i], bl::tmap::Direction::Up})) continue;

            if (map.townTiles(nps[i].x, nps[i].y) == val && visited(nps[i].x, nps[i].y) == 0) {
                toVisit.emplace_back(nps[i]);
                visited(nps[i].x, nps[i].y) = 1;
            }
        }
    }

    return Ptr(new FillTownTileAction(fill, std::move(set)));
}

EditMap::FillTownTileAction::FillTownTileAction(
    std::uint8_t fill, std::vector<std::pair<sf::Vector2i, std::uint8_t>>&& set)
: set(set)
, id(fill) {}

bool EditMap::FillTownTileAction::apply(EditMap& map) {
    for (const auto& p : set) {
        map.townTiles(p.first.x, p.first.y) = id;
        map.updateTownTileColor(p.first.x, p.first.y);
    }
    return false;
}

bool EditMap::FillTownTileAction::undo(EditMap& map) {
    for (const auto& p : set) {
        map.townTiles(p.first.x, p.first.y) = p.second;
        map.updateTownTileColor(p.first.x, p.first.y);
    }
    return false;
}

const char* EditMap::FillTownTileAction::description() const { return "fill town tiles"; }

EditMap::Action::Ptr EditMap::SetLevelTileAction::create(const sf::Vector2i& pos,
                                                         core::map::LevelTransition lt,
                                                         core::map::LevelTransition orig) {
    return EditMap::Action::Ptr{new SetLevelTileAction(pos, lt, orig)};
}

EditMap::SetLevelTileAction::SetLevelTileAction(const sf::Vector2i& pos,
                                                core::map::LevelTransition lt,
                                                core::map::LevelTransition orig)
: pos(pos)
, lt(lt)
, orig(orig) {}

bool EditMap::SetLevelTileAction::apply(EditMap& map) {
    map.transitionField(pos.x, pos.y) = lt;
    return false;
}

bool EditMap::SetLevelTileAction::undo(EditMap& map) {
    map.transitionField(pos.x, pos.y) = orig;
    return false;
}

const char* EditMap::SetLevelTileAction::description() const { return "set level transition"; }

EditMap::Action::Ptr EditMap::SetLevelTileAreaAction::create(const sf::IntRect& area,
                                                             core::map::LevelTransition lt,
                                                             EditMap& map) {
    bl::ctr::Vector2D<core::map::LevelTransition> orig;
    orig.setSize(area.width, area.height);
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            orig(x - area.left, y - area.top) = map.transitionField(x, y);
        }
    }

    return EditMap::Action::Ptr{new SetLevelTileAreaAction(area, lt, std::move(orig))};
}

EditMap::SetLevelTileAreaAction::SetLevelTileAreaAction(
    const sf::IntRect& area, core::map::LevelTransition lt,
    bl::ctr::Vector2D<core::map::LevelTransition>&& orig)
: area(area)
, lt(lt)
, orig(orig) {}

bool EditMap::SetLevelTileAreaAction::apply(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) { map.transitionField(x, y) = lt; }
    }
    return false;
}

bool EditMap::SetLevelTileAreaAction::undo(EditMap& map) {
    for (int x = area.left; x < area.left + area.width; ++x) {
        for (int y = area.top; y < area.top + area.height; ++y) {
            map.transitionField(x, y) = orig(x - area.left, y - area.top);
        }
    }
    return false;
}

const char* EditMap::SetLevelTileAreaAction::description() const {
    return "set level transition area";
}

} // namespace component
} // namespace editor
