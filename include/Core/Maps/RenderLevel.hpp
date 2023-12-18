#ifndef CORE_MAPS_RENDERLEVEL_HPP
#define CORE_MAPS_RENDERLEVEL_HPP

#include <BLIB/Graphics.hpp>
#include <Core/Maps/LayerSet.hpp>

namespace core
{
namespace map
{
struct RenderLevel {
    enum ZoneType { Bottom = 0, Ysort = 1, Top = 2 };

    struct Zone {
        bl::gfx::BatchedSprites tileSprites;
        bl::gfx::BatchedSlideshows tileAnims;
    };

    std::array<Zone, 3> zones;

    void create(bl::engine::Engine& engine, const sf::Vector2u& mapSize);
};

} // namespace map
} // namespace core

#endif
