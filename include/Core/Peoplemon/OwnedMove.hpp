#ifndef CORE_PEOPLEMON_OWNEDMOVE_HPP
#define CORE_PEOPLEMON_OWNEDMOVE_HPP

#include <BLIB/Serialization.hpp>
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

    /**
     * @brief Restores some pp to the move
     *
     * @param pp The amount to restore
     */
    void restorePP(int pp);
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
using core::pplmn::OwnedMove;

template<>
struct Serializer<OwnedMove> {
    using IdSerializer = Serializer<core::pplmn::MoveId>;

    static bool serialize(OutputStream& output, const OwnedMove& v) {
        if (!IdSerializer::serialize(output, v.id)) return false;
        if (!output.write<std::uint8_t>(v.curPP)) return false;
        if (!output.write<std::uint8_t>(v.maxPP)) return false;
        return true;
    }

    static bool deserialize(InputStream& input, OwnedMove& v) {
        if (!IdSerializer::deserialize(input, v.id)) return false;
        std::uint8_t t;
        if (!input.read<std::uint8_t>(t)) return false;
        v.curPP = t;
        if (!input.read<std::uint8_t>(t)) return false;
        v.maxPP = t;
        return true;
    }

    static std::size_t size(const OwnedMove& v) {
        return IdSerializer::size(v.id) + sizeof(std::uint8_t) * 2;
    }
};

} // namespace binary

template<>
struct SerializableObject<core::pplmn::OwnedMove> : public SerializableObjectBase {
    using Id   = core::pplmn::MoveId;
    using Move = core::pplmn::OwnedMove;

    SerializableField<1, Move, Id> id;
    SerializableField<2, Move, unsigned int> curPP;
    SerializableField<3, Move, unsigned int> maxPP;

    SerializableObject()
    : SerializableObjectBase("OwnedMove")
    , id("id", *this, &Move::id, SerializableFieldBase::Required{})
    , curPP("pp", *this, &Move::curPP, SerializableFieldBase::Required{})
    , maxPP("maxPP", *this, &Move::maxPP, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
