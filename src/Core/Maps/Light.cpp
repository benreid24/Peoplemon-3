#include <Core/Maps/Light.hpp>

namespace core
{
namespace map
{
const sf::Vector3i Light::DefaultColor(240, 240, 150);

Light::Light()
: radius(0)
, color(255, 255, 255) {}

Light::Light(std::uint16_t rad, const sf::Vector2i& pos, const sf::Vector3i& color)
: radius(rad)
, position(pos)
, color(color) {}

} // namespace map
} // namespace core
