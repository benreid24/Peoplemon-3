#ifndef EDITOR_PAGES_SUBPAGES_LAYERS_HPP
#define EDITOR_PAGES_SUBPAGES_LAYERS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Map.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Provides controls to add, move, remove, and toggle visibility for layers
 *
 * @ingroup Pages
 *
 */
class Layers {
public:
    /**
     * @brief Construct a new Layers subpage
     *
     */
    Layers();

    /**
     * @brief Syncs the GUI elements with the map layers that exist
     *
     * @param levels The levels in the map
     */
    void sync(const std::vector<core::map::LayerSet>& levels);

    /**
     * @brief Returns the GUI content to pack
     *
     */
    bl::gui::Box::Ptr getContent();

    /**
     * @brief Packs the GUI content
     *
     */
    void pack();

    /**
     * @brief Hides the GUI content. Good for saving space when not active
     *
     */
    void unpack();

private:
    struct Item {
        bl::gui::Box::Ptr row;
        bl::gui::Label::Ptr name;
        bl::gui::CheckButton::Ptr visibleToggle;
        bl::gui::Button::Ptr upBut;
        bl::gui::Button::Ptr downBut;
        bl::gui::Button::Ptr delBut;

        Item(unsigned int i);
    };

    bl::gui::Box::Ptr contentWrapper;
    bl::gui::Notebook::Ptr content;
    bl::gui::ComboBox::Ptr activeSelect;
    bl::gui::Box::Ptr bottomBox;
    bl::gui::Box::Ptr ysortBox;
    bl::gui::Box::Ptr topBox;

    std::vector<Item> rows;
    std::vector<bool> visible;
};

} // namespace page
} // namespace editor

#endif
