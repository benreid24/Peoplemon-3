#ifndef CORE_PEOPLEMON_WILDPEOPLEMON_HPP
#define CORE_PEOPLEMON_WILDPEOPLEMON_HPP

#include <BLIB/Serialization/Binary.hpp>
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
namespace binary
{
template<>
struct SerializableObject<core::pplmn::WildPeoplemon> : public SerializableObjectBase {
    using Wild = core::pplmn::WildPeoplemon;

    SerializableField<1, Wild, std::uint8_t> minLevel;
    SerializableField<2, Wild, std::uint8_t> maxLevel;
    SerializableField<3, Wild, std::uint16_t> freq;

    SerializableObject()
    : minLevel(*this, &Wild::minLevel)
    , maxLevel(*this, &Wild::maxLevel)
    , freq(*this, &Wild::frequency) {}
};
} // namespace binary
} // namespace serial
} // namespace bl

#endif
