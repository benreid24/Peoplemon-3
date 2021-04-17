#ifndef CORE_MAPS_COLLISION_HPP
#define CORE_MAPS_COLLISION_HPP

#include <cstdint>

namespace core
{
namespace map
{
/**
 * @brief The different types of collisions in maps
 *
 * @ingroup Maps
 *
 */
enum struct Collision : std::uint8_t {
    Blocked           = 0,
    Open              = 1,
    TopOpen           = 2,
    RightOpen         = 3,
    BottomOpen        = 4,
    LeftOpen          = 5,
    TopRightOpen      = 6,
    BottomRightOpen   = 7,
    BottomLeftOpen    = 8,
    TopLeftOpen       = 9,
    TopBottomOpen     = 10,
    LeftRightOpen     = 11,
    TopClosed         = 12,
    RightClosed       = 13,
    BottomClosed      = 14,
    LeftClosed        = 15,
    SurfRequired      = 16,
    WaterfallRequired = 17
};

} // namespace map
} // namespace core

#endif
