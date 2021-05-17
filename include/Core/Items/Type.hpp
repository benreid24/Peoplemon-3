#ifndef CORE_ITEMS_TYPE_HPP
#define CORE_ITEMS_TYPE_HPP

#include <cstdint>

namespace core
{
namespace item
{
/**
 * @brief The type classification of an item. This is used to determine when an item may be used and
 *        how to use it
 *
 * @ingroup Items
 *
 */
enum struct Type : std::uint16_t {
    /// Invalid type for Unknown items
    Unknown = 0,

    /// The item is used on a peoplemon
    TargetPeoplemon = 1,

    /// The item is used to catch peoplemon
    Peopleball,

    /// The item is used to evolve peoplemon
    EvolveStone,

    /// The item modifiers player state (ie repel)
    PlayerModifier,

    /// Items who's only use is to be sold
    Useless,

    /// The item may be held by Peoplemon
    HoldItem,

    /// The item is a key item
    KeyItem,

    /// The item is a TM
    TM
};

} // namespace item
} // namespace core

#endif
