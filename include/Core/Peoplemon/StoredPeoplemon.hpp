#ifndef CORE_PEOPLEMON_STOREDPEOPLEMON_HPP
#define CORE_PEOPLEMON_STOREDPEOPLEMON_HPP

#include <BLIB/Serialization.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <SFML/System/Vector2.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a Peoplemon in storage
 *
 * @ingroup Peoplemon
 *
 */
struct StoredPeoplemon {
    /// The peoplemon itself
    OwnedPeoplemon peoplemon;

    /// Which box the peoplemon is in
    std::uint16_t boxNumber;

    /// The position of the peoplemon in the box
    sf::Vector2i position;

    /**
     * @brief Construct a new Stored Peoplemon
     *
     */
    StoredPeoplemon() = default;

    /**
     * @brief Construct a new Stored Peoplemon
     *
     * @param ppl The peoplemon to store
     * @param box The box to store it in
     * @param pos The position to occupy in the box
     */
    StoredPeoplemon(const OwnedPeoplemon& ppl, std::uint16_t box, const sf::Vector2i& pos)
    : peoplemon(ppl)
    , boxNumber(box)
    , position(pos) {}
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::pplmn::StoredPeoplemon> : public SerializableObjectBase {
    using SP = core::pplmn::StoredPeoplemon;
    using OP = core::pplmn::OwnedPeoplemon;

    SerializableField<1, SP, OP> peoplemon;
    SerializableField<2, SP, std::uint16_t> boxNumber;
    SerializableField<3, SP, sf::Vector2i> position;

    SerializableObject()
    : SerializableObjectBase("StoredPeoplemon")
    , peoplemon("peoplemon", *this, &SP::peoplemon, SerializableFieldBase::Required{})
    , boxNumber("boxNumber", *this, &SP::boxNumber, SerializableFieldBase::Required{})
    , position("position", *this, &SP::position, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
