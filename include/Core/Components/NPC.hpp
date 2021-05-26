#ifndef CORE_COMPONENTS_NPC_HPP
#define CORE_COMPONENTS_NPC_HPP

#include <BLIB/Entities.hpp>
#include <Core/Files/Conversation.hpp>

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
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 12;

    /**
     * @brief Construct a new NPC component
     *
     * @param name The name of the NPC
     * @param conversation The conversation of the NPC
     */
    NPC(const std::string& name, const file::Conversation& conversation);

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
    const file::Conversation _conversation;
};

} // namespace component
} // namespace core

#endif
