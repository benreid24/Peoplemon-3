#ifndef CORE_ENTITIES_POSITION_HPP
#define CORE_ENTITIES_POSITION_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Components/Direction.hpp>

namespace core
{
namespace component
{
/**
 * @brief Basic structure representing the position of an entity
 *
 * @ingroup Entity
 *
 */
class Position {
public:
    /**
     * @brief Creates an empty position
     *
     */
    Position();

    /**
     * @brief Set the position in tiles. This also sets the interpolated position to be the top
     *        right corner of the given tile position
     *
     * @param posTiles The position, in tiles, to set
     */
    void setTiles(const sf::Vector2i& posTiles);

    /**
     * @brief Set the interpolated position in pixels. This is used for rendering and moving between
     *        tiles. Note that the tiles position is never changed by this method, so setTiles()
     *        must be called when interpolation is complete
     *
     * @param interpolated The interpolated position, in pixels
     */
    void setPixels(const sf::Vector2f& interpolated);

    /**
     * @brief Returns the current position in tiles
     *
     */
    const sf::Vector2i& positionTiles() const;

    /**
     * @brief Returns the current position in pixels
     *
     */
    const sf::Vector2f& positionPixels() const;

    /// The direction the entity is facing
    Direction direction;

    /// The level the entity is on in its map
    std::uint8_t level;

private:
    sf::Vector2i position;
    sf::Vector2f interpolatedPosition;
};

} // namespace component
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
template<>
struct Serializer<core::component::Position, false> {
    static bool serialize(File& output, const core::component::Position& position) {
        if (!Serializer<sf::Vector2i>::serialize(output, position.positionTiles())) return false;
        if (!Serializer<core::component::Direction>::serialize(output, position.direction))
            return false;
        return Serializer<std::uint8_t>::serialize(output, position.level);
    }

    static bool deserialize(File& input, core::component::Position& result) {
        sf::Vector2i pos;
        if (!Serializer<sf::Vector2i>::deserialize(input, pos)) return false;
        result.setTiles(pos);
        if (!Serializer<core::component::Direction>::deserialize(input, result.direction))
            return false;
        return Serializer<std::uint8_t>::deserialize(input, result.level);
    }

    static std::uint32_t size(const core::component::Position& pos) {
        return Serializer<sf::Vector2i>::size(pos.positionTiles()) +
               Serializer<core::component::Direction>::size(pos.direction) +
               Serializer<std::uint8_t>::size(pos.level);
    }
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
