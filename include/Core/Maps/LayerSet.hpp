#ifndef CORE_MAPS_LAYERSET_HPP
#define CORE_MAPS_LAYERSET_HPP

#include <Core/Maps/Collision.hpp>
#include <Core/Maps/Layer.hpp>

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
using TileLayer = Layer<std::uint16_t>;

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
     * @brief Returns a reference to the top tiles in this set
     *
     * @return std::vector<TileLayer>& The
     */
    std::vector<TileLayer>& topLayers();

private:
    bl::file::binary::SerializableField<1, CollisionLayer> collisions;
    bl::file::binary::SerializableField<2, CatchLayer> catches;
    bl::file::binary::SerializableField<3, std::vector<TileLayer>> bottom;
    bl::file::binary::SerializableField<4, std::vector<TileLayer>> ysort;
    bl::file::binary::SerializableField<5, std::vector<TileLayer>> top;
};

} // namespace map
} // namespace core

#endif
