#ifndef CORE_PEOPLEMON_OWNEDMOVE_HPP
#define CORE_PEOPLEMON_OWNEDMOVE_HPP

#include <BLIB/Files/Binary/Serializer.hpp>
#include <Core/Peoplemon/MoveId.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a move owned by a peoplemon
 *
 * @ingroup Peoplemon
 *
 */
struct OwnedMove {
    /// The id of the move
    MoveId id;

    /// The current pp of the move
    unsigned int curPP;

    /// The pp to restore to. Increased max pp is stored here
    unsigned int maxPP;

    /**
     * @brief Creates an empty Unknown move with 0 pp
     *
     */
    OwnedMove();

    /**
     * @brief Creates the move from the given id and sets PP accordingly
     *
     * @param id The move to set to
     */
    OwnedMove(MoveId id);
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
using core::pplmn::OwnedMove;

template<>
struct Serializer<OwnedMove> {
    using IdSerializer = Serializer<core::pplmn::MoveId>;

    static bool serialize(File& output, const OwnedMove& v) {
        if (!IdSerializer::serialize(output, v.id)) return false;
        if (!output.write<std::uint8_t>(v.curPP)) return false;
        if (!output.write<std::uint8_t>(v.maxPP)) return false;
        return true;
    }

    static bool deserialize(File& input, OwnedMove& v) {
        if (!IdSerializer::deserialize(input, v.id)) return false;
        std::uint8_t t;
        if (!input.read<std::uint8_t>(t)) return false;
        v.curPP = t;
        if (!input.read<std::uint8_t>(t)) return false;
        v.maxPP = t;
        return true;
    }
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
