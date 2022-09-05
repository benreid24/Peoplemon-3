#ifndef CORE_MAPS_SPAWN_HPP
#define CORE_MAPS_SPAWN_HPP

#include <BLIB/Serialization.hpp>
#include <Core/Components/Position.hpp>
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
struct Spawn {
    std::uint16_t id;
    component::Position position;

    /**
     * @brief Creates an empty spawn
     *
     */
    Spawn();

    /**
     * @brief Creates a spawn with the given parameters
     *
     * @param id The id of the spawn
     * @param position The position to spawn at
     */
    Spawn(std::uint16_t id, const component::Position& position);
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::Spawn> : public SerializableObjectBase {
    SerializableField<1, core::map::Spawn, std::uint16_t> id;
    SerializableField<2, core::map::Spawn, core::component::Position> position;

    SerializableObject()
    : id("id", *this, &core::map::Spawn::id, SerializableFieldBase::Required{})
    , position("position", *this, &core::map::Spawn::position, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
