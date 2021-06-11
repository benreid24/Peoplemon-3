#include <Editor/Pages/Subpages/MapArea.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

MapArea::MapArea(const component::EditMap::ClickCb& cb, core::system::Systems& s)
: map(component::EditMap::create(cb, s)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 0));

    Box::Ptr controlRow = Box::create(
        LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Compact, LinePacker::RightAlign));
    CheckButton::Ptr enableBut = CheckButton::create("Enable Map Controls");
    enableBut->getSignal(Action::ValueChanged).willAlwaysCall([this](const Action& a, Element*) {
        map->setControlsEnabled(a.data.bvalue);
    });
    controlRow->pack(enableBut, false, true);

    content->pack(controlRow, true, false);
    content->pack(map, true, true);
}

component::EditMap& MapArea::editMap() { return *map; }

bl::gui::Element::Ptr MapArea::getContent() { return content; }

} // namespace page
} // namespace editor
