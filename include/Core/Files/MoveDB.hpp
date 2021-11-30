#ifndef CORE_FILE_MOVEDB_HPP
#define CORE_FILE_MOVEDB_HPP

#include <BLIB/Files/Binary.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Peoplemon/MoveEffect.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/Type.hpp>

namespace core
{
namespace file
{
class MoveDBLoader;

/**
 * @brief Stores the metadata of all peoplemon moves
 *
 * @ingroup Files
 *
 */
struct MoveDB
: private bl::file::binary::SerializableObject
, private bl::util::NonCopyable {
    /**
     * @brief Creates an empty dataset
     *
     */
    MoveDB();

    /**
     * @brief Loads the moves from the data file
     *
     * @return True on success, false on error
     */
    bool load();

    /**
     * @brief Saves the moves to the data file
     *
     * @return True on success, false on error
     */
    bool save() const;

#define SF bl::file::binary::SerializableField
    SF<1, std::unordered_map<pplmn::MoveId, std::string>> names;
    SF<2, std::unordered_map<pplmn::MoveId, std::string>> descriptions;
    SF<3, std::unordered_map<pplmn::MoveId, std::string>> animationPaths;
    SF<4, std::unordered_map<pplmn::MoveId, pplmn::Type>> types;
    SF<5, std::unordered_map<pplmn::MoveId, int>> damages;
    SF<6, std::unordered_map<pplmn::MoveId, int>> accuracies;
    SF<7, std::unordered_map<pplmn::MoveId, int>> priorities;
    SF<8, std::unordered_map<pplmn::MoveId, unsigned int>> pps;
    SF<9, std::unordered_map<pplmn::MoveId, bool>> contactors;
    SF<10, std::unordered_map<pplmn::MoveId, bool>> specials;
    SF<11, std::unordered_map<pplmn::MoveId, pplmn::MoveEffect>> effects;
    SF<12, std::unordered_map<pplmn::MoveId, int>> effectChances;
    SF<13, std::unordered_map<pplmn::MoveId, int>> effectIntensities;
    SF<14, std::unordered_map<pplmn::MoveId, bool>> effectSelves;
#undef SF

    // TODO - have some sort of struct for render behavior. Something like:
    /*
    struct MoveAnimation {
        struct Action {
            enum Actor {
                MoveUser,
                MoveTarget
            };
            enum Effect {
                Shake,
                Flash,
                StatUp,
                StatDown,
                etc
            };

            Actor actor;
            Effect effect;
            float time; // percentage of move animation time, in range [0, 1]
        };

        std::string path; // points to path containing front and back anims
        std::vector<Action> actions;
    };
    */

    friend class MoveDBLoader;
};

} // namespace file
} // namespace core

#endif
