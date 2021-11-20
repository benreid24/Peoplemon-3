#ifndef CORE_PEOPLEMON_ID_HPP
#define CORE_PEOPLEMON_ID_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief The id of a peoplemon
 *
 * @ingroup Peoplemon
 *
 */
enum struct Id : std::uint8_t {
    Unknown = 0,
    // TODO - other peoplemon ids
};

} // namespace pplmn
} // namespace core

#endif
