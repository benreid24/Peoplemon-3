#include <Editor/Pages/Subpages/Levels.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Levels::Levels(const RenderFilterCb& filterCb)
: filterCb(filterCb) {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    itemArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4));
    itemArea->setMaxSize({300, 175});
    content->pack(itemArea, true, false);
}

Box::Ptr Levels::getContent() { return contentWrapper; }

void Levels::pack() { contentWrapper->pack(content, true, true); }

void Levels::unpack() { content->remove(); }

void Levels::sync(const std::vector<bool>& filter) {
    itemArea->clearChildren(true);
    rows.reserve(filter.size());
    for (unsigned int i = 0; i < filter.size(); ++i) {
        rows.emplace_back(i, filter[i], filterCb);
        itemArea->pack(rows.back().row, true, false);
    }
}

Levels::Item::Item(unsigned int i, bool v, const RenderFilterCb& filterCb) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    name = Label::create("Level " + std::to_string(i));
    name->setColor(sf::Color(0, 180, 200), sf::Color::Transparent);
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(v);
    visibleToggle->getSignal(Action::ValueChanged)
        .willAlwaysCall([this, i, filterCb](const Action&, Element*) {
            filterCb(i, visibleToggle->getValue());
        });
    row->pack(visibleToggle, false, true);

    upBut = Button::create("Up");
    row->pack(upBut, false, true);

    downBut = Button::create("Down");
    row->pack(downBut, false, true);

    delBut = Button::create("Delete");
    delBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    row->pack(delBut, false, true);
}

} // namespace page
} // namespace editor
