#ifndef CORE_PEOPLEMON_STAT_HPP
#define CORE_PEOPLEMON_STAT_HPP

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
enum struct Stat : unsigned int {
    HP             = 0,
    Attack         = 1,
    Defense        = 2,
    SpecialAttack  = 3,
    SpecialDefense = 4,
    Accuracy       = 5,
    Evasion        = 6,
    Speed          = 7,
    Critical       = 8
};

} // namespace pplmn
} // namespace core

#endif
