#ifndef CORE_ITEMS_ITEM_HPP
#define CORE_ITEMS_ITEM_HPP

#include <Core/Files/ItemDB.hpp>
#include <Core/Items/Category.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Items/Type.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <vector>

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
     * @brief Set the data source for the item methods
     *
     * @param source The data source. Must remain in scope
     */
    static void setDataSource(file::ItemDB& source);

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
     * @brief Returns the list of valid item ids
     *
     */
    static const std::vector<Id>& validIds();

    /**
     * @brief Returns whether or not the item can be used in battle
     *
     * @param item The item to test
     * @return True if can be used, false if not
     */
    static bool canUseInBattle(Id item);

private:
    static std::unordered_map<Id, std::string>* names;
    static std::unordered_map<Id, std::string>* descriptions;
    static std::unordered_map<Id, int>* values;
};

} // namespace item
} // namespace core

#endif
