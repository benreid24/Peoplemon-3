#ifndef CORE_PEOPLEMON_PEOPLEMON_HPP
#define CORE_PEOPLEMON_PEOPLEMON_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Core/Peoplemon/Id.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/SpecialAbility.hpp>
#include <Core/Peoplemon/Stats.hpp>
#include <Core/Peoplemon/Type.hpp>

/**
 * @addtogroup Peoplemon
 * @ingroup Core
 * @brief Collection of classes and enums pertaining to Peoplemon themselves
 *
 */

namespace core
{
/// Collection of classes and enums pertaining to Peoplemon themselves
namespace pplmn
{
/**
 * @brief Serves as the primary store of all Peoplemon information
 *
 * @ingroup Peoplemon
 *
 */
class Peoplemon {
public:
    /**
     * @brief Casts an integer to a validated id. Returns Unknown if the id is invalid
     *
     * @param id The id to check
     * @return Id The corresponding Id, or Id::Unknown if invalid
     */
    static Id cast(unsigned int id);

    /**
     * @brief Returns the name of the given Peoplemon
     *
     * @param id The peoplemon to get the name of
     * @return const std::string& The name of the peoplemon
     */
    static const std::string& name(Id id);

    /**
     * @brief Returns the description of the given Peoplemon
     *
     * @param id The peoplemon to get the description of
     * @return const std::string& The description of the peoplemon
     */
    static const std::string& description(Id id);

    /**
     * @brief Returns the type of the Peoplemon
     *
     * @param id The Peoplemon to get the type of
     * @return Type The type of the given Peoplemon
     */
    static Type type(Id id);

    /**
     * @brief Returns the ability of the given peoplemon, if any
     *
     * @param id The peoplemon to get the ability for
     * @return SpecialAbility The ability of the peoplemon
     */
    static SpecialAbility specialAbility(Id id);

    /**
     * @brief Returns the base stats for the given peoplemon
     *
     * @param id The peoplemon to get base stats for
     * @return const Stats& The base stats of the given peoplemon
     */
    static const Stats& baseStats(Id id);

    /**
     * @brief Returns whether or not the given Peoplemon can learn the given move
     *
     * @param ppl The peoplemon trying to learn
     * @param move The move to be learned
     * @return True if the Peoplemon can learn it, false otherwise
     */
    static bool canLearnMove(Id ppl, MoveId move);

    /**
     * @brief Returns the move the peoplemon learns at the given level, if any
     *
     * @param ppl The peoplemon that levled up
     * @param level The level reached
     * @return MoveId The move to learn, if any
     */
    static MoveId moveLearnedAtLevel(Id ppl, unsigned int level);

    /**
     * @brief Returns the minimum level that the given peoplemon will evolve at
     *
     * @param id The peoplemon to check
     * @return unsigned int The level to evolve at. Over 100 means no evolution
     */
    static unsigned int evolveLevel(Id id);

    /**
     * @brief Returns what the given peoplemon will evolve into
     *
     * @param orig The peoplemon that is evolving
     * @return Id The peoplemon that is being evolved into
     */
    static Id evolvesInto(Id orig);

    /**
     * @brief Returns the ev points awarded for defeating the given peoplemon
     *
     * @param id The peoplemon that was defeated
     * @return const Stats& The EVs awarded
     */
    static const Stats& evAward(Id id);

    /**
     * @brief Returns the multiplier used when computing the XP award for defeating the given
     *        peoplemon
     *
     * @param id The peoplemon that was defeated
     * @return float The XP reward multiplier
     */
    static float xpYieldMultiplier(Id id);

    /**
     * @brief Returns the XP required to level up the given peoplemon
     *
     * @param id The peoplemon trying to level up
     * @param level The current level of the peoplemon
     * @return unsigned int The XP required to advance to the next level
     */
    static unsigned int levelUpXp(Id id, unsigned int level);

private:
    std::unordered_map<Id, std::string> names;
    std::unordered_map<Id, std::string> descriptions;
    std::unordered_map<Id, Type> types;
    std::unordered_map<Id, SpecialAbility> abilities;
    std::unordered_map<Id, Stats> baseStats;
    std::unordered_map<Id, std::unordered_set<MoveId>> validMoves;
    std::unordered_map<Id, std::unordered_map<unsigned int, MoveId>> learnedMoves;
    std::unordered_map<Id, unsigned int> evolveLevels;
    std::unordered_map<Id, Id> evolveIds;
    std::unordered_map<Id, Stats> evAwards;
    std::unordered_map<Id, unsigned int> xpGroups;
    std::unordered_map<Id, int> xpMults;
};

} // namespace pplmn
} // namespace core

#endif
