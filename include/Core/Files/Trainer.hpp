#ifndef CORE_FILES_TRAINER_HPP
#define CORE_FILES_TRAINER_HPP

#include <Core/Files/Behavior.hpp>

#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

namespace core
{
namespace file
{
class Trainer : public bl::file::binary::SerializableObject {
public:
    Trainer();

    bool save(const std::string& file) const;

    bool load(const std::string& file);

    std::string& name();

    const std::string& name() const;

    std::string& animation();

    const std::string& animation() const;

    std::string& prebattleConversation();

    const std::string& prebattleConversation() const;

    std::string& postBattleConversation();

    const std::string& postBattleConversation() const;

    std::string& lostBattleLine();

    const std::string& lostBattleLine() const;

    std::uint8_t& visionRange();

    std::uint8_t visionRange() const;

    Behavior& behavior();

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
