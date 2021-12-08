#ifndef CORE_MAPS_CHARACTERSPAWN_HPP
#define CORE_MAPS_CHARACTERSPAWN_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <Core/Components/Position.hpp>
#include <SFML/Graphics.hpp>
#include <string>

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
struct CharacterSpawn {
    component::Position position;
    std::string file;

    /**
     * @brief Construct a new Character Spawn object
     *
     */
    CharacterSpawn() = default;

    /**
     * @brief Construct a new Character Spawn object
     *
     * @param pos The position to spawn at
     * @param file The file to load from
     */
    CharacterSpawn(const component::Position& pos, const std::string& file)
    : position(pos)
    , file(file) {}
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct SerializableObject<core::map::CharacterSpawn> : public SerializableObjectBase {
    SerializableField<1, core::component::Position, offsetof(core::map::CharacterSpawn, position)>
        position;
    SerializableField<2, std::string, offsetof(core::map::CharacterSpawn, file)> file;

    SerializableObject()
    : position(*this)
    , file(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
