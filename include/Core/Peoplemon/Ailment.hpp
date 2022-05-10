#ifndef CORE_PEOPLEMON_AILMENT_HPP
#define CORE_PEOPLEMON_AILMENT_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents an ailment that a Peoplemon can have
 *
 * @ingroup Peoplemon
 *
 */
enum struct Ailment : std::uint8_t {
    None       = 0,
    Annoyed    = 2,
    Frustrated = 3,
    Sticky     = 5,
    Sleep      = 6,
    Frozen     = 7
};

} // namespace pplmn
} // namespace core

#endif
