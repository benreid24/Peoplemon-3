#include <Editor/Components/NewMapDialog.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

NewMapDialog::NewMapDialog(GUI::Ptr p, const CreateCb& cb)
: createCb(cb)
, parent(p)
, tilesetPicker(
      core::Properties::TilesetPath(), {"tlst"},
      [this](const std::string& tileset) { tilesetLabel->setText(tileset); },
      [this]() { tilesetPicker.close(); }) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "New map");

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    row->pack(Label::create("Map file:"), false, true);
    mapFileLabel = Label::create("map file");
    mapFileLabel->setColor(sf::Color(15, 100, 245), sf::Color::Transparent);
    row->pack(mapFileLabel, false, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    row->pack(Label::create("Name:"), false, true);
    nameEntry = TextEntry::create();
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Width:"), false, true);
    widthEntry = TextEntry::create();
    widthEntry->setRequisition({100, 0});
    row->pack(widthEntry, false, true);
    row->pack(Label::create("Height:"), false, true);
    heightEntry = TextEntry::create();
    heightEntry->setRequisition({100, 0});
    row->pack(heightEntry, false, true);
    window->pack(row);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickBut = Button::create("Pick Tileset");
    pickBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        tilesetPicker.open(FilePicker::CreateNew, "Pick tileset", parent, true);
    });
    row->pack(pickBut, false, true);
    tilesetLabel = Label::create("tileset file");
    row->pack(tilesetLabel, false, true);
    window->pack(row, true, false);

    Button::Ptr createBut = Button::create("Create Map");
    createBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO - validate input and call cb
    });
    window->pack(createBut);

    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
}

void NewMapDialog::show(const std::string& file) {
    // TODO - reset inputs and pack window
}

} // namespace component
} // namespace editor
