#ifndef CORE_MAPS_TOWN_HPP
#define CORE_MAPS_TOWN_HPP

#include <BLIB/Serialization.hpp>
#include <Core/Maps/Weather.hpp>
#include <Core/Peoplemon/WildPeoplemon.hpp>
#include <string>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief Represents a town, route, or region within a map. Maps may have many towns. Individual
 *        tiles are associated to a town. If a tile is not associated with a town then a default
 *        town is used with the name, music, and weather of the map itself
 *
 * @ingroup Maps
 *
 */
struct Town {
    std::string name;
    std::string playlist;
    Weather::Type weather;
    std::uint16_t pcSpawn;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::Town> : public SerializableObjectBase {
    using Town    = core::map::Town;
    using Weather = core::map::Weather;
    using Wild    = core::pplmn::WildPeoplemon;

    SerializableField<1, Town, std::string> name;
    SerializableField<2, Town, std::string> playlist;
    SerializableField<3, Town, Weather::Type> weather;
    SerializableField<4, Town, std::uint16_t> pcSpawn;

    SerializableObject()
    : name("name", *this, &Town::name, SerializableFieldBase::Required{})
    , playlist("playlist", *this, &Town::playlist, SerializableFieldBase::Required{})
    , weather("weather", *this, &Town::weather, SerializableFieldBase::Required{})
    , pcSpawn("pcSpawn", *this, &Town::pcSpawn, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
