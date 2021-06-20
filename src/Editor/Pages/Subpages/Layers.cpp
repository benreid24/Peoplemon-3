#include <Editor/Pages/Subpages/Layers.hpp>

#include <BLIB/Util/Random.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Layers::Layers() {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    ScrollArea::Ptr itemArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 8));
    bottomBox                = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    bottomBox->pack(Label::create("Bottom Layers"), true, false);
    ysortBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    ysortBox->pack(Label::create("Y-Sort Layers"), true, false);
    topBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    topBox->pack(Label::create("Top Layers"), true, false);
    itemArea->pack(bottomBox, true, false);
    itemArea->pack(ysortBox, true, false);
    itemArea->pack(topBox, true, false);

    for (unsigned int i = 0; i < 5; ++i) { rows.emplace_back(i); }
    bottomBox->pack(rows[0].row, true, false);
    bottomBox->pack(rows[1].row, true, false);
    ysortBox->pack(rows[2].row, true, false);
    ysortBox->pack(rows[3].row, true, false);
    topBox->pack(rows[4].row, true, false);

    itemArea->setMaxSize({300, 175});
    content->pack(itemArea, true, false);
}

Box::Ptr Layers::getContent() { return contentWrapper; }

void Layers::pack() { contentWrapper->pack(content, true, true); }

void Layers::unpack() { content->remove(); }

Layers::Item::Item(unsigned int i) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    name = Label::create("Layer " + std::to_string(i));
    name->setColor(sf::Color(bl::util::Random::get<std::uint8_t>(0, 255),
                             bl::util::Random::get<std::uint8_t>(0, 255),
                             bl::util::Random::get<std::uint8_t>(0, 255)),
                   sf::Color::Transparent);
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(true);
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
