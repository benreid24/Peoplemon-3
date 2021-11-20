#ifndef CORE_PEOPLEMON_WILDPEOPLEMON_HPP
#define CORE_PEOPLEMON_WILDPEOPLEMON_HPP

#include <Core/Peoplemon/OwnedPeoplemon.hpp>

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
    unsigned int minLevel;

    /// The maximum level that may spawn, inclusive
    unsigned int maxLevel;

    /// Frequency of spawns relative to other wild peoplemon in the region
    unsigned int frequency;

    // TODO - how to do overrides?

    /**
     * @brief Generate an OwnedPeoplemon from the template
     * 
     */
    OwnedPeoplemon generate() const;
};

} // namespace pplmn
} // namespace core

#endif
