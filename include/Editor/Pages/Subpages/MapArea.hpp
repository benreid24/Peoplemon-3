#ifndef EDITOR_PAGES_SUBPAGES_MAPAREA_HPP
#define EDITOR_PAGES_SUBPAGES_MAPAREA_HPP

#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Section of the map area with the map itself and related controls
 *
 * @ingroup Pages
 *
 */
class MapArea {
public:
    /**
     * @brief Construct a new Map Area
     *
     * @param clickCb Callback when the map itself is clicked
     * @param syncCb Callback when the GUI should be updated
     * @param systems The main game systems
     */
    MapArea(const component::EditMap::PositionCb& clickCb,
            const component::EditMap::ActionCb& syncCb, core::system::Systems& systems);

    /**
     * @brief Returns the contained map
     *
     */
    component::EditMap& editMap();

    /**
     * @brief Returns the GUI element to pack
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Enables the map controls
     *
     */
    void enableControls();

    /**
     * @brief Disables the map controls when a dialog is opened or the map tab is inactive
     *
     */
    void disableControls();

private:
    const component::EditMap::PositionCb onClick;
    bl::gui::Box::Ptr content;
    bl::gui::Label::Ptr positionLabel;
    bl::gui::Button::Ptr undoBut;
    bl::gui::Button::Ptr redoBut;
    bl::gui::CheckButton::Ptr enableBut;
    bl::gui::CheckButton::Ptr dragBut;
    component::EditMap::Ptr map;
    sf::Vector2i lastDragTile;

    void refreshButtons();
    void onMouseOver(const sf::Vector2f& pixels, const sf::Vector2i& tiles);
};

} // namespace page
} // namespace editor

#endif
