#ifndef CORE_FILES_NPC_HPP
#define CORE_FILES_NPC_HPP

#include <Core/Files/Behavior.hpp>

#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

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
class NPC : public bl::file::binary::SerializableObject {
public:
    /**
     * @brief Construct a new NPC file
     *
     */
    NPC();

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
     * @return True on success, false on error
     */
    bool load(const std::string& file);

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
    bl::file::binary::SerializableField<1, std::string> nameField;
    bl::file::binary::SerializableField<2, std::string> animField;
    bl::file::binary::SerializableField<3, std::string> conversationField;
    bl::file::binary::SerializableField<4, Behavior> behaviorField;
};

} // namespace file
} // namespace core

#endif
