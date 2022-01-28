#include <Editor/Pages/Subpages/Towns.hpp>

#include <Editor/Pages/Subpages/Catchables.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Towns::Towns(component::EditMap& m)
: map(m)
, active(0)
, playlistWindow(std::bind(&Towns::onPlaylistPick, this, std::placeholders::_1),
                 std::bind(&Towns::takeFocus, this)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 6.f));

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    noTownBut    = RadioButton::create("No town", "none");
    noTownBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        active = 0;
    });
    row->pack(noTownBut, false, true);

    Button::Ptr but = Button::create("New Town");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Towns::newTown, this));
    row->pack(but, false, true);
    content->pack(row, true, false);

    scrollRegion = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    scrollRegion->setColor(sf::Color::Transparent, sf::Color::Black);
    scrollRegion->setOutlineThickness(1.5f);
    content->pack(scrollRegion, true, true);

    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Edit Town");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&Towns::closeWindow, this));

    row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    nameEntry = TextEntry::create();
    nameEntry->setRequisition({80.f, 15.f});
    row->pack(Label::create("Name:"));
    row->pack(nameEntry, true, false);
    window->pack(row, true, false);

    playlistLabel = Label::create("");
    playlistLabel->setColor(sf::Color(20, 230, 255), sf::Color::Transparent);
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    but = Button::create("Pick Playlist");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        playlistWindow.open(gui, playlistLabel->getText());
    });
    row->pack(but, false, true);
    row->pack(playlistLabel, true, false);
    window->pack(row, true, false);

    row           = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    weatherSelect = component::WeatherSelect::create();
    row->pack(Label::create("Weather:"));
    row->pack(weatherSelect);
    window->pack(row, true, false);

    but = Button::create("Save");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Towns::onTownEdit, this));
    window->pack(but);
}

Element::Ptr Towns::getContent() { return content; }

std::uint8_t Towns::selected() const { return active; }

void Towns::setGUI(const GUI::Ptr& g) { gui = g; }

sf::Color Towns::getColor(std::uint8_t i) { return Catchables::getColor(i); }

void Towns::refresh() {
    scrollRegion->clearChildren(true);
    active = 0;
    noTownBut->setValue(true);

    for (unsigned int i = 0; i < map.towns.size(); ++i) {
        scrollRegion->pack(makeRow(i, map.towns[i].name), true, false);
    }
}

void Towns::onPlaylistPick(const std::string& p) {
    playlistLabel->setText(p);
    window->setForceFocus(true);
}

void Towns::takeFocus() { window->setForceFocus(true); }

void Towns::closeWindow() {
    window->setForceFocus(false);
    window->remove();
}

void Towns::newTown() { map.addTown(); }

void Towns::editTown(std::uint8_t i) {
    editing = i;
    nameEntry->setInput(map.towns[i].name);
    playlistLabel->setText(map.towns[i].playlist);
    weatherSelect->setSelectedWeather(map.towns[i].weather);
    gui->pack(window);
    window->setForceFocus(true);
}

void Towns::removeTown(std::uint8_t i) { map.removeTown(i); }

void Towns::onTownEdit() {
    core::map::Town t;
    t.name     = nameEntry->getInput();
    t.playlist = playlistLabel->getText();
    t.weather  = weatherSelect->selectedWeather();
    map.editTown(editing, t);
    closeWindow();
}

Box::Ptr Towns::makeRow(std::uint8_t i, const std::string& name) {
    Box::Ptr row = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    row->setColor(getColor(i + 1), sf::Color::Black);

    RadioButton::Ptr but = RadioButton::create(name, name, noTownBut->getRadioGroup());
    but->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
        active = i+1;
    });
    but->setHorizontalAlignment(RenderSettings::Left);

    Button::Ptr edit = Button::create("Edit");
    edit->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Towns::editTown, this, i));

    Button::Ptr del = Button::create("Remove");
    del->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Towns::removeTown, this, i));
    del->setColor(sf::Color::Red, sf::Color::Black);

    row->pack(del, false, true);
    row->pack(edit, false, true);
    row->pack(but, true, true);

    return row;
}

} // namespace page
} // namespace editor
