#ifndef CORE_MAPS_LIGHT_HPP
#define CORE_MAPS_LIGHT_HPP

#include <BLIB/Files/Binary.hpp>

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
struct Light : public bl::file::binary::SerializableObject {
    bl::file::binary::SerializableField<1, std::uint16_t> radius;
    bl::file::binary::SerializableField<2, sf::Vector2i> position;

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

    /**
     * @brief Copy constructs the light
     *
     * @param copy The light to copy
     */
    Light(const Light& copy);

    /**
     * @brief Copies from the given light
     *
     * @param copy The light to copy
     * @return Light& A reference to this light
     */
    Light& operator=(const Light& copy);
};

} // namespace map
} // namespace core

#endif
