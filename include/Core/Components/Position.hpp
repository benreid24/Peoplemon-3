#ifndef CORE_ENTITIES_POSITION_HPP
#define CORE_ENTITIES_POSITION_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Components/Direction.hpp>

namespace core
{
namespace entity
{
/**
 * @brief Basic structure representing the position of an entity
 *
 * @ingroup Entity
 *
 */
class Position : public bl::file::binary::SerializableObject {
public:
    /**
     * @brief Creates an empty position
     *
     */
    Position();

    /**
     * @brief Copy constructs the position from another
     *
     * @param copy The position to copy from
     */
    Position(const Position& copy);

    /**
     * @brief Copies the position from another
     *
     * @param copy The position to copy from
     * @return Position& A reference to this position
     */
    Position& operator=(const Position& copy);

    /// The direction the entity is facing
    Direction& direction;

    /// The position of the entity, in tiles
    sf::Vector2i& position;

    /// The current position of the entity if moving between tiles
    sf::Vector2f interpolatedPosition;

    /// The level the entity is on in its map
    std::uint8_t& level;

private:
    bl::file::binary::SerializableField<1, Direction> dir;
    bl::file::binary::SerializableField<2, sf::Vector2i> pos;
    bl::file::binary::SerializableField<3, std::uint8_t> lev;
};

} // namespace entity
} // namespace core

#endif
