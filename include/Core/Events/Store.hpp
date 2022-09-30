#ifndef CORE_EVVENTS_STORE_HPP
#define CORE_EVVENTS_STORE_HPP

#include <Core/Items/Id.hpp>
#include <vector>

namespace core
{
namespace event
{
/**
 * @brief Event that is fired by the Core module to signal to the game to open a store
 *
 * @ingroup Events
 *
 */
struct StoreOpened {
    /**
     * @brief Construct a new Store Opened event
     *
     * @param items The items to sell
     * @param sellPrices Optional overrides of the prices a player gets for selling certain items
     */
    StoreOpened(const std::vector<std::pair<item::Id, int>>& items,
                const std::vector<std::pair<item::Id, int>>& sellPrices)
    : items(items)
    , sellPrices(sellPrices) {}

    const std::vector<std::pair<item::Id, int>>& items;
    const std::vector<std::pair<item::Id, int>>& sellPrices;
};

/**
 * @brief Event that is fired when an item is purchased in a store
 *
 * @ingroup Events
 *
 */
struct ItemPurchased {
    /**
     * @brief Construct a new Item Purchased event
     *
     * @param item The item that was purchased
     * @param qty The amount that was purchased
     */
    ItemPurchased(item::Id item, int qty)
    : item(item)
    , qty(qty) {}

    const item::Id item;
    const int qty;
};

/**
 * @brief Event that is fired when a store is closed
 *
 */
struct StoreClosed {};

} // namespace event
} // namespace core

#endif
