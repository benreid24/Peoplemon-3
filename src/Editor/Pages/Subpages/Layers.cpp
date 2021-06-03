#include <Editor/Pages/Subpages/Layers.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Layers::Layers(Mode m)
: mode(m)
, prefix(m == Layer ? "Layer" : "Level") {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Uniform));
    activeSelect = ComboBox::create();
    activeSelect->addOption(prefix + " 0");
    activeSelect->addOption(prefix + " 1");
    activeSelect->addOption(prefix + " 2");
    activeSelect->addOption(prefix + " 3");
    activeSelect->addOption(prefix + " 4");
    activeSelect->setHorizontalAlignment(RenderSettings::Left);
    row->pack(Label::create("Currently editing:"), true, true);
    row->pack(activeSelect, false, true);
    content->pack(row);

    if (mode == Layer) {
        row = Box::create(LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Uniform));
        firstYSortSelect = ComboBox::create();
        firstYSortSelect->addOption(prefix + " 0");
        firstYSortSelect->addOption(prefix + " 1");
        firstYSortSelect->addOption(prefix + " 2");
        firstYSortSelect->addOption(prefix + " 3");
        firstYSortSelect->addOption(prefix + " 4");
        firstYSortSelect->setHorizontalAlignment(RenderSettings::Left);
        row->pack(Label::create("First y-sort layer:"));
        row->pack(firstYSortSelect);
        content->pack(row, true, false);

        row = Box::create(LinePacker::create(LinePacker::Horizontal, 2, LinePacker::Uniform));
        firstTopSelect = ComboBox::create();
        firstTopSelect->addOption(prefix + " 0");
        firstTopSelect->addOption(prefix + " 1");
        firstTopSelect->addOption(prefix + " 2");
        firstTopSelect->addOption(prefix + " 3");
        firstTopSelect->addOption(prefix + " 4");
        firstTopSelect->setHorizontalAlignment(RenderSettings::Left);
        row->pack(Label::create("First top layer:"));
        row->pack(firstTopSelect);
        content->pack(row, true, false);
    }

    itemArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4));
    rows.reserve(5);
    for (unsigned int i = 0; i < 5; ++i) {
        rows.emplace_back(prefix, i);
        itemArea->pack(rows.back().row, true, false);
    }
    itemArea->setMaxSize({400, 1000});
    content->pack(itemArea, true, false);
}

Box::Ptr Layers::getContent() { return contentWrapper; }

void Layers::pack() { contentWrapper->pack(content, true, true); }

void Layers::unpack() { content->remove(); }

Layers::Item::Item(const std::string& prefix, unsigned int i) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    name = Label::create(prefix + " " + std::to_string(i));
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(true);
    row->pack(visibleToggle, false, true);

    upBut = Button::create("Move Up");
    row->pack(upBut, false, true);

    downBut = Button::create("Move Down");
    row->pack(downBut, false, true);

    delBut = Button::create("Delete");
    delBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    row->pack(delBut, false, true);
}

} // namespace page
} // namespace editor
