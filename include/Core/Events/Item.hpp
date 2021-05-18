#ifndef CORE_EVENTS_ITEM_HPP
#define CORE_EVENTS_ITEM_HPP

#include <Core/Items/Id.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when an item is picked up
 *
 * @ingroup Events
 *
 */
struct ItemPickedUp {
    /// The id of the item that was picked up
    item::Id item;

    /**
     * @brief Construct a new ItemPickedUp event
     *
     * @param item The item that was picked up
     */
    ItemPickedUp(item::Id item)
    : item(item) {}
};

} // namespace event
} // namespace core

#endif
