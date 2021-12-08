#ifndef CORE_MAPS_CATCHZONE_HPP
#define CORE_MAPS_CATCHZONE_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief Represents a zone containing information about wild peoplemon that can appear in a Map
 *
 * @ingroup Maps
 *
 */
struct CatchZone {
    sf::IntRect area;
    std::vector<std::string> peoplemon;

    /**
     * @brief Must be called once the map is loaded. This loads the wild peoplemon information
     *
     */
    void activate();

    // TODO - method to generate the people that jumps out
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
struct SerializableObject<core::map::CatchZone> : public SerializableObjectBase {
    SerializableField<1, sf::IntRect, offsetof(core::map::CatchZone, area)> area;
    SerializableField<2, std::vector<std::string>, offsetof(core::map::CatchZone, peoplemon)>
        peoplemon;

    SerializableObject()
    : area(*this)
    , peoplemon(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
