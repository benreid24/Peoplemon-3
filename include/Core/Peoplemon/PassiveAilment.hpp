#ifndef CORE_PEOPLEMON_PASSIVEAILMENT_HPP
#define CORE_PEOPLEMON_PASSIVEAILMENT_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a passive ailment a Peoplemon can have
 *
 * @ingroup Peoplemon
 *
 */
enum struct PassiveAilment : std::uint8_t {
    None       = 0,
    Confused   = 0x1 << 0,
    Stolen     = 0x1 << 1,
    Trapped    = 0x1 << 2,
    Distracted = 0x1 << 3
};

} // namespace pplmn
} // namespace core

#endif
