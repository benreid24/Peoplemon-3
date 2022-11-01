#ifndef CORE_CAMERAS_UTIL_HPP
#define CORE_CAMERAS_UTIL_HPP

#include <BLIB/Entities/Entity.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace system
{
class Systems;
}

namespace camera
{
sf::Vector2f* getPositionPointer(system::Systems& s, bl::entity::Entity e);
}
} // namespace core

#endif
