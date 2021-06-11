#ifndef EDITOR_PAGES_SUBPAGES_MAPAREA_HPP
#define EDITOR_PAGES_SUBPAGES_MAPAREA_HPP

#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace page
{
class MapArea {
public:
    MapArea(const component::EditMap::ClickCb& clickCb, core::system::Systems& systems);

    component::EditMap& editMap();

    bl::gui::Element::Ptr getContent();

private:
    bl::gui::Box::Ptr content;
    component::EditMap::Ptr map;
};

} // namespace page
} // namespace editor

#endif
