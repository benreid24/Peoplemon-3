#ifndef CORE_FILES_PEOPLEMONDB_HPP
#define CORE_FILES_PEOPLEMONDB_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <unordered_map>
#include <unordered_set>

#include <Core/Peoplemon/Id.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/SpecialAbility.hpp>
#include <Core/Peoplemon/Stats.hpp>
#include <Core/Peoplemon/Type.hpp>

namespace core
{
namespace file
{
/**
 * @brief Data structure that holds the underlaying data for all the peoplemon
 *
 * @ingroup Files
 *
 */
struct PeoplemonDB : private bl::util::NonCopyable {
    /**
     * @brief Saves the database data to the save file
     *
     */
    bool save() const;

    /**
     * @brief Loads the database data from the save file
     *
     */
    bool load();

    std::unordered_map<pplmn::Id, std::string> names;
    std::unordered_map<pplmn::Id, std::string> descriptions;
    std::unordered_map<pplmn::Id, pplmn::Type> types;
    std::unordered_map<pplmn::Id, pplmn::SpecialAbility> abilities;
    std::unordered_map<pplmn::Id, pplmn::Stats> stats;
    std::unordered_map<pplmn::Id, std::unordered_set<pplmn::MoveId>> validMoves;
    std::unordered_map<pplmn::Id, std::unordered_map<unsigned int, pplmn::MoveId>> learnedMoves;
    std::unordered_map<pplmn::Id, unsigned int> evolveLevels;
    std::unordered_map<pplmn::Id, pplmn::Id> evolveIds;
    std::unordered_map<pplmn::Id, pplmn::Stats> evAwards;
    std::unordered_map<pplmn::Id, unsigned int> xpGroups;
    std::unordered_map<pplmn::Id, int> xpMults;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct SerializableObject<core::file::PeoplemonDB> : public SerializableObjectBase {
    using DB = core::file::PeoplemonDB;
    using Id = core::pplmn::Id;

    SerializableField<1, std::unordered_map<Id, std::string>, offsetof(DB, names)> names;
    SerializableField<2, std::unordered_map<Id, std::string>, offsetof(DB, descriptions)>
        descriptions;
    SerializableField<3, std::unordered_map<Id, core::pplmn::Type>, offsetof(DB, types)> types;
    SerializableField<4, std::unordered_map<Id, core::pplmn::SpecialAbility>,
                      offsetof(DB, abilities)>
        abilities;
    SerializableField<5, std::unordered_map<Id, core::pplmn::Stats>, offsetof(DB, stats)> stats;
    SerializableField<6, std::unordered_map<Id, std::unordered_set<core::pplmn::MoveId>>,
                      offsetof(DB, validMoves)>
        validMoves;
    SerializableField<7,
                      std::unordered_map<Id, std::unordered_map<unsigned int, core::pplmn::MoveId>>,
                      offsetof(DB, learnedMoves)>
        learnedMoves;
    SerializableField<8, std::unordered_map<Id, unsigned int>, offsetof(DB, evolveLevels)>
        evolveLevels;
    SerializableField<9, std::unordered_map<Id, Id>, offsetof(DB, evolveIds)> evolveIds;
    SerializableField<10, std::unordered_map<Id, core::pplmn::Stats>, offsetof(DB, evAwards)>
        evAwards;
    SerializableField<11, std::unordered_map<Id, unsigned int>, offsetof(DB, xpGroups)> xpGroups;
    SerializableField<12, std::unordered_map<Id, int>, offsetof(DB, xpMults)> xpMults;

    SerializableObject()
    : names(*this)
    , descriptions(*this)
    , types(*this)
    , abilities(*this)
    , stats(*this)
    , validMoves(*this)
    , learnedMoves(*this)
    , evolveLevels(*this)
    , evolveIds(*this)
    , evAwards(*this)
    , xpGroups(*this)
    , xpMults(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
