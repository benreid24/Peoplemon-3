#ifndef CORE_MAPS_CHARACTERSPAWN_HPP
#define CORE_MAPS_CHARACTERSPAWN_HPP

#include <BLIB/Serialization.hpp>
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
template<>
struct SerializableObject<core::map::CharacterSpawn> : public SerializableObjectBase {
    SerializableField<1, core::map::CharacterSpawn, core::component::Position> position;
    SerializableField<2, core::map::CharacterSpawn, std::string> file;

    SerializableObject()
    : position("position", *this, &core::map::CharacterSpawn::position,
               SerializableFieldBase::Required{})
    , file("file", *this, &core::map::CharacterSpawn::file, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
