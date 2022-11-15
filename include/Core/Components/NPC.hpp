#ifndef CORE_COMPONENTS_NPC_HPP
#define CORE_COMPONENTS_NPC_HPP

#include <Core/Files/Conversation.hpp>
#include <Core/Files/NPC.hpp>

namespace core
{
namespace component
{
/**
 * @brief Adding this to an entity will allow it to be interacted with and talked to
 *
 * @ingroup Components
 *
 */
class NPC {
public:
    /**
     * @brief Construct a new NPC component
     *
     * @param data The NPC file data
     */
    NPC(const file::NPC& data);

    /**
     * @brief The name of the NPC
     *
     */
    const std::string& name() const;

    /**
     * @brief The conversation of the NPC
     *
     */
    const file::Conversation& conversation() const;

private:
    const std::string _name;
    file::Conversation _conversation;
};

} // namespace component
} // namespace core

#endif
