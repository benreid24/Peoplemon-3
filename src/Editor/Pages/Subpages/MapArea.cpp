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

    Box::Ptr controlRow = Box::create(LinePacker::create(LinePacker::Horizontal, 0));

    Box::Ptr leftSide = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    undoText          = Label::create("Undo");
    undoBut           = Button::create(undoText);
    undoBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        map->undo();
    });
    undoBut->setActive(false);
    redoText = Label::create("Redo");
    redoBut  = Button::create(redoText);
    redoBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        map->redo();
    });
    redoBut->setActive(false);
    leftSide->pack(undoBut, false, true);
    leftSide->pack(redoBut, false, true);

    Box::Ptr rightSide = Box::create(
        LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Compact, LinePacker::RightAlign));
    CheckButton::Ptr enableBut = CheckButton::create("Enable Map Controls");
    enableBut->getSignal(Action::ValueChanged).willAlwaysCall([this](const Action& a, Element*) {
        map->setControlsEnabled(a.data.bvalue);
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
        undoText->setText(std::string("Undo ") + undoDesc);
        undoBut->setActive(true);
    }
    else {
        undoText->setText("Undo");
        undoBut->setActive(false);
    }

    const char* redoDesc = map->redoDescription();
    if (redoDesc) {
        redoText->setText(std::string("Redo ") + redoDesc);
        redoBut->setActive(true);
    }
    else {
        redoText->setText("Redo");
        redoBut->setActive(false);
    }
}

void MapArea::onMouseOver(const sf::Vector2f&, const sf::Vector2i& tiles) {
    positionLabel->setText("Tile: (" + std::to_string(tiles.x) + ", " + std::to_string(tiles.y) +
                           ")");
}

} // namespace page
} // namespace editor
