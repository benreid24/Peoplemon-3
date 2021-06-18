#ifndef EDITOR_PAGES_SUBPAGES_MAPAREA_HPP
#define EDITOR_PAGES_SUBPAGES_MAPAREA_HPP

#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace page
{
class MapArea {
public:
    MapArea(const component::EditMap::PositionCb& clickCb,
            const component::EditMap::ActionCb& syncCb, core::system::Systems& systems);

    component::EditMap& editMap();

    bl::gui::Element::Ptr getContent();

private:
    bl::gui::Box::Ptr content;
    bl::gui::Label::Ptr positionLabel;
    bl::gui::Button::Ptr undoBut;
    bl::gui::Label::Ptr undoText;
    bl::gui::Button::Ptr redoBut;
    bl::gui::Label::Ptr redoText;
    component::EditMap::Ptr map;

    void refreshButtons();
    void onMouseOver(const sf::Vector2f& pixels, const sf::Vector2i& tiles);
};

} // namespace page
} // namespace editor

#endif
