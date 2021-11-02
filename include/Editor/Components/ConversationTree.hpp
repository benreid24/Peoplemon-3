#ifndef EDITOR_COMPONENTS_CONVERSATIONTREE_HPP
#define EDITOR_COMPONENTS_CONVERSATIONTREE_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Conversation.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Special component that renders a clickable tree view of a conversation
 *
 * @ingroup Components
 *
 */
class ConversationTree {
public:
    /// Called when a node is clicked
    using ClickCb = std::function<void(unsigned int)>;

    /**
     * @brief Construct a new Conversation Tree component
     *
     * @param clickCb Callback for when a node is clicked
     * @param container The gui element to pack into
     */
    ConversationTree(const ClickCb& clickCb, bl::gui::Box::Ptr& container);

    /**
     * @brief Updates the rendered tree with the new nodes
     *
     * @param nodes The new tree to render
     */
    void update(const std::vector<core::file::Conversation::Node>& nodes);

private:
    const ClickCb onClick;
};

} // namespace component
} // namespace editor

#endif
