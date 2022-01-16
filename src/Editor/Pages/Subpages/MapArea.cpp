#include <Editor/Pages/Subpages/MapArea.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

MapArea::MapArea(const component::EditMap::PositionCb& cb,
                 const component::EditMap::ActionCb& syncCb, core::system::Systems& s)
: map(component::EditMap::create(
      cb, std::bind(&MapArea::onMouseOver, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&MapArea::refreshButtons, this), syncCb, s)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 0));
    content->setOutlineThickness(0.f);

    Box::Ptr controlRow = Box::create(LinePacker::create(LinePacker::Horizontal, 0));
    controlRow->setOutlineThickness(0.f);

    Box::Ptr leftSide = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    undoBut           = Button::create("Undo");
    undoBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        map->undo();
    });
    undoBut->setActive(false);
    redoBut = Button::create("Redo");
    redoBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        map->redo();
    });
    redoBut->setActive(false);
    CheckButton::Ptr gbut = CheckButton::create("Grid");
    gbut->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event& e, Element*) {
        map->showGrid(e.toggleValue());
    });
    leftSide->pack(undoBut, false, true);
    leftSide->pack(redoBut, false, true);
    leftSide->pack(gbut, false, true);

    Box::Ptr rightSide = Box::create(
        LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Compact, LinePacker::RightAlign));
    enableBut = CheckButton::create("Enable Map Controls");
    enableBut->setTooltip("Enable or disable map editing. Useful for not making accidental edits");
    enableBut->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event& a, Element*) {
        map->setControlsEnabled(a.toggleValue());
    });
    positionLabel = Label::create("Tile: ()");
    rightSide->pack(enableBut, false, true);
    rightSide->pack(positionLabel, false, true);

    controlRow->pack(leftSide, true, false);
    controlRow->pack(rightSide, true, false);
    content->pack(controlRow, true, false);
    content->pack(map, true, true);
}

component::EditMap& MapArea::editMap() { return *map; }

bl::gui::Element::Ptr MapArea::getContent() { return content; }

void MapArea::refreshButtons() {
    const char* undoDesc = map->undoDescription();
    if (undoDesc) {
        undoBut->setTooltip(std::string("Undo ") + undoDesc);
        undoBut->setActive(true);
    }
    else {
        undoBut->setTooltip("");
        undoBut->setActive(false);
    }

    const char* redoDesc = map->redoDescription();
    if (redoDesc) {
        redoBut->setTooltip(std::string("Redo ") + redoDesc);
        redoBut->setActive(true);
    }
    else {
        redoBut->setTooltip("");
        redoBut->setActive(false);
    }
}

void MapArea::onMouseOver(const sf::Vector2f&, const sf::Vector2i& tiles) {
    positionLabel->setText("Tile: (" + std::to_string(tiles.x) + ", " + std::to_string(tiles.y) +
                           ")");
}

void MapArea::enableControls() {
    editMap().setControlsEnabled(true);
    enableBut->setValue(true);
}

void MapArea::disableControls() {
    editMap().setControlsEnabled(false);
    enableBut->setValue(false);
}

} // namespace page
} // namespace editor
