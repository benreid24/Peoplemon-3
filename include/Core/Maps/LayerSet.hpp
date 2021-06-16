#ifndef CORE_MAPS_LAYERSET_HPP
#define CORE_MAPS_LAYERSET_HPP

#include <Core/Maps/Catch.hpp>
#include <Core/Maps/Collision.hpp>
#include <Core/Maps/Layer.hpp>
#include <Core/Maps/Tile.hpp>

namespace core
{
namespace map
{
/**
 * @brief Specialization of Layer for collisions
 *
 * @ingroup Maps
 *
 */
using CollisionLayer = Layer<Collision>;

/**
 * @brief Specialization of Layer for catch tiles
 *
 * @ingroup Maps
 *
 */
using CatchLayer = Layer<Catch>;

/**
 * @brief Specialization of Layer for tiles
 *
 * @ingroup Maps
 *
 */
using TileLayer = Layer<Tile>;

/**
 * @brief Special type of tile layer for y-sorted tiles. Used for rendering only
 *
 * @ingroup Maps
 *
 */
using SortedLayer = bl::container::Vector2D<Tile*>;

/**
 * @brief Specialization of Layer for height transitions
 *
 * @ingroup Maps
 *
 */
using HeightTransitionLayer = Layer<std::uint8_t>;

/**
 * @brief Encapsulates a set of layers for a given map height. Maps have one LayerSet per height
 *        level, as well as a TransitionLayer
 *
 * @ingroup Maps
 *
 */
class LayerSet : public bl::file::binary::SerializableObject {
public:
    /**
     * @brief Builds a new empty layer set
     *
     */
    LayerSet();

    /**
     * @brief Copy constructs from the given layer set
     *
     * @param copy The set to copy
     */
    LayerSet(const LayerSet& copy);

    /**
     * @brief Copies the internals from the given layer set
     *
     * @param copy The set to copy
     * @return LayerSet& A reference to this set
     */
    LayerSet& operator=(const LayerSet& copy);

    /**
     * @brief Creates the given number of layers and sets the proper size for each layer,
     *        initializing each layer tile to empty
     *
     * @param width The width of the map, in tiles
     * @param height The height of the map, in tiles
     * @param bottomLayerCount The number of layers rendered under entities
     * @param ysortLayerCount The number of layers rendered with entities
     * @param topLayercount The number of layers renered over entities
     */
    void init(unsigned int width, unsigned int height, unsigned int bottomLayerCount,
              unsigned int ysortLayerCount, unsigned int topLayercount);

    /**
     * @brief Initializes all the tiles for rendering. This must be called before rendering is done
     *
     * @param tileset The tileset to use
     */
    void activate(Tileset& tileset);

    /**
     * @brief Returns a reference to the collision layer for this set
     *
     * @return CollisionLayer& The collisions at this height
     */
    CollisionLayer& collisionLayer();

    /**
     * @brief Returns a reference to the catchable layer for this set
     *
     * @return CatchLayer& The catch tiles at this height
     */
    CatchLayer& catchLayer();

    /**
     * @brief Returns a reference to the bottom tiles in this set
     *
     * @return std::vector<TileLayer>& The tiles rendered underneath all entities
     */
    std::vector<TileLayer>& bottomLayers();

    /**
     * @brief Returns a reference to the sorted tiles in this set
     *
     * @return std::vector<TileLayer>& The tiles rendered inline with entities based on y coordinate
     */
    std::vector<TileLayer>& ysortLayers();

    /**
     * @brief Returns an immutable reference to the y-sorted layers
     *
     */
    const std::vector<SortedLayer>& renderSortedLayers() const;

    /**
     * @brief Returns a reference to the top tiles in this set
     *
     * @return std::vector<TileLayer>& The
     */
    std::vector<TileLayer>& topLayers();

    /**
     * @brief Returns the total number of layers contained
     *
     */
    unsigned int layerCount() const;

    /**
     * @brief Updates tiles in the layer set
     *
     * @param area The area of the map to update
     * @param dt Elapsed time in seconds since last call to update()
     */
    void update(const sf::IntRect& area, float dt);

    /**
     * @brief Returns a pointer to the pointer to sorted tile
     *
     * @param tileset The tileset to use for determining size
     * @param layer Which sorted layer the tile is in
     * @param x The x position of the tile
     * @param y The y position of the tile
     */
    Tile** getSortedTile(Tileset& tileset, unsigned int layer, unsigned int x, unsigned int y);

private:
    bl::file::binary::SerializableField<1, CollisionLayer> collisions;
    bl::file::binary::SerializableField<2, CatchLayer> catches;
    bl::file::binary::SerializableField<3, std::vector<TileLayer>> bottom;
    bl::file::binary::SerializableField<4, std::vector<TileLayer>> ysort;
    bl::file::binary::SerializableField<5, std::vector<TileLayer>> top;

    std::vector<SortedLayer> ysortedLayers;
};

} // namespace map
} // namespace core

#endif
