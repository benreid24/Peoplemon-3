#ifndef CORE_ITEMS_ITEM_HPP
#define CORE_ITEMS_ITEM_HPP

#include <Core/Battles/Battler.hpp>
#include <Core/Files/ItemDB.hpp>
#include <Core/Items/Category.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Items/Type.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Player/State.hpp>
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

    /**
     * @brief Returns whether or not the given item will affect the peoplemon
     *
     * @param item The item to use
     * @param ppl The peoplemon to check
     * @return True if the item can be used, false if not
     */
    static bool hasEffectOnPeoplemon(Id item, const pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Returns whether or not the given item will affect the peoplemon in battle
     *
     * @param item The item to use
     * @param ppl The peoplemon to check
     * @param battler The battler using the item
     * @return True if the item can be used, false if not
     */
    static bool hasEffectOnPeoplemon(Id item, const pplmn::BattlePeoplemon& ppl,
                                     const battle::Battler& battler);

    /**
     * @brief Applies the given item to the peoplemon
     *
     * @param item The itemto use
     * @param ppl The peoplemon to use it on
     * @param team The peoplemon's team. Optional
     * @param battleTeam The peoplemon's team when in battle. Optional
     */
    static void useOnPeoplemon(Id item, pplmn::OwnedPeoplemon& ppl,
                               std::vector<pplmn::OwnedPeoplemon>* team        = nullptr,
                               std::vector<pplmn::BattlePeoplemon>* battleTeam = nullptr);

    /**
     * @brief Applies the given item to the peoplemon
     *
     * @param item The itemto use
     * @param ppl The peoplemon to use it on
     * @param battler The battler using the item
     */
    static void useOnPeoplemon(Id item, pplmn::BattlePeoplemon& ppl, battle::Battler& battler);

    /**
     * @brief Returns the text to display when the item is used on the given peoplemon
     *
     * @param item The item that was used
     * @param ppl The peoplemon that it was used on
     * @return std::string The text to display
     */
    static std::string getUseLine(Id item, const pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Returns whether or not the given item will affect the player
     *
     * @param item The item to use
     * @param state The player's state
     * @return True if the item may be used, false if not
     */
    static bool hasEffectOnPlayer(Id item, const player::State& state);

    /**
     * @brief Uses the given item on the player
     *
     * @param item The item to use
     * @param state The player's state
     */
    static void useOnPlayer(Id item, player::State& state);

    /**
     * @brief Generates the text to print when the item is used on the player
     *
     * @param item The item being used
     * @return std::string The text to display
     */
    static std::string getUseLine(Id item);

    /**
     * @brief Returns the move taught by the TM
     *
     * @param tm The TM to get the move for
     * @return pplmn::MoveId The move to teach, or Unknown
     */
    static pplmn::MoveId getTmMove(Id tm);

    /**
     * @brief Returns the catch rate for the given peopleball
     *
     * @param ball The peopleball to get the rate for
     * @param ppl The peoplemon the ball is being used on
     * @param turnNumber The current turn of the battle
     * @param levelRatio Result of throwerLevel / wildLevel
     * @return float The catch rate, or 1 if not a peopleball
     */
    static float getPeopleballRate(Id ball, pplmn::Id ppl, int turnNumber, float levelRatio);

private:
    static std::unordered_map<Id, std::string>* names;
    static std::unordered_map<Id, std::string>* descriptions;
    static std::unordered_map<Id, int>* values;
};

} // namespace item
} // namespace core

#endif
