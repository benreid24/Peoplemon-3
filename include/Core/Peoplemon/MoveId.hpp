#ifndef CORE_PEOPLEMON_MOVEID_HPP
#define CORE_PEOPLEMON_MOVEID_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief The id of a move
 *
 * @ingroup Peoplemon
 *
 */
enum struct MoveId : std::uint8_t {
    Unknown = 0,
    // TODO - other move ids
};

} // namespace pplmn
} // namespace core

#endif
