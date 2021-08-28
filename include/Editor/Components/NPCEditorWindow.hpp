#ifndef EDITOR_COMPONENTS_NPCEDITORWINDOW_HPP
#define EDITOR_COMPONENTS_NPCEDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief Editor for NPC files
 *
 * @ingroup Components
 *
 */
class NpcEditorWindow {
public:
    /// Callback for when an NPC file is selected
    using SelectCb = std::function<void(const std::string& file)>;

    /**
     * @brief Construct a new Npc Editor Window
     *
     * @param cb Callback for when an NPC file is selected
     */
    NpcEditorWindow(const SelectCb& cb);

    /**
     * @brief Opens the NPC editor
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
    bool clean;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Button::Ptr saveBut;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::Label::Ptr animLabel;
    bl::gui::Label::Ptr convLabel;

    bl::gui::FilePicker filePicker;
    bool makingNew;
    void onChooseFile(const std::string& file);

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
