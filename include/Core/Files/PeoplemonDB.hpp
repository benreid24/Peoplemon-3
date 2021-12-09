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

    SerializableField<1, DB, std::unordered_map<Id, std::string>> names;
    SerializableField<2, DB, std::unordered_map<Id, std::string>> descriptions;
    SerializableField<3, DB, std::unordered_map<Id, core::pplmn::Type>> types;
    SerializableField<4, DB, std::unordered_map<Id, core::pplmn::SpecialAbility>> abilities;
    SerializableField<5, DB, std::unordered_map<Id, core::pplmn::Stats>> stats;
    SerializableField<6, DB, std::unordered_map<Id, std::unordered_set<core::pplmn::MoveId>>>
        validMoves;
    SerializableField<7, DB,
                      std::unordered_map<Id, std::unordered_map<unsigned int, core::pplmn::MoveId>>>
        learnedMoves;
    SerializableField<8, DB, std::unordered_map<Id, unsigned int>> evolveLevels;
    SerializableField<9, DB, std::unordered_map<Id, Id>> evolveIds;
    SerializableField<10, DB, std::unordered_map<Id, core::pplmn::Stats>> evAwards;
    SerializableField<11, DB, std::unordered_map<Id, unsigned int>> xpGroups;
    SerializableField<12, DB, std::unordered_map<Id, int>> xpMults;

    SerializableObject()
    : names(*this, &DB::names)
    , descriptions(*this, &DB::descriptions)
    , types(*this, &DB::types)
    , abilities(*this, &DB::abilities)
    , stats(*this, &DB::stats)
    , validMoves(*this, &DB::validMoves)
    , learnedMoves(*this, &DB::learnedMoves)
    , evolveLevels(*this, &DB::evolveLevels)
    , evolveIds(*this, &DB::evolveIds)
    , evAwards(*this, &DB::evAwards)
    , xpGroups(*this, &DB::xpGroups)
    , xpMults(*this, &DB::xpMults) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
