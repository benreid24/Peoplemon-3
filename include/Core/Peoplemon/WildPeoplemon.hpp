#ifndef CORE_PEOPLEMON_WILDPEOPLEMON_HPP
#define CORE_PEOPLEMON_WILDPEOPLEMON_HPP

#include <BLIB/Serialization.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Template struct to generate OwnedPeoplemon for random encounters
 *
 * @ingroup Peoplemon
 *
 */
struct WildPeoplemon {
    /// The type of peoplemon to generate
    Id id;

    /// The minimum level that may spawn, inclusive
    std::uint8_t minLevel;

    /// The maximum level that may spawn, inclusive
    std::uint8_t maxLevel;

    /// Frequency of spawns relative to other wild peoplemon in the region
    std::uint16_t frequency;

    /**
     * @brief Generate an OwnedPeoplemon from the template
     *
     */
    OwnedPeoplemon generate() const;
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::pplmn::WildPeoplemon> : public SerializableObjectBase {
    using Wild = core::pplmn::WildPeoplemon;

    SerializableField<1, Wild, std::uint8_t> minLevel;
    SerializableField<2, Wild, std::uint8_t> maxLevel;
    SerializableField<3, Wild, std::uint16_t> freq;
    SerializableField<4, Wild, core::pplmn::Id> id;

    SerializableObject()
    : minLevel("minLevel", *this, &Wild::minLevel, SerializableFieldBase::Required{})
    , maxLevel("maxLevel", *this, &Wild::maxLevel, SerializableFieldBase::Required{})
    , freq("freq", *this, &Wild::frequency, SerializableFieldBase::Required{})
    , id("id", *this, &Wild::id, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
