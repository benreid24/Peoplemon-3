#include <Editor/Components/NewMapDialog.hpp>

#include <BLIB/Interfaces/GUI/Dialogs/tinyfiledialogs.hpp>
#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
namespace
{
bool isnum(const std::string& s) {
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}
} // namespace
using namespace bl::gui;

NewMapDialog::NewMapDialog(const CreateCb& cb)
: createCb(cb)
, tilesetPicker(
      core::Properties::TilesetPath(), {"tlst"},
      [this](const std::string& tileset) {
          tilesetLabel->setText(tileset);
          tilesetPicker.close();
      },
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
    nameEntry->setRequisition({250, 10});
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Width:"), false, true);
    widthEntry = TextEntry::create();
    widthEntry->setRequisition({100, 10});
    row->pack(widthEntry, false, true);
    row->pack(Label::create("Height:"), false, true);
    heightEntry = TextEntry::create();
    heightEntry->setRequisition({100, 10});
    row->pack(heightEntry, false, true);
    window->pack(row);

    row     = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    pickBut = Button::create("Pick Tileset");
    row->pack(pickBut, false, true);
    tilesetLabel = Label::create("tileset file");
    tilesetLabel->setColor(sf::Color(30, 245, 85), sf::Color::Transparent);
    row->pack(tilesetLabel, false, true);
    window->pack(row, true, false);

    Button::Ptr createBut = Button::create("Create Map");
    createBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (nameEntry->getInput().empty()) {
            bl::dialog::tinyfd_messageBox("Error", "Please enter a map name", "ok", "error", 1);
            return;
        }
        if (widthEntry->getInput().empty()) {
            bl::dialog::tinyfd_messageBox("Error", "Please enter a width", "ok", "error", 1);
            return;
        }
        if (heightEntry->getInput().empty()) {
            bl::dialog::tinyfd_messageBox("Error", "Please enter a height", "ok", "error", 1);
            return;
        }
        if (!isnum(widthEntry->getInput())) {
            bl::dialog::tinyfd_messageBox(
                "Error", "Width must be a positive integer", "ok", "error", 1);
            return;
        }
        if (!isnum(heightEntry->getInput())) {
            bl::dialog::tinyfd_messageBox(
                "Error", "Height must be a positive integer", "ok", "error", 1);
            return;
        }
        if (tilesetLabel->getText().empty()) {
            bl::dialog::tinyfd_messageBox("Error", "Please pick a tileset", "ok", "error", 1);
            return;
        }

        window->remove();
        createCb(mapFileLabel->getText(),
                 nameEntry->getInput(),
                 tilesetLabel->getText(),
                 std::atoi(widthEntry->getInput().c_str()),
                 std::atoi(heightEntry->getInput().c_str()));
    });
    window->pack(createBut);

    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        window->remove();
    });
}

void NewMapDialog::show(GUI::Ptr parent, const std::string& file) {
    mapFileLabel->setText(file);
    tilesetLabel->setText("");
    nameEntry->setInput("");
    widthEntry->setInput("");
    heightEntry->setInput("");

    if (!pickInit) {
        pickInit = true;
        pickBut->getSignal(Event::LeftClicked)
            .willAlwaysCall([this, parent](const Event&, Element*) {
                tilesetPicker.open(FilePicker::CreateOrPick, "Pick tileset", parent, true);
            });
    }

    parent->pack(window);
}

} // namespace component
} // namespace editor
