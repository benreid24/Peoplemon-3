#ifndef CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP
#define CORE_PEOPLEMON_OWNEDPEOPLEMON_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Peoplemon/Id.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>
#include <Core/Peoplemon/Stats.hpp>
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

    friend class WildPeoplemon;
    friend class BattlePeoplemon;
    friend class bl::serial::binary::SerializableObject<OwnedPeoplemon>;
    friend class editor::component::OwnedPeoplemonWindow;
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
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
    : id(*this, &OP::_id)
    , customName(*this, &OP::customName)
    , level(*this, &OP::level)
    , xp(*this, &OP::xp)
    , hp(*this, &OP::hp)
    , ivs(*this, &OP::ivs)
    , evs(*this, &OP::evs)
    , moves(*this, &OP::moves)
    , ailment(*this, &OP::ailment)
    , item(*this, &OP::item) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
