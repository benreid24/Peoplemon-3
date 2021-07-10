#include <Editor/Pages/Subpages/Levels.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Levels::Levels(const RenderFilterCb& filterCb, const ShiftCb& os, const AddCb& addCb)
: filterCb(filterCb)
, shiftCb(os) {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Box::create(LinePacker::create(LinePacker::Vertical, 12));

    Button::Ptr addBut = Button::create("Add Level");
    addBut->getSignal(Action::LeftClicked).willAlwaysCall([this, addCb](const Action&, Element*) {
        addCb();
    });
    content->pack(addBut);

    itemArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4));
    itemArea->setMaxSize({300, 175});
    content->pack(itemArea, true, false);
}

Box::Ptr Levels::getContent() { return contentWrapper; }

void Levels::pack() { contentWrapper->pack(content, true, true); }

void Levels::unpack() { content->remove(); }

void Levels::sync(const std::vector<bool>& filter) {
    itemArea->clearChildren(true);
    rows.clear();
    rows.reserve(filter.size());
    for (unsigned int i = 0; i < filter.size(); ++i) {
        rows.emplace_back(i, filter.size() - 1, filter[i], filterCb, shiftCb);
        itemArea->pack(rows.back().row, true, false);
    }
}

Levels::Item::Item(unsigned int i, unsigned int mi, bool v, const RenderFilterCb& filterCb,
                   const ShiftCb& shiftCb) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    BL_LOG_INFO << "Level " << i;

    name = Label::create("Level " + std::to_string(i));
    name->setColor(sf::Color(0, 180, 200), sf::Color::Transparent);
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(v);
    visibleToggle->getSignal(Action::ValueChanged)
        .willAlwaysCall([this, i, &filterCb](const Action&, Element*) {
            BL_LOG_INFO << i << " -> " << visibleToggle->getValue();
            filterCb(i, visibleToggle->getValue());
        });
    row->pack(visibleToggle, false, true);

    upBut = Button::create("Up");
    upBut->setActive(i > 0);
    upBut->getSignal(Action::LeftClicked)
        .willAlwaysCall([this, i, &shiftCb](const Action&, Element*) { shiftCb(i, true); });
    row->pack(upBut, false, true);

    downBut = Button::create("Down");
    downBut->setActive(i < mi);
    downBut->getSignal(Action::LeftClicked)
        .willAlwaysCall([this, i, &shiftCb](const Action&, Element*) { shiftCb(i, false); });
    row->pack(downBut, false, true);

    delBut = Button::create("Delete");
    delBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    row->pack(delBut, false, true);
}

} // namespace page
} // namespace editor
