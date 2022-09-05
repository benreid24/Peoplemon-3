#ifndef CORE_MAPS_LAYERSET_HPP
#define CORE_MAPS_LAYERSET_HPP

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
using CatchLayer = Layer<std::uint8_t>;

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
class LayerSet {
public:
    /**
     * @brief Builds a new empty layer set
     *
     */
    LayerSet() = default;

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
     * @brief Returns a reference to the collision layer for this set
     *
     * @return CollisionLayer& The collisions at this height
     */
    const CollisionLayer& collisionLayer() const;

    /**
     * @brief Returns a reference to the catchable layer for this set
     *
     * @return CatchLayer& The catch tiles at this height
     */
    CatchLayer& catchLayer();

    /**
     * @brief Returns a reference to the catchable layer for this set
     *
     * @return CatchLayer& The catch tiles at this height
     */
    const CatchLayer& catchLayer() const;

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
     * @brief Returns a mutable reference to the y-sorted layers. Use with caution
     *
     */
    std::vector<SortedLayer>& renderSortedLayers();

    /**
     * @brief Returns a reference to the top tiles in this set
     *
     * @return std::vector<TileLayer>& The
     */
    std::vector<TileLayer>& topLayers();

    /**
     * @brief Returns a reference to the bottom tiles in this set
     *
     * @return std::vector<TileLayer>& The tiles rendered underneath all entities
     */
    const std::vector<TileLayer>& bottomLayers() const;

    /**
     * @brief Returns a reference to the sorted tiles in this set
     *
     * @return std::vector<TileLayer>& The tiles rendered inline with entities based on y coordinate
     */
    const std::vector<TileLayer>& ysortLayers() const;

    /**
     * @brief Returns a reference to the top tiles in this set
     *
     * @return std::vector<TileLayer>& The
     */
    const std::vector<TileLayer>& topLayers() const;

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
    CollisionLayer collisions;
    CatchLayer catches;
    std::vector<TileLayer> bottom;
    std::vector<TileLayer> ysort;
    std::vector<TileLayer> top;

    std::vector<SortedLayer> ysortedLayers;

    friend class bl::serial::SerializableObject<LayerSet>;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::LayerSet> : public SerializableObjectBase {
    SerializableField<1, core::map::LayerSet, core::map::CollisionLayer> collisions;
    SerializableField<2, core::map::LayerSet, core::map::CatchLayer> catches;
    SerializableField<3, core::map::LayerSet, std::vector<core::map::TileLayer>> bottom;
    SerializableField<4, core::map::LayerSet, std::vector<core::map::TileLayer>> ysort;
    SerializableField<5, core::map::LayerSet, std::vector<core::map::TileLayer>> top;

    SerializableObject()
    : collisions("cols", *this, &core::map::LayerSet::collisions, SerializableFieldBase::Required{})
    , catches("catches", *this, &core::map::LayerSet::catches, SerializableFieldBase::Required{})
    , bottom("bottom", *this, &core::map::LayerSet::bottom, SerializableFieldBase::Required{})
    , ysort("ysort", *this, &core::map::LayerSet::ysort, SerializableFieldBase::Required{})
    , top("top", *this, &core::map::LayerSet::top, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
