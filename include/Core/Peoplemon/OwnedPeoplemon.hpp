#ifndef CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP
#define CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP

#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Peoplemon/Id.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>
#include <Core/Peoplemon/Stats.hpp>
#include <string>

namespace core
{
namespace pplmn
{
class WildPeoplemon;
class BattlePeoplemon;

/**
 * @brief Represents an instance of a peoplemon. Can be a wild peoplemon, trainer, or player
 *        peoplemon. Only contains persistent data specific to the individual peoplemon
 *
 * @ingroup Peoplemon
 *
 */
class OwnedPeoplemon {
public:
    /**
     * @brief Makes an empty peoplemon with id Unknown
     *
     */
    OwnedPeoplemon();

    /**
     * @brief Makes a peoplemon of the given type and level
     *
     * @param id The species of peoplemon to create
     * @param level The level to create at
     */
    OwnedPeoplemon(Id id, unsigned int level);

    /**
     * @brief Returns the name of this peoplemon, custom or defualt
     *
     */
    const std::string& name() const;

    /**
     * @brief Sets a custom name for this peoplemon
     *
     * @param name The name. Pass empty to reset to default
     */
    void setCustomName(const std::string& name);

    /**
     * @brief Returns the current level of this peoplemon
     *
     */
    unsigned int currentLevel() const;

    /**
     * @brief Returns the current XP of this peoplemon
     *
     */
    unsigned int currentXP() const;

    /**
     * @brief Award XP to this peoplemon. Level's up the peoplemon if applicable. Returns the number
     *        of XP left over after a level up. A return of 0 indicates that all XP has been awarded
     *
     * @param xp The XP to award
     * @return The number of unused XP (overflow from a level up)
     */
    unsigned int awardXP(unsigned int xp);

    /**
     * @brief Returns the computed stats for the peoplemon. Does not take into account changes
     *        during battle
     *
     */
    Stats currentStats() const;

    /**
     * @brief Access the current HP
     *
     */
    unsigned int& currentHp();

    /**
     * @brief Award EVs to this peoplemon
     *
     * @param evs The EVs to add
     */
    void awardEVs(const Stats& evs);

    /**
     * @brief Access the current ailment of this peoplemon
     *
     */
    Ailment& currentAilment();

    /**
     * @brief Access the current hold item of this peoplemon
     *
     */
    item::Id& holdItem();

    // TODO - expose moves?

private:
    Id id;
    std::string customName;
    unsigned int level;
    unsigned int xp;
    unsigned int hp;
    Stats ivs;
    Stats evs;
    OwnedMove moves[4];
    Ailment ailment;
    item::Id item;

    friend class WildPeoplemon;
    friend class BattlePeoplemon;
};

} // namespace pplmn
} // namespace core

#endif
