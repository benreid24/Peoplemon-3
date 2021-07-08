#ifndef EDITOR_PAGES_SUBPAGES_LEVELS_HPP
#define EDITOR_PAGES_SUBPAGES_LEVELS_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Provides controls to add, move, remove, and toggle visibility for levels
 *
 * @ingroup Pages
 *
 */
class Levels {
public:
    using RenderFilterCb = std::function<void(unsigned int level, bool visible)>;

    /**
     * @brief Construct a new Levels subpage
     *
     * @param filterCb Shows or hides a level
     *
     */
    Levels(const RenderFilterCb& filterCb);

    /**
     * @brief Syncs the number of levels and their visible status
     *
     * @param filter The current render filter for levels
     */
    void sync(const std::vector<bool>& filter);

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

        Item(unsigned int i, bool visible, const RenderFilterCb& filterCb);
    };

    const RenderFilterCb filterCb;

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
