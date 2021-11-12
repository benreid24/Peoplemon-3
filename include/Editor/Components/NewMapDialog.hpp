#ifndef EDITOR_COMPONENTS_NEWMAPDIALOG_HPP
#define EDITOR_COMPONENTS_NEWMAPDIALOG_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Dialog for creating new maps
 *
 * @ingroup UIComponents
 *
 */
class NewMapDialog {
public:
    /// Callback for when maps are created
    using CreateCb =
        std::function<void(const std::string& file, const std::string& name,
                           const std::string& tileset, unsigned int w, unsigned int h)>;

    /**
     * @brief Creates a new new-map dialoig
     *
     * @param createCb The callback to trigger when a new map should be created
     */
    NewMapDialog(const CreateCb& createCb);

    /**
     * @brief Opens the new map dialog
     *
     * @param parent The parent GUI element
     * @param file The map file to save to
     */
    void show(bl::gui::GUI::Ptr parent, const std::string& file);

private:
    const CreateCb createCb;

    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr mapFileLabel;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr widthEntry;
    bl::gui::TextEntry::Ptr heightEntry;
    bl::gui::Label::Ptr tilesetLabel;
    bl::gui::FilePicker tilesetPicker;
    bl::gui::Button::Ptr pickBut;
    bool pickInit;
};

} // namespace component
} // namespace editor

#endif
