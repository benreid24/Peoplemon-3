#include <Editor/Pages/Subpages/MapArea.hpp>

namespace editor
{
namespace page
{
MapArea::MapArea(const component::EditMap::ClickCb& cb, core::system::Systems& s)
: map(component::EditMap::create(cb, s)) {}

component::EditMap& MapArea::editMap() { return *map; }

bl::gui::Element::Ptr MapArea::getContent() { return map; }

} // namespace page
} // namespace editor
