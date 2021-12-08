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

    SerializableField<1, std::unordered_map<Id, std::string>, offsetof(DB, names)> names;
    SerializableField<2, std::unordered_map<Id, std::string>, offsetof(DB, descriptions)>
        descriptions;
    SerializableField<3, std::unordered_map<Id, std::string>, offsetof(DB, animationPaths)>
        animationPaths;
    SerializableField<4, std::unordered_map<Id, core::pplmn::Type>, offsetof(DB, types)> types;
    SerializableField<5, std::unordered_map<Id, int>, offsetof(DB, damages)> damages;
    SerializableField<6, std::unordered_map<Id, int>, offsetof(DB, accuracies)> accuracies;
    SerializableField<7, std::unordered_map<Id, int>, offsetof(DB, priorities)> priorities;
    SerializableField<8, std::unordered_map<Id, unsigned int>, offsetof(DB, pps)> pps;
    SerializableField<9, std::unordered_map<Id, bool>, offsetof(DB, contactors)> contactors;
    SerializableField<10, std::unordered_map<Id, bool>, offsetof(DB, specials)> specials;
    SerializableField<11, std::unordered_map<Id, core::pplmn::MoveEffect>, offsetof(DB, effects)>
        effects;
    SerializableField<12, std::unordered_map<Id, int>, offsetof(DB, effectChances)> effectChances;
    SerializableField<13, std::unordered_map<Id, int>, offsetof(DB, effectIntensities)>
        effectIntensities;
    SerializableField<14, std::unordered_map<Id, bool>, offsetof(DB, effectSelves)> effectSelves;

    SerializableObject()
    : names(*this)
    , descriptions(*this)
    , animationPaths(*this)
    , types(*this)
    , damages(*this)
    , accuracies(*this)
    , priorities(*this)
    , pps(*this)
    , contactors(*this)
    , specials(*this)
    , effects(*this)
    , effectChances(*this)
    , effectIntensities(*this)
    , effectSelves(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
