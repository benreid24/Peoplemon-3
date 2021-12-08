#ifndef CORE_FILES_NPC_HPP
#define CORE_FILES_NPC_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <Core/Files/Behavior.hpp>

/**
 * @addtogroup Files
 * @ingroup Core
 * @brief Collection of data classes that provide saving and loading of different game file formats
 *
 */

namespace core
{
/// Collection of data classes that provide saving and loading of different game file formats
namespace file
{
/**
 * @brief File data for NPCs
 *
 * @ingroup Files
 *
 */
class NPC {
public:
    /**
     * @brief Construct a new NPC file
     *
     */
    NPC() = default;

    /**
     * @brief Saves the NPC data to the given file
     *
     * @param file The file to save to
     * @return True on success, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Loads the NPC data from the given file
     *
     * @param file The file to load from
     * @param spawnDir The direction to put into the standing behavior on load
     * @return True on success, false on error
     */
    bool load(const std::string& file, component::Direction spawnDir = component::Direction::Up);

    /**
     * @brief The name of the NPC
     *
     */
    std::string& name();

    /**
     * @brief The name of the NPC
     *
     */
    const std::string& name() const;

    /**
     * @brief The directory of the movement animations
     *
     */
    std::string& animation();

    /**
     * @brief The directory of the movement animations
     *
     */
    const std::string& animation() const;

    /**
     * @brief The conversation file of the NPC
     *
     */
    std::string& conversation();

    /**
     * @brief The conversation file of the NPC
     *
     */
    const std::string& conversation() const;

    /**
     * @brief The behavior of the NPC
     *
     */
    Behavior& behavior();

    /**
     * @brief The behavior of the NPC
     *
     */
    Behavior behavior() const;

private:
    std::string nameField;
    std::string animField;
    std::string conversationField;
    Behavior behaviorField;

    friend class bl::serial::binary::SerializableObject<NPC>;
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
struct SerializableObject<core::file::NPC> : public SerializableObjectBase {
    using N = core::file::NPC;

    SerializableField<1, std::string, offsetof(N, nameField)> nameField;
    SerializableField<2, std::string, offsetof(N, animField)> animField;
    SerializableField<3, std::string, offsetof(N, conversationField)> conversationField;
    SerializableField<4, core::file::Behavior, offsetof(N, behaviorField)> behaviorField;

    SerializableObject()
    : nameField(*this)
    , animField(*this)
    , conversationField(*this)
    , behaviorField(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
