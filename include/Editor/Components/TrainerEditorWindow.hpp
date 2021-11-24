#ifndef EDITOR_COMPONENTS_TRAINEREDITORWINDOW_HPP
#define EDITOR_COMPONENTS_TRAINEREDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Editor/Components/AnimationWindow.hpp>
#include <Editor/Components/BehaviorEditor.hpp>
#include <Editor/Components/ConversationWindow.hpp>
#include <Editor/Components/ItemSelector.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief Editor for NPC files
 *
 * @ingroup UIComponents
 *
 */
class TrainerEditorWindow {
public:
    /// Callback for when a trainer file is selected
    using SelectCb = std::function<void(const std::string& file)>;

    /// Called when the window is closed
    using CloseCb = std::function<void()>;

    /**
     * @brief Construct a new Trainer Editor Window
     *
     * @param cb Callback for when a trainer file is selected
     * @param cb Callback for when the window is closed
     */
    TrainerEditorWindow(const SelectCb& cb, const CloseCb& closeCb);

    /**
     * @brief Opens the trainer editor
     *
     * @param parent The parent GUI element
     * @param file The file to open. Empty for new file
     */
    void show(bl::gui::GUI::Ptr parent, const std::string& file);

    /**
     * @brief Hides the window and all created child windows
     *
     */
    void hide();

private:
    const SelectCb selectCb;
    const CloseCb closeCb;
    bool clean;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Button::Ptr saveBut;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::Label::Ptr animLabel;
    bl::gui::Label::Ptr bbLabel;
    bl::gui::Label::Ptr abLabel;
    bl::gui::TextEntry::Ptr loseLineEntry;
    bl::gui::ComboBox::Ptr visionRangeEntry;

    core::item::Id curItem;
    bl::gui::SelectBox::Ptr itemSelectBox;
    ItemSelector::Ptr itemSelector;
    std::vector<core::item::Id> items;

    bl::gui::SelectBox::Ptr pplBox;
    std::vector<core::pplmn::OwnedPeoplemon> peoplemon;

    bl::gui::FilePicker filePicker;
    bool makingNew;
    void onChooseFile(const std::string& file);

    AnimationWindow animWindow;
    void onChooseAnimation(const std::string& anim);

    BehaviorEditor behaviorEditor;

    ConversationWindow conversationWindow;
    bool selectingBB;
    void onChooseConversation(const std::string& conv);

    void forceWindowOnTop();

    void makeDirty();
    void makeClean();
    void reset();
    void load(const std::string& file);
    bool validate(bool saving) const;
    bool confirmDiscard() const;
};

} // namespace component
} // namespace editor

#endif
