#ifndef CORE_ENTITIES_POSITION_HPP
#define CORE_ENTITIES_POSITION_HPP

#include <BLIB/Entities/Component.hpp>
#include <BLIB/Serialization.hpp>
#include <Core/Components/Direction.hpp>

namespace core
{
namespace component
{
/**
 * @brief Basic structure representing the position of an entity
 *
 * @ingroup Components
 *
 */
class Position {
public:
    /// Required to be used in the BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 1;

    /**
     * @brief Creates an empty position
     *
     */
    Position();

    /**
     * @brief Construct a new Position component
     *
     * @param level The level in the map
     * @param tiles The position in tiles
     * @param direction The direction it is facing
     */
    Position(std::uint8_t level, const sf::Vector2i& tiles, Direction direction);

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

    /**
     * @brief Returns the position adjacent to this one when moving in the given direction.
     *        Does not handle level transition or interpolation. If the direction moved is not the
     *        same as the current direction then no movement occurs and the direction is changed
     *
     * @param dir The direction to move in
     * @return Position The new position after moving or rotating
     */
    Position move(Direction dir) const;

    /**
     * @brief Returns whether or not this position is currently being interpolated
     *
     * @return True if there is active unfinished interpolation, false if still
     */
    bool moving() const;

    /**
     * @brief Returns the direction that an entity at 'from' position should face to face the 'to'
     *        position
     *
     * @param from The position of the entity being rotated
     * @param to The position to face
     * @return Direction The direction to face
     */
    static Direction facePosition(const Position& from, const Position& to);

    /**
     * @brief Tests whether two positions are adjacent to one another and within one level
     *
     * @param left One position to test with
     * @param right The other position to test with
     * @return True if the positions are adjacent, false if not
     */
    static bool adjacent(const Position& left, const Position& right);

    /// The direction the entity is facing
    Direction direction;

    /// The level the entity is on in its map
    std::uint8_t level;

private:
    sf::Vector2i position;
    sf::Vector2f interpolatedPosition;

    friend class bl::serial::SerializableObject<Position>;
};

} // namespace component
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct Serializer<core::component::Position, false> {
    static bool serialize(OutputStream& output, const core::component::Position& position) {
        if (!Serializer<sf::Vector2i>::serialize(output, position.positionTiles())) return false;
        if (!Serializer<core::component::Direction>::serialize(output, position.direction)) {
            return false;
        }
        return Serializer<std::uint8_t>::serialize(output, position.level);
    }

    static bool deserialize(InputStream& input, core::component::Position& result) {
        sf::Vector2i pos;
        if (!Serializer<sf::Vector2i>::deserialize(input, pos)) return false;
        result.setTiles(pos);
        if (!Serializer<core::component::Direction>::deserialize(input, result.direction)) {
            return false;
        }
        return Serializer<std::uint8_t>::deserialize(input, result.level);
    }

    static std::uint32_t size(const core::component::Position& pos) {
        return Serializer<sf::Vector2i>::size(pos.positionTiles()) +
               Serializer<core::component::Direction>::size(pos.direction) +
               Serializer<std::uint8_t>::size(pos.level);
    }
};

} // namespace binary

template<>
struct SerializableObject<core::component::Position> : SerializableObjectBase {
    using Pos = core::component::Position;
    using Dir = core::component::Direction;

    SerializableField<1, Pos, std::uint8_t> level;
    SerializableField<2, Pos, Dir> direction;
    SerializableField<3, Pos, sf::Vector2i> position;
    SerializableField<4, Pos, sf::Vector2f> pixels;

    SerializableObject()
    : level("level", *this, &Pos::level, SerializableFieldBase::Required{})
    , direction("direction", *this, &Pos::direction, SerializableFieldBase::Required{})
    , position("position", *this, &Pos::position, SerializableFieldBase::Required{})
    , pixels("pixels", *this, &Pos::interpolatedPosition, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

inline std::ostream& operator<<(std::ostream& os, const core::component::Position& pos) {
    os << "(" << static_cast<unsigned>(pos.level) << ", [" << pos.positionTiles().x << ", "
       << pos.positionTiles().y << "])";
    return os;
}

#endif
