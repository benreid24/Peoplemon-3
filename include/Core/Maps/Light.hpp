#ifndef CORE_MAPS_LIGHT_HPP
#define CORE_MAPS_LIGHT_HPP

#include <BLIB/Serialization/Binary.hpp>

namespace core
{
namespace map
{
/**
 * @brief Represents a renderable light in a Map
 *
 * @ingroup Maps
 *
 */
struct Light {
    std::uint16_t radius;
    sf::Vector2i position;

    /**
     * @brief Creates an empty light
     *
     */
    Light();

    /**
     * @brief Creates a light from the given parameters
     *
     * @param radius The radius of the light, in pixels
     * @param position The position of the light, in pixels
     */
    Light(std::uint16_t radius, const sf::Vector2i& position);
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
struct SerializableObject<core::map::Light> : public SerializableObjectBase {
    SerializableField<1, std::uint16_t, offsetof(core::map::Light, radius)> radius;
    SerializableField<2, sf::Vector2i, offsetof(core::map::Light, position)> position;

    SerializableObject()
    : radius(*this)
    , position(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
