#ifndef CORE_FILES_TRAINER_HPP
#define CORE_FILES_TRAINER_HPP

#include <Core/Files/Behavior.hpp>

#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

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
class Trainer : public bl::file::binary::SerializableObject {
public:
    /**
     * @brief Construct an empty Trainer
     *
     */
    Trainer();

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

    /**
     * @brief The name of the trainer
     *
     */
    std::string& name();

    /**
     * @brief The name of the trainer
     *
     */
    const std::string& name() const;

    /**
     * @brief The animation file of the trainer
     *
     */
    std::string& animation();

    /**
     * @brief The animation file of the trainer
     *
     */
    const std::string& animation() const;

    /**
     * @brief The prebattle conversation file of the trainer
     *
     */
    std::string& prebattleConversation();

    /**
     * @brief The prebattle conversation file of the trainer
     *
     */
    const std::string& prebattleConversation() const;

    /**
     * @brief The postbattle conversation file of the trainer
     *
     */
    std::string& postBattleConversation();

    /**
     * @brief The postbattle conversation file of the trainer
     *
     */
    const std::string& postBattleConversation() const;

    /**
     * @brief The line that is said when trainer loses battle
     *
     */
    std::string& lostBattleLine();

    /**
     * @brief The line that is said when trainer loses battle
     *
     */
    const std::string& lostBattleLine() const;

    /**
     * @brief How far the trainer can see, in tiles
     *
     */
    std::uint8_t& visionRange();

    /**
     * @brief How far the trainer can see, in tiles
     *
     */
    std::uint8_t visionRange() const;

    /**
     * @brief The behavior of the trainer
     *
     */
    Behavior& behavior();

    /**
     * @brief The behavior of the trainer
     *
     */
    Behavior behavior() const;

private:
    bl::file::binary::SerializableField<1, std::string> nameField;
    bl::file::binary::SerializableField<2, std::string> animField;
    bl::file::binary::SerializableField<3, std::string> preBattleField;
    bl::file::binary::SerializableField<4, std::string> postBattleField;
    bl::file::binary::SerializableField<5, std::string> loseBattleLineField;
    bl::file::binary::SerializableField<6, std::uint8_t> rangeField;
    bl::file::binary::SerializableField<7, Behavior> behaviorField;
    // TODO - determine how to store peoplemon and battle music/background
};

} // namespace file
} // namespace core

#endif
