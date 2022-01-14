#ifndef EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP
#define EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/CatchRegion.hpp>

namespace editor
{
namespace component
{
class EditMap;
}

namespace page
{
/**
 * @brief Editor subpage for selecting catch tiles to place
 *
 * @ingroup Pages
 *
 */
class Catchables {
public:
    /**
     * @brief Construct a new Catchables GUI
     *
     * @param map The map being edited
     *
     */
    Catchables(component::EditMap& map);

    /**
     * @brief Returns the GUI element to pack
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Returns the currently selected catch type
     *
     */
    std::uint8_t selected() const;

    /**
     * @brief Returns the color to use for the given catch region
     *
     */
    static sf::Color getColor(std::uint8_t index);

    /**
     * @brief Refreshes GUI elements
     *
     */
    void refresh();

private:
    component::EditMap& map;
    bl::gui::Box::Ptr content;
    bl::gui::ScrollArea::Ptr scrollRegion;
    bl::gui::RadioButton::Ptr noCatchBut;
    std::vector<bl::gui::Box::Ptr> rows;
    std::uint8_t active;

    enum RowAction { Edit, Remove, Select };
    using RowSelect = std::function<void(const bl::gui::Box*, RowAction)>;

    bl::gui::Box::Ptr createRow(const core::map::CatchRegion& region, const RowSelect& oscb,
                                bl::gui::RadioButton::Group* group);
    void rowClicked(const bl::gui::Box* row, RowAction action);
    void refreshColors();
};

} // namespace page
} // namespace editor

#endif
