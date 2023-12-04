#include <Editor/Pages/Subpages/Catchables.hpp>

#include <Core/Resources.hpp>
#include <Editor/Components/EditMap.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace page
{
namespace
{
const std::array<sf::Color, 10> colors = {sf::Color(89, 147, 240, 150),
                                          sf::Color(89, 237, 240, 150),
                                          sf::Color(89, 240, 164, 150),
                                          sf::Color(139, 240, 89, 150),
                                          sf::Color(227, 240, 89, 150),
                                          sf::Color(240, 200, 89, 150),
                                          sf::Color(240, 144, 89, 150),
                                          sf::Color(240, 89, 119, 150),
                                          sf::Color(240, 89, 237, 150),
                                          sf::Color(170, 89, 240, 150)};
}

using namespace bl::gui;

Catchables::Catchables(component::EditMap& m)
: map(m)
, editWindow(std::bind(&Catchables::onEdit, this)) {
    content      = Box::create(LinePacker::create(LinePacker::Vertical, 8.f));
    scrollRegion = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 8.f));

    scrollRegion->setColor(sf::Color::Transparent, sf::Color::Black);
    scrollRegion->setOutlineThickness(1.5f);

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 0.f));
    noCatchBut   = RadioButton::create("No wild peoplemon", "none");
    noCatchBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        active = 0;
    });
    Button::Ptr addbut = Button::create("Add Wild Region");
    addbut->setHorizontalAlignment(RenderSettings::Right);
    addbut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        map.addCatchRegion();
    });
    row->pack(noCatchBut, false, true);
    row->pack(addbut, true, true);

    content->pack(row, true, false);
    content->pack(scrollRegion, true, true);
}

void Catchables::setGUI(GUI* g) { gui = g; }

Element::Ptr Catchables::getContent() { return content; }

std::uint8_t Catchables::selected() const { return active; }

sf::Color Catchables::getColor(std::uint8_t i) {
    return i > 0 ? colors[(i - 1) % colors.size()] : sf::Color::Transparent;
}

void Catchables::refresh() {
    scrollRegion->clearChildren(true);
    noCatchBut->setValue(true);
    active = 0;

    rows.clear();
    rows.reserve(map.catchRegions().size());
    for (const auto& region : map.catchRegions()) {
        rows.emplace_back(createRow(
            region,
            std::bind(&Catchables::rowClicked, this, std::placeholders::_1, std::placeholders::_2),
            noCatchBut->getRadioGroup()));
        scrollRegion->pack(rows.back(), true, false);
    }

    refreshColors();
}

void Catchables::refreshColors() {
    for (unsigned int i = 0; i < rows.size(); ++i) {
        rows[i]->setColor(getColor(i + 1), sf::Color::Black);
    }
}

Box::Ptr Catchables::createRow(const core::map::CatchRegion& region, const RowSelect& oscb,
                               bl::gui::RadioButton::Group* group) {
    Box::Ptr row            = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    RadioButton::Ptr select = RadioButton::create(region.name, region.name, group);
    Button::Ptr editBut     = Button::create("Edit");
    Button::Ptr delBut      = Button::create("Delete");

    select->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(oscb, row.get(), RowAction::Select));
    editBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(oscb, row.get(), RowAction::Edit));
    delBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(oscb, row.get(), RowAction::Remove));

    delBut->setColor(sf::Color::Red, sf::Color::Black);
    select->setHorizontalAlignment(RenderSettings::Left);

    row->pack(delBut, false, true);
    row->pack(editBut, false, true);
    row->pack(select, true, true);

    return row;
}

void Catchables::rowClicked(const bl::gui::Box* row, RowAction action) {
    int i = -1;
    for (unsigned int j = 0; j < rows.size(); ++j) {
        if (rows[j].get() == row) {
            i = j;
            break;
        }
    }
    if (i < 0) {
        BL_LOG_ERROR << "Invalid row clicked";
        return;
    }

    switch (action) {
    case Remove:
        map.removeCatchRegion(i);
        break;

    case Edit:
        editing = i;
        editWindow.open(gui, map.catchRegions()[i]);
        break;

    case Select:
    default:
        active = i + 1;
        break;
    }
}

void Catchables::onEdit() { map.editCatchRegion(editing, editWindow.getValue()); }

} // namespace page
} // namespace editor
