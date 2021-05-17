#ifndef CORE_PLAYER_BAG_HPP
#define CORE_PLAYER_BAG_HPP

#include <Core/Items/Item.hpp>
#include <vector>

namespace core
{
namespace player
{
/**
 * @brief Basic inventory class for managing player items
 *
 */
class Bag {
public:
    /**
     * @brief Simple struct representing a set of items in the bag
     *
     */
    struct Item {
        /// The item in the bag
        item::Id id;

        /// How many of the item are in the bag
        unsigned int qty;

        /**
         * @brief Construct an empty item
         *
         */
        Item();

        /**
         * @brief Construct a new Item object
         *
         * @param id The item id
         * @param qty The number of that item
         */
        Item(item::Id id, unsigned int qty);
    };

    /**
     * @brief Returns the set of owned items in the given category
     *
     * @param category The category to search for
     * @param result Vector to populate with results
     */
    void getByCategory(item::Category category, std::vector<Item>& result) const;

    /**
     * @brief Returns the set of owned items of the given type
     *
     * @param category The type to search for
     * @param result Vector to populate with results
     */
    void getByType(item::Type type, std::vector<Item>& result) const;

    /**
     * @brief Returns all owned items
     *
     * @param result Populated with all owned items
     */
    void getAll(std::vector<Item>& result) const;

    /**
     * @brief Returns the number of the given item owned
     *
     * @param item The item to search for
     * @return unsigned int The amount of that item that is owned
     */
    unsigned int itemCount(item::Id item) const;

    /**
     * @brief Returns true if at least one of the given items is owned
     *
     * @param item The item to search for
     * @return True if owned, false if none
     */
    bool hasItem(item::Id item) const;

    /**
     * @brief Adds the given item to the bag
     *
     * @param item The item to add
     * @param qty How many to add
     */
    void addItem(item::Id item, unsigned int qty = 1);

    /**
     * @brief Removes the given item from the bag
     *
     * @param item The item to remove
     * @param qty How many to remove
     * @return True if the item was removed, false if not enough to remove
     */
    bool removeItem(item::Id item, unsigned int qty = 1);

    /**
     * @brief Removes all items
     *
     */
    void clear();

private:
    std::vector<Item> items;

    unsigned int find(item::Id id) const;
};

} // namespace player
} // namespace core

#endif
