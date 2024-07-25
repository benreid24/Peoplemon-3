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
    /**
     * @brief Batched graphics primitives for a set of layers
     */
    struct Zone {
        bl::gfx::BatchedSprites tileSprites;
        bl::gfx::BatchedSlideshows tileAnims;
    };

    /// The graphics primitives for all the layers in the level
    std::vector<Zone*> zones;

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

private:
    std::list<Zone> storage;
};

} // namespace map
} // namespace core

#endif
