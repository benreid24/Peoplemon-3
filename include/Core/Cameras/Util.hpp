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
/**
 * @brief Helper function to get a pointer to the pixel position of the given entity
 *
 * @param s The main game systems
 * @param e The entity to get the position pointer to
 * @return const sf::Vector2f* Pointer to the pixel position, nullptr if invalid
 */
const sf::Vector2f* getPositionPointer(system::Systems& s, bl::entity::Entity e);

} // namespace camera
} // namespace core

#endif
