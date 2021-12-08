#ifndef CORE_FILES_TRAINER_HPP
#define CORE_FILES_TRAINER_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <Core/Files/Behavior.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace core
{
namespace file
{
/**
 * @brief Static data for trainers in the world
 *
 * @ingroup Files
 *
 */
struct Trainer {
public:
    /**
     * @brief Saves the trainer data to the given file
     *
     * @param file The file to save to
     * @return True on save, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Loads the trainer data from the given file
     *
     * @param file The file to load from
     * @param spawnDir The direction to put in the standing behavior on load
     * @return True on load, false on error
     */
    bool load(const std::string& file, component::Direction spawnDir = component::Direction::Down);

    std::string name;
    std::string animation;
    std::string prebattleConversation;
    std::string postBattleConversation;
    std::string lostBattleLine;
    std::uint8_t visionRange;
    Behavior behavior;
    std::vector<pplmn::OwnedPeoplemon> peoplemon;
    std::vector<item::Id> items;
    // TODO - determine how to store battle music/background
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
struct SerializableObject<core::file::Trainer> : public SerializableObjectBase {
    using T   = core::file::Trainer;
    using Ppl = core::pplmn::OwnedPeoplemon;

    SerializableField<1, std::string, offsetof(T, name)> name;
    SerializableField<2, std::string, offsetof(T, animation)> anim;
    SerializableField<3, std::string, offsetof(T, prebattleConversation)> preBattle;
    SerializableField<4, std::string, offsetof(T, postBattleConversation)> postBattle;
    SerializableField<5, std::string, offsetof(T, lostBattleLine)> loseBattleLine;
    SerializableField<6, std::uint8_t, offsetof(T, visionRange)> range;
    SerializableField<7, core::file::Behavior, offsetof(T, behavior)> behavior;
    SerializableField<8, std::vector<Ppl>, offsetof(T, peoplemon)> peoplemon;
    SerializableField<9, std::vector<core::item::Id>, offsetof(T, items)> items;

    SerializableObject()
    : name(*this)
    , anim(*this)
    , preBattle(*this)
    , postBattle(*this)
    , loseBattleLine(*this)
    , range(*this)
    , behavior(*this)
    , peoplemon(*this)
    , items(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
