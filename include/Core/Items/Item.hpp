#ifndef CORE_ITEMS_ITEM_HPP
#define CORE_ITEMS_ITEM_HPP

#include <Core/Items/Category.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Items/Type.hpp>
#include <Core/Items/UseResult.hpp>

namespace core
{
namespace item
{
/**
 * @brief Collection of static methods centered around items
 *
 * @ingroup Items
 *
 */
struct Item {
    /**
     * @brief Helper function to cast a raw id to an item Id
     *
     * @param id The raw item id
     * @return Id The resulting id, or Unknown if unrecognized
     */
    static Id cast(unsigned int id);

    /**
     * @brief Returns the category of the given item
     *
     * @param item The item to get the category of
     * @return Category The category of the given item
     */
    static Category getCategory(Id item);

    /**
     * @brief Returns the type of the given item
     *
     * @param item The item to get the type of
     * @return Type The type of the given item
     */
    static Type getType(Id item);

    /**
     * @brief Returns the name of the given item
     *
     * @param item The item to get the name for
     * @return const std::string& The name of the item
     */
    static const std::string& getName(item::Id item);

    /**
     * @brief Returns the description of the given item
     *
     * @param item The item to get the description for
     * @return const std::string& The description of the item
     */
    static const std::string& getDescription(item::Id item);

    /**
     * @brief Returns the value of the given item
     *
     * @param item The item to get the value for
     * @return int The value of the item
     */
    static int getValue(item::Id item);

    /**
     * @brief Uses the given item on the given peoplemon
     *
     * @param item The item to use
     * @return UseResult The result of the use
     */
    static UseResult useOnPeoplemon(Id item); // TODO - peoplemon data

    /**
     * @brief Uses the given item to evolve the given peoplemon
     *
     * @param item The item to use
     * @return UseResult The result of the use
     */
    static UseResult evolvePeoplemon(Id item); // TODO - peoplemon data

    /**
     * @brief Uses the given item on the player
     *
     * @param item The item to use
     * @return UseResult The result of the use
     */
    static UseResult useOnPlayer(Id item); // TODO - player data

    /**
     * @brief Uses the given key item. Most are not usable
     *
     * @param item The item to use
     * @return UseResult The result of the use
     */
    static UseResult useKeyItem(Id item); // TODO - player data
};

} // namespace item
} // namespace core

#endif
