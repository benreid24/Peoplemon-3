#ifndef CORE_FILES_PEOPLEMONDB_HPP
#define CORE_FILES_PEOPLEMONDB_HPP

#include <BLIB/Files/Binary.hpp>
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
class PeoplemonDBLoader;

struct PeoplemonDB
: private bl::file::binary::SerializableObject
, private bl::util::NonCopyable {
    /**
     * @brief Makes a new empty peoplemon database
     *
     */
    PeoplemonDB();

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

#define SF bl::file::binary::SerializableField
    SF<1, std::unordered_map<pplmn::Id, std::string>> names;
    SF<2, std::unordered_map<pplmn::Id, std::string>> descriptions;
    SF<3, std::unordered_map<pplmn::Id, pplmn::Type>> types;
    SF<4, std::unordered_map<pplmn::Id, pplmn::SpecialAbility>> abilities;
    SF<5, std::unordered_map<pplmn::Id, pplmn::Stats>> stats;
    SF<6, std::unordered_map<pplmn::Id, std::unordered_set<pplmn::MoveId>>> validMoves;
    SF<7, std::unordered_map<pplmn::Id, std::unordered_map<unsigned int, pplmn::MoveId>>>
        learnedMoves;
    SF<8, std::unordered_map<pplmn::Id, unsigned int>> evolveLevels;
    SF<9, std::unordered_map<pplmn::Id, pplmn::Id>> evolveIds;
    SF<10, std::unordered_map<pplmn::Id, pplmn::Stats>> evAwards;
    SF<11, std::unordered_map<pplmn::Id, unsigned int>> xpGroups;
    SF<12, std::unordered_map<pplmn::Id, int>> xpMults;
#undef SF

    friend class PeoplemonDBLoader;
};

} // namespace file
} // namespace core

#endif
