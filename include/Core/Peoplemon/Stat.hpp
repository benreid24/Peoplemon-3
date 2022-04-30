#ifndef CORE_PEOPLEMON_STAT_HPP
#define CORE_PEOPLEMON_STAT_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a single stat. Used as an offset to access Stats as an array
 *
 * @ingroup Peoplemon
 *
 */
enum struct Stat : std::uint8_t {
    HP             = 0,
    Attack         = 1,
    Defense        = 2,
    SpecialAttack  = 3,
    SpecialDefense = 4,
    Speed          = 5,
    Evasion,
    Accuracy,
    Critical
};

} // namespace pplmn
} // namespace core

#endif
