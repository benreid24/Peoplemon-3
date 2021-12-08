#include <Core/Maps/Light.hpp>

namespace core
{
namespace map
{
Light::Light()
: radius(0) {}

Light::Light(std::uint16_t rad, const sf::Vector2i& pos)
: radius(rad)
, position(pos) {}

} // namespace map
} // namespace core
