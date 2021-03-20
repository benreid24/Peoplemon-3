#include <Core/Maps/Light.hpp>

namespace core
{
namespace map
{
Light::Light()
: radius(*this)
, position(*this) {}

Light::Light(std::uint16_t rad, const sf::Vector2i& pos)
: Light() {
    radius   = rad;
    position = pos;
}

Light::Light(const Light& copy)
: Light() {
    *this = copy;
}

Light& Light::operator=(const Light& copy) {
    radius   = copy.radius;
    position = copy.position;
    return *this;
}

} // namespace map
} // namespace core
