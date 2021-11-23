#ifndef CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP
#define CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP

#include <BLIB/Files/Binary.hpp>
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
class OwnedPeoplemon;
} // namespace pplmn
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
template<>
struct Serializer<core::pplmn::OwnedPeoplemon> {
    static bool serialize(File& output, const core::pplmn::OwnedPeoplemon& p);

    static bool deserialize(File& input, core::pplmn::OwnedPeoplemon& p);
};

} // namespace binary
} // namespace file
} // namespace bl

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
     * @brief Loads the peoplemon from a legacy file
     *
     * @param file Path to the file, relative to the peoplemon directory
     * @return True on success, false on error
     */
    bool loadLegacyFile(const std::string& file);

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

    /**
     * @brief Returns the moves known by this Peoplemon
     *
     */
    const OwnedMove* knownMoves() const;

    /**
     * @brief Returns whether or not this peoplemon knows the given move
     *
     * @param move The move to check for
     * @return True if known, false if unknown
     */
    bool knowsMove(MoveId move) const;

    /**
     * @brief Teaches the peoplemon the given move, potentially replacing an existing one
     *
     * @param move The move to learn
     * @param i The index to place it, in range [0, 4)
     */
    void learnMove(MoveId move, unsigned int i);

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
    friend class bl::file::binary::Serializer<OwnedPeoplemon>;
};

} // namespace pplmn
} // namespace core

#endif
