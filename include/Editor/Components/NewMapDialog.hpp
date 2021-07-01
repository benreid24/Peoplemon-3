#ifndef EDITOR_COMPONENTS_NEWMAPDIALOG_HPP
#define EDITOR_COMPONENTS_NEWMAPDIALOG_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace component
{
class NewMapDialog {
public:
    using CreateCb =
        std::function<void(const std::string& file, const std::string& name,
                           const std::string& tileset, unsigned int w, unsigned int h)>;

    NewMapDialog(bl::gui::GUI::Ptr parent, const CreateCb& createCb);

    void show(const std::string& file);

private:
    const CreateCb createCb;

    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr mapFileLabel;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr widthEntry;
    bl::gui::TextEntry::Ptr heightEntry;
    bl::gui::Label::Ptr tilesetLabel;
    bl::gui::FilePicker tilesetPicker;
};

} // namespace component
} // namespace editor

#endif
