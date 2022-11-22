#ifndef CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP
#define CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP

#include <BLIB/Serialization.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Peoplemon/Id.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>
#include <Core/Peoplemon/SpecialAbility.hpp>
#include <Core/Peoplemon/Stats.hpp>
#include <Core/Peoplemon/Type.hpp>
#include <string>

namespace editor
{
namespace component
{
class OwnedPeoplemonWindow;
}
} // namespace editor

namespace core
{
namespace pplmn
{
class OwnedPeoplemon;
} // namespace pplmn
} // namespace core

namespace core
{
namespace pplmn
{
struct WildPeoplemon;
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
     * @brief Returns the id of this peoplemon
     *
     * @return Id The id of this peoplemon
     */
    Id id() const;

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
     * @brief Returns the type of the peoplemon
     *
     */
    Type type() const;

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
     * @brief Returns the xp required to level up
     *
     */
    unsigned int nextLevelXP() const;

    /**
     * @brief Award XP to this peoplemon. A return of 0 indicates that all XP has been awarded. Non
     *        zero indicates that the peoplemon should level up
     *
     * @param xp The XP to award
     * @return The number of unused XP (overflow from a level up)
     */
    unsigned int awardXP(unsigned int xp);

    /**
     * @brief Levels up the peoplemon
     *
     * @return MoveId The move to learn, if any
     */
    MoveId levelUp();

    /**
     * @brief Returns the computed stats for the peoplemon. Does not take into account changes
     *        during battle
     *
     */
    Stats currentStats() const;

    /**
     * @brief Returns the current EVs of this peoplemon
     *
     */
    const Stats& currentEVs() const;

    /**
     * @brief Returns the current IVs of this peoplemon
     *
     */
    const Stats& currentIVs() const;

    /**
     * @brief Access the current HP
     *
     */
    std::uint16_t& currentHp();

    /**
     * @brief Returns the current HP
     *
     */
    std::uint16_t currentHp() const;

    /**
     * @brief Applies damage to the peoplemon. Ensures that the hp does not go negative
     *
     * @param dmg The amount of hp to reduce
     */
    void applyDamage(int dmg);

    /**
     * @brief Restores HP and ensures it does not go over max HP
     *
     * @param hp The amount of hp to restore
     */
    void giveHealth(int hp);

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
     * @brief Access the current ailment of this peoplemon
     *
     */
    Ailment currentAilment() const;

    /**
     * @brief Access the current hold item of this peoplemon
     *
     */
    item::Id& holdItem();

    /**
     * @brief Access the current hold item of this peoplemon
     *
     */
    item::Id holdItem() const;

    /**
     * @brief Returns the special ability of this peoplemon
     *
     * @return SpecialAbility The special ability of this peoplemon
     */
    SpecialAbility ability() const;

    /**
     * @brief Returns the moves known by this Peoplemon
     *
     */
    const OwnedMove* knownMoves() const;

    /**
     * @brief Returns the moves known by this Peoplemon
     *
     */
    OwnedMove* knownMoves();

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

    /**
     * @brief Attempts to learn the given move if there is an open slot
     *
     * @param move The move to learn
     * @return True if learned, false if all moves full
     */
    bool gainMove(MoveId move);

    /**
     * @brief Restores HP and removes ailments
     *
     */
    void heal();

    /**
     * @brief Returns whether or not this peoplemon has an EXP share
     *
     */
    bool hasExpShare() const;

    /**
     * @brief Returns the XP awarded by defeating this peoplemon
     *
     * @param isTrainer True if commanded by a trainer, false otherwise
     * @return unsigned int The XP to award
     */
    unsigned int xpYield(bool isTrainer) const;

    /**
     * @brief Returns the OwnedMove for the given move
     *
     * @param id The move id to find
     * @return OwnedMove* The move itself, or nullptr if not found
     */
    OwnedMove* findMove(MoveId id);

    /**
     * @brief Performs the check that a shake would occur with the given peopleball
     *
     * @param ball The peopleball being used
     * @param turnNumber The current turn of the battle
     * @param opLevel The level of the peoplemon throwing the ball
     * @return True if the ball should shake, false if the peoplemon should break out
     */
    bool shakePasses(item::Id ball, int turnNumber, unsigned int opLevel);

    /**
     * @brief Returns whether or not this peoplemon can be cloned
     *
     */
    bool canClone() const;

private:
    Id _id;
    std::string customName;
    std::uint16_t level;
    std::uint32_t xp;
    std::uint16_t hp;
    Stats ivs;
    Stats evs;
    OwnedMove moves[4];
    Ailment ailment;
    item::Id item;

    friend struct WildPeoplemon;
    friend class BattlePeoplemon;
    friend struct bl::serial::SerializableObject<OwnedPeoplemon>;
    friend class editor::component::OwnedPeoplemonWindow;
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::pplmn::OwnedPeoplemon> : public SerializableObjectBase {
    using OP    = core::pplmn::OwnedPeoplemon;
    using Id    = core::pplmn::Id;
    using Stats = core::pplmn::Stats;
    using Move  = core::pplmn::OwnedMove;
    using Ail   = core::pplmn::Ailment;
    using Item  = core::item::Id;

    SerializableField<1, OP, Id> id;
    SerializableField<2, OP, std::string> customName;
    SerializableField<3, OP, std::uint16_t> level;
    SerializableField<4, OP, std::uint32_t> xp;
    SerializableField<5, OP, std::uint16_t> hp;
    SerializableField<6, OP, Stats> ivs;
    SerializableField<7, OP, Stats> evs;
    SerializableField<8, OP, Move[4]> moves;
    SerializableField<9, OP, Ail> ailment;
    SerializableField<10, OP, Item> item;

    SerializableObject()
    : id("id", *this, &OP::_id, SerializableFieldBase::Required{})
    , customName("name", *this, &OP::customName, SerializableFieldBase::Required{})
    , level("level", *this, &OP::level, SerializableFieldBase::Required{})
    , xp("xp", *this, &OP::xp, SerializableFieldBase::Required{})
    , hp("hp", *this, &OP::hp, SerializableFieldBase::Required{})
    , ivs("ivs", *this, &OP::ivs, SerializableFieldBase::Required{})
    , evs("evs", *this, &OP::evs, SerializableFieldBase::Required{})
    , moves("moves", *this, &OP::moves, SerializableFieldBase::Required{})
    , ailment("ailment", *this, &OP::ailment, SerializableFieldBase::Required{})
    , item("item", *this, &OP::item, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
