#include <Editor/Pages/Subpages/Layers.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Layers::Layers() {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Uniform));
    firstYSortSelect = ComboBox::create();
    firstYSortSelect->addOption("Layer 0");
    firstYSortSelect->addOption("Layer 1");
    firstYSortSelect->addOption("Layer 2");
    firstYSortSelect->addOption("Layer 3");
    firstYSortSelect->addOption("Layer 4");
    firstYSortSelect->setSelectedOption(0);
    firstYSortSelect->setHorizontalAlignment(RenderSettings::Left);
    row->pack(Label::create("First y-sort layer:"));
    row->pack(firstYSortSelect);
    content->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Uniform));
    firstTopSelect = ComboBox::create();
    firstTopSelect->addOption("Layer 0");
    firstTopSelect->addOption("Layer 1");
    firstTopSelect->addOption("Layer 2");
    firstTopSelect->addOption("Layer 3");
    firstTopSelect->addOption("Layer 4");
    firstTopSelect->setSelectedOption(0);
    firstTopSelect->setHorizontalAlignment(RenderSettings::Left);
    row->pack(Label::create("First top layer:"));
    row->pack(firstTopSelect);
    content->pack(row, true, false);

    itemArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4));
    rows.reserve(5);
    for (unsigned int i = 0; i < 5; ++i) {
        rows.emplace_back(i);
        itemArea->pack(rows.back().row, true, false);
    }
    itemArea->setMaxSize({300, 175});
    content->pack(itemArea, true, false);
}

Box::Ptr Layers::getContent() { return contentWrapper; }

void Layers::pack() { contentWrapper->pack(content, true, true); }

void Layers::unpack() { content->remove(); }

Layers::Item::Item(unsigned int i) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    name = Label::create("Layer " + std::to_string(i));
    name->setColor(sf::Color(0, 180, 200), sf::Color::Transparent);
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
