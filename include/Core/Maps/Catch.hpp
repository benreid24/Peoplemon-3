#ifndef CORE_MAPS_CATCH_HPP
#define CORE_MAPS_CATCH_HPP

#include <cstdint>

namespace core
{
namespace map
{
/**
 * @brief Represents what happens regarding wild peoplemon when a tile is stepped on
 *
 * @ingroup Maps
 *
 */
enum struct Catch : std::uint8_t { NoEncounter = 0, RandomEncounter = 1 };

} // namespace map
} // namespace core

#endif
