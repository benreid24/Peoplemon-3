#ifndef EDITOR_PAGES_SUBPAGES_LAYERS_HPP
#define EDITOR_PAGES_SUBPAGES_LAYERS_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Works for both layers and levels. Provides controls to add, move, remove, and toggle
 *        visibility for layers or levels
 *
 * @ingroup Pages
 *
 */
class Layers {
public:
    enum Mode {
        Layer,
        Level
    };

    Layers(Mode mode);

    bl::gui::Box::Ptr getContent();

    void pack();

    void unpack();

private:
    struct Item {
        bl::gui::Box::Ptr row;
        bl::gui::Label::Ptr name;
        bl::gui::CheckButton::Ptr visibleToggle;
        bl::gui::Button::Ptr upBut;
        bl::gui::Button::Ptr downBut;
        bl::gui::Button::Ptr delBut;

        Item(const std::string& prefix, unsigned int i);
    };

    const Mode mode;
    const std::string prefix;

    bl::gui::Box::Ptr contentWrapper;
    bl::gui::Box::Ptr content;
    bl::gui::ComboBox::Ptr activeSelect;
    bl::gui::ComboBox::Ptr firstYSortSelect;
    bl::gui::ComboBox::Ptr firstTopSelect;
    bl::gui::ScrollArea::Ptr itemArea;
    std::vector<Item> rows;
};

} // namespace page
} // namespace editor

#endif
