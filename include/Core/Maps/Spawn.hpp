#ifndef CORE_MAPS_SPAWN_HPP
#define CORE_MAPS_SPAWN_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Entities/Direction.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>

namespace core
{
namespace map
{
/**
 * @brief Basic struct representing a spawn in a Map
 *
 * @ingroup Maps
 *
 */
struct Spawn : public bl::file::binary::SerializableObject {
    bl::file::binary::SerializableField<1, std::uint16_t> id;
    bl::file::binary::SerializableField<2, sf::Vector2i> position;
    bl::file::binary::SerializableField<1, entity::Direction> direction;

    /**
     * @brief Creates an empty spawn
     *
     */
    Spawn();

    /**
     * @brief Creates a spawn with the given parameters
     *
     * @param id The id of the spawn
     * @param pos The position to spawn at, in tiles
     * @param dir The direction to spawn facing
     */
    Spawn(std::uint16_t id, const sf::Vector2i& pos, entity::Direction dir);

    /**
     * @brief Copy constructs a new Spawn
     *
     * @param copy The spawn to copy from
     */
    Spawn(const Spawn& copy);

    /**
     * @brief Copies the given spawn
     *
     * @param copy The spawn to copy from
     * @return Spawn& A reference to this spawn
     */
    Spawn& operator=(const Spawn& copy);
};

} // namespace map
} // namespace core

#endif
