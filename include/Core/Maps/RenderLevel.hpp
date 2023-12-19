#ifndef CORE_MAPS_RENDERLEVEL_HPP
#define CORE_MAPS_RENDERLEVEL_HPP

#include <BLIB/Graphics.hpp>
#include <Core/Maps/LayerSet.hpp>

namespace core
{
namespace map
{
/**
 * @brief Basic struct containing the graphics elements to render a map level
 *
 * @ingroup Maps
 */
struct RenderLevel {
    /// Indices for the different layer zones
    enum ZoneType { Bottom = 0, Ysort = 1, Top = 2 };

    /**
     * @brief Batched graphics primitives for a set of layers
     */
    struct Zone {
        bl::gfx::BatchedSprites tileSprites;
        bl::gfx::BatchedSlideshows tileAnims;
    };

    /// The graphics primitives for all the layers in the level
    std::array<Zone, 3> zones;

    /**
     * @brief Creates the graphics primitives and adds them to the given scene
     *
     * @param engine Game engine instance
     * @param tileset Stitched texture atlas for all tiles
     * @param layerCount The total number of layers in the level
     * @param mapSize The size of the map in tiles
     * @param scene The scene to add primitives to
     */
    void create(bl::engine::Engine& engine, bl::rc::res::TextureRef tileset,
                unsigned int layerCount, const sf::Vector2u& mapSize, bl::rc::Scene* scene);

    /**
     * @brief Returns the correct zone for the given layer in the given level
     *
     * @param level The level the layer belongs to
     * @param layer The index of the layer to get the zone for
     * @return The zone for the given layer
     */
    Zone& getZone(const LayerSet& level, unsigned int layer);
};

} // namespace map
} // namespace core

#endif
