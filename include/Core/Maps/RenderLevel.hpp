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

    /**
     * @brief Inserts a layer at the given position
     *
     * @param position The index to insert at
     */
    void insertLayer(unsigned int position);

    /**
     * @brief Removes the layer at the given position
     *
     * @param position The index of the layer to remove
     */
    void removeLayer(unsigned int position);

    /**
     * @brief Swaps two render layers
     *
     * @param l1 Index of the first layer to swap
     * @param l2 Index of the second layer to swap
     */
    void swapLayers(unsigned int l1, unsigned int l2);

private:
    bl::engine::Engine* enginePtr;
    bl::rc::res::TextureRef tileset;
    sf::Vector2u mapSize;
    bl::rc::Scene* scene;
    std::list<Zone> storage;
};

} // namespace map
} // namespace core

#endif
