#ifndef EDITOR_COMPONENTS_CONVERSATIONWINDOW_HPP
#define EDITOR_COMPONENTS_CONVERSATIONWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Conversation.hpp>
#include <Editor/Components/ConversationNode.hpp>
#include <Editor/Components/ConversationTree.hpp>

namespace editor
{
namespace component
{
/**
 * @brief A window for choosing and editing conversations
 *
 * @ingroup Components
 *
 */
class ConversationWindow {
public:
    /// Callback for when a conversation is chosen
    using SelectCb = std::function<void(const std::string&)>;

    /// Callback for when the window is closed with no choice made
    using CancelCb = std::function<void()>;

    /**
     * @brief Initialize a new conversation window
     *
     * @param onSelect Callback to call when a conversation is chosen
     * @param onCancel Callback to call when the window is closed
     */
    ConversationWindow(const SelectCb& onSelect, const CancelCb& onCancel);

    /**
     * @brief Actually opens the window and initializes with a conversation
     *
     * @param parent The parent GUI object
     * @param current The conversation to load. Blank to make a new one
     */
    void open(const bl::gui::GUI::Ptr& parent, const std::string& current);

private:
    enum struct FilePickerMode { MakeNew, OpenExisting, SetFile } filePickerMode;

    const SelectCb selectCb;
    const CancelCb cancelCb;
    core::file::Conversation value;
    unsigned int currentNode;

    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Box::Ptr nodeBox;
    ConversationTree::Ptr treeComponent;
    ConversationNode nodeComponent;
    bl::gui::FilePicker filePicker;
    bool dirty;

    bl::gui::Label::Ptr fileLabel;
    bl::gui::Button::Ptr saveBut;

    void makeDirty();
    void makeClean();
    void sync();
    bool validate() const;
    bool confirmDiscard() const;
    void setSelected(unsigned int i);
};

} // namespace component
} // namespace editor

#endif
