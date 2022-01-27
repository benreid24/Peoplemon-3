#ifndef EDITOR_PAGES_SUBPAGES_TOWNS_HPP
#define EDITOR_PAGES_SUBPAGES_TOWNS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Town.hpp>
#include <Editor/Components/EditMap.hpp>
#include <Editor/Components/WeatherSelect.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Subpage for creating, editing, deleting, and placing towns in maps
 *
 * @ingroup Pages
 *
 */
class Towns {
public:
    /**
     * @brief Creates the towns subpage and GUI elements
     *
     * @param map The edit map component
     */
    Towns(component::EditMap& map);

    /**
     * @brief Set the primary GUI object
     *
     * @param gui The primary GUI object
     */
    void setGUI(const bl::gui::GUI::Ptr* gui);

    /**
     * @brief Refreshes the list of towns in the page
     *
     */
    void refresh();

private:
    bl::gui::GUI::Ptr gui;
    component::EditMap& map;

    bl::gui::ScrollArea::Ptr scrollRegion;
    bl::gui::RadioButton::Ptr noTownBut;

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr nameEntry;
    // TODO - playlist picker component
    component::WeatherSelect::Ptr weatherSelect;
};

} // namespace page
} // namespace editor

#endif
