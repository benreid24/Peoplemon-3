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
     * @brief Returns a reference to the given tile layer. Helper to use absolute indices to access
     *        all layer zones
     *
     * @param layer The layer index to fetch
     * @return A reference to the layer at the given index
     */
    TileLayer& getLayer(unsigned int layer);

private:
    CollisionLayer collisions;
    CatchLayer catches;
    std::vector<TileLayer> bottom;
    std::vector<TileLayer> ysort;
    std::vector<TileLayer> top;

    friend struct bl::serial::SerializableObject<LayerSet>;
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
    : SerializableObjectBase("LayerSet")
    , collisions("cols", *this, &core::map::LayerSet::collisions, SerializableFieldBase::Required{})
    , catches("catches", *this, &core::map::LayerSet::catches, SerializableFieldBase::Required{})
    , bottom("bottom", *this, &core::map::LayerSet::bottom, SerializableFieldBase::Required{})
    , ysort("ysort", *this, &core::map::LayerSet::ysort, SerializableFieldBase::Required{})
    , top("top", *this, &core::map::LayerSet::top, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
