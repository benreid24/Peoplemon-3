#ifndef EDITOR_COMPONENTS_CONVERSATIONNODE_HPP
#define EDITOR_COMPONENTS_CONVERSATIONNODE_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Conversation.hpp>
#include <Editor/Components/ItemSelector.hpp>
#include <Editor/Components/ScriptSelector.hpp>

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

    /// Called when a node is jumped to
    using SelectCb = std::function<void(unsigned int)>;

    // Called when the window should force focus or not
    using FocusCb = std::function<void(bool)>;

    /**
     * @brief Construct a new Conversation Node editor
     *
     * @param focusCb Called when the parent window should force focus or not
     * @param editCb Callback for when the node is modified
     * @param deleteCb Callback for when the node is deleted
     * @param regenTree Called when the conversation tree should be re-rendered
     * @param createNode Called when a new conversation node should be created
     * @param selectCb Called when a node is jumped to
     * @param container The gui element to pack into
     * @param nodes A pointer to all nodes
     */
    ConversationNode(const FocusCb& focusCb, const NotifyCb& editCb, const NotifyCb& deleteCb,
                     const NotifyCb& regenTree, const CreateNode& createNode,
                     const SelectCb& selectCb, bl::gui::Box::Ptr& container,
                     const std::vector<core::file::Conversation::Node>* nodes);

    /**
     * @brief Update the node editor content with the given node value
     *
     * @param i The index of the current node
     * @param node Value to populate the gui with
     */
    void update(unsigned int i, const core::file::Conversation::Node& node);

    /**
     * @brief Get the current node value
     *
     * @return const core::file::Conversation::Node& The current node value
     */
    const core::file::Conversation::Node& getValue() const;

    /**
     * @brief Set the parent GUI object
     *
     */
    void setParent(const bl::gui::GUI::Ptr& parent);

private:
    class NodeConnector {
    public:
        using ChangeCb = std::function<void(unsigned int)>;

        NodeConnector(const std::string& prompt, const NotifyCb& regenTree,
                      const CreateNode& createNode, const NotifyCb& syncJumpCb,
                      const ChangeCb& changeCb, const SelectCb& selectCb,
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

    class Choice {
    public:
        using OnChange = std::function<void(unsigned int, const std::string&, unsigned int)>;
        using OnDelete = std::function<void(unsigned int, std::list<Choice>::iterator)>;

        Choice(unsigned int index, const std::string& c, unsigned int jump,
               const OnChange& onChange, const OnDelete& onDelete, const NotifyCb& regenTree,
               const CreateNode& createNode, const NotifyCb& syncJumpCb, const SelectCb& selectCb,
               const std::vector<core::file::Conversation::Node>* nodes);

        void setIndex(unsigned int i);
        void setIterator(std::list<Choice>::iterator it);
        bl::gui::Box::Ptr& content();
        void syncJump();

    private:
        const OnChange onChange;
        const OnDelete onDelete;
        const SelectCb onJump;
        unsigned int index;
        std::list<Choice>::iterator it;
        bl::gui::Box::Ptr box;
        bl::gui::TextEntry::Ptr entry;
        NodeConnector jump;
    };

    bl::gui::GUI::Ptr parent;
    const FocusCb focusCb;
    const NotifyCb onEdit;
    const NotifyCb onDelete;
    const NotifyCb regenTree;
    const CreateNode createNodeCb;
    const SelectCb onJump;
    const std::vector<core::file::Conversation::Node>* allNodes;
    core::file::Conversation::Node current;
    unsigned int index;

    bl::gui::Label::Ptr nodeTitle;
    bl::gui::Box::Ptr editArea;
    bl::gui::ComboBox::Ptr typeEntry;

    NodeConnector passNext;
    NodeConnector failNext;

    bl::gui::Box::Ptr promptRow;
    bl::gui::TextEntry::Ptr promptEntry;
    NodeConnector nextNode;

    bl::gui::Box::Ptr choiceArea;
    bl::gui::ScrollArea::Ptr choiceScroll;
    std::list<Choice> choices;
    void onChoiceChange(unsigned int j, const std::string& t, unsigned int n);
    void onChoiceDelete(unsigned int i, std::list<Choice>::iterator it);
    void syncJumps();

    component::ItemSelector::Ptr itemSelector;
    bl::gui::Box::Ptr itemRow;
    void onItemChange(core::item::Id item);

    bl::gui::Box::Ptr moneyRow;
    bl::gui::TextEntry::Ptr moneyEntry;
    void onMoneyChange();

    bl::gui::Box::Ptr flagRow;
    bl::gui::TextEntry::Ptr flagEntry;

    bl::gui::Box::Ptr scriptRow;
    bl::gui::Label::Ptr scriptLabel;
    ScriptSelector scriptSelector;

    void onTypeChange();
};

} // namespace component
} // namespace editor

#endif
