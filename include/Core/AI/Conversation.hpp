#ifndef CORE_AI_CONVERSATION_HPP
#define CORE_AI_CONVERSATION_HPP

#include <Core/Files/Conversation.hpp>

/**
 * @addtogroup AI
 * @ingroup Core
 * @brief Collection of classes and utilities for implementing AI
 *
 */

namespace core
{
namespace system
{
class Systems;
}

/// Collection of classes and utilities for implementing AI
namespace ai
{
/**
 * @brief Wrapper around file::Conversation that tracks current node,
 *
 */
class Conversation {
public:
    /**
     * @brief Creates an empty conversation wrapper
     *
     * @param systems The primary systems object
     *
     */
    Conversation(system::Systems& systems);

    /**
     * @brief Sets the wrapper to point to the given underlying. The underlying must remain in scope
     *        during the lifetime of the wrapper. Sets current node to first waiting node
     *
     * @param conversation The conversation to wrap
     */
    void setConversation(const file::Conversation& conversation);

    /**
     * @brief Returns the current node
     *
     */
    const file::Conversation::Node& currentNode() const;

    /**
     * @brief Returns whether or not the conversation is finished
     *
     */
    bool finished() const;

    /**
     * @brief Continues to the next node on player input
     *
     */
    void notifyNext();

    /**
     * @brief Jumps to the node for a check pass (ie take item/money success etc)
     *
     */
    void notifyCheckPassed();

    /**
     * @brief Jumps to the node for a check pass (ie take item/money fail etc)
     *
     */
    void notifyCheckFailed();

    /**
     * @brief Jumps to the node for the given choice if the current node is a Prompt
     *
     * @param choice The choice that was made
     */
    void notifyChoiceMade(const std::string& choice);

private:
    system::Systems& systems;
    const std::vector<file::Conversation::Node>* nodes;
    unsigned int current;

    void followNodes();
};

} // namespace ai
} // namespace core

#endif
