#ifndef EDITOR_COMPONENTS_CONVERSATIONNODE_HPP
#define EDITOR_COMPONENTS_CONVERSATIONNODE_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Conversation.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Helper component for editing individual conversation nodes
 *
 * @ingroup Components
 *
 */
class ConversationNode {
public:
    /// Called when the node value is modified
    using NotifyCb = std::function<void()>;

    /// Called when a new node should be created
    using CreateNode = std::function<unsigned int()>;

    /**
     * @brief Construct a new Conversation Node editor
     *
     * @param editCb Callback for when the node is modified
     * @param deleteCb Callback for when the node is deleted
     * @param regenTree Called when the conversation tree should be re-rendered
     * @param createNode Called when a new conversation node should be created
     * @param container The gui element to pack into
     * @param nodes A pointer to all nodes
     */
    ConversationNode(const NotifyCb& editCb, const NotifyCb& deleteCb, const NotifyCb& regenTree,
                     const CreateNode& createNode, bl::gui::Box::Ptr& container,
                     const std::vector<core::file::Conversation::Node>* nodes);

    /**
     * @brief Update the node editor content with the given node value
     *
     * @param node Value to populate the gui with
     */
    void update(const core::file::Conversation::Node& node);

    /**
     * @brief Get the current node value
     *
     * @return const core::file::Conversation::Node& The current node value
     */
    const core::file::Conversation::Node& getValue() const;

private:
    class NodeConnector {
    public:
        using ChangeCb = std::function<void(unsigned int)>;

        NodeConnector(const std::string& label, const NotifyCb& regenTree,
                      const CreateNode& createNode, const ChangeCb& changeCb,
                      const std::vector<core::file::Conversation::Node>* nodes);

        void sync();
        void setSelected(unsigned int node);
        unsigned int getSelected() const;
        bl::gui::Box::Ptr& content();

    private:
        bl::gui::Box::Ptr row;
        bl::gui::ComboBox::Ptr entry;
        const std::vector<core::file::Conversation::Node>* nodes;
    };

    const NotifyCb onEdit;
    const NotifyCb onDelete;
    const NotifyCb regenTree;
    const CreateNode createNode;
    const std::vector<core::file::Conversation::Node>* allNodes;
    core::file::Conversation::Node values[10];
    core::file::Conversation::Node* current;

    bl::gui::Box::Ptr editArea;

    bl::gui::Box::Ptr promptRow;
    bl::gui::TextEntry::Ptr promptEntry;
    NodeConnector nextNode;
};

} // namespace component
} // namespace editor

#endif
