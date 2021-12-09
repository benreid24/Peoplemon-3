#ifndef CORE_FILE_MOVEDB_HPP
#define CORE_FILE_MOVEDB_HPP

#include <BLIB/Serialization/Binary.hpp>
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
struct MoveDB : private bl::util::NonCopyable {
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

    std::unordered_map<pplmn::MoveId, std::string> names;
    std::unordered_map<pplmn::MoveId, std::string> descriptions;
    std::unordered_map<pplmn::MoveId, std::string> animationPaths;
    std::unordered_map<pplmn::MoveId, pplmn::Type> types;
    std::unordered_map<pplmn::MoveId, int> damages;
    std::unordered_map<pplmn::MoveId, int> accuracies;
    std::unordered_map<pplmn::MoveId, int> priorities;
    std::unordered_map<pplmn::MoveId, unsigned int> pps;
    std::unordered_map<pplmn::MoveId, bool> contactors;
    std::unordered_map<pplmn::MoveId, bool> specials;
    std::unordered_map<pplmn::MoveId, pplmn::MoveEffect> effects;
    std::unordered_map<pplmn::MoveId, int> effectChances;
    std::unordered_map<pplmn::MoveId, int> effectIntensities;
    std::unordered_map<pplmn::MoveId, bool> effectSelves;

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
    friend class bl::serial::binary::SerializableObject<MoveDB>;
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
struct SerializableObject<core::file::MoveDB> : public SerializableObjectBase {
    using Id = core::pplmn::MoveId;
    using DB = core::file::MoveDB;

    SerializableField<1, DB, std::unordered_map<Id, std::string>> names;
    SerializableField<2, DB, std::unordered_map<Id, std::string>> descriptions;
    SerializableField<3, DB, std::unordered_map<Id, std::string>> animationPaths;
    SerializableField<4, DB, std::unordered_map<Id, core::pplmn::Type>> types;
    SerializableField<5, DB, std::unordered_map<Id, int>> damages;
    SerializableField<6, DB, std::unordered_map<Id, int>> accuracies;
    SerializableField<7, DB, std::unordered_map<Id, int>> priorities;
    SerializableField<8, DB, std::unordered_map<Id, unsigned int>> pps;
    SerializableField<9, DB, std::unordered_map<Id, bool>> contactors;
    SerializableField<10, DB, std::unordered_map<Id, bool>> specials;
    SerializableField<11, DB, std::unordered_map<Id, core::pplmn::MoveEffect>> effects;
    SerializableField<12, DB, std::unordered_map<Id, int>> effectChances;
    SerializableField<13, DB, std::unordered_map<Id, int>> effectIntensities;
    SerializableField<14, DB, std::unordered_map<Id, bool>> effectSelves;

    SerializableObject()
    : names(*this, &DB::names)
    , descriptions(*this, &DB::descriptions)
    , animationPaths(*this, &DB::animationPaths)
    , types(*this, &DB::types)
    , damages(*this, &DB::damages)
    , accuracies(*this, &DB::accuracies)
    , priorities(*this, &DB::priorities)
    , pps(*this, &DB::pps)
    , contactors(*this, &DB::contactors)
    , specials(*this, &DB::specials)
    , effects(*this, &DB::effects)
    , effectChances(*this, &DB::effectChances)
    , effectIntensities(*this, &DB::effectIntensities)
    , effectSelves(*this, &DB::effectSelves) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
