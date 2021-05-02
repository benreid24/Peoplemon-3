#ifndef CORE_MAPS_CHARACTERSPAWN_HPP
#define CORE_MAPS_CHARACTERSPAWN_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Components/Position.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace map
{
/**
 * @brief Represents a character to be spawned into a map on load
 *
 * @ingroup Maps
 *
 */
struct CharacterSpawn : public bl::file::binary::SerializableObject {
    bl::file::binary::SerializableField<1, component::Position> position;
    bl::file::binary::SerializableField<2, std::string> file;

    /**
     * @brief Creates an empty spawn
     *
     */
    CharacterSpawn();

    /**
     * @brief Creates a spawn from the given parameters
     *
     * @param pos The position to spawn the character at
     * @param file The file to load the character from
     */
    CharacterSpawn(const component::Position& pos, const std::string& file);

    /**
     * @brief Copy constructs from the given copy spawn
     *
     * @param copy The spawn to copy from
     */
    CharacterSpawn(const CharacterSpawn& copy);

    /**
     * @brief Copies from the given spawn
     *
     * @param copy The spawn to copy from
     * @return CharacterSpawn& A reference to this spawn
     */
    CharacterSpawn& operator=(const CharacterSpawn& copy);
};

} // namespace map
} // namespace core

#endif
