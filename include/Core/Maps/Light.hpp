#ifndef CORE_MAPS_LIGHT_HPP
#define CORE_MAPS_LIGHT_HPP

#include <BLIB/Serialization.hpp>

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
template<>
struct SerializableObject<core::map::Light> : public SerializableObjectBase {
    SerializableField<1, core::map::Light, std::uint16_t> radius;
    SerializableField<2, core::map::Light, sf::Vector2i> position;

    SerializableObject()
    : SerializableObjectBase("Light")
    , radius("radius", *this, &core::map::Light::radius, SerializableFieldBase::Required{})
    , position("position", *this, &core::map::Light::position, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
