#include <Editor/Pages/Subpages/Towns.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Editor/Pages/Subpages/Catchables.hpp>
#include <Editor/Pages/Subpages/MapArea.hpp>

namespace editor
{
namespace page
{
namespace
{
constexpr std::uint16_t NoSpawn = std::numeric_limits<std::uint16_t>::max();
constexpr float FlymapScale     = 0.5f;
} // namespace
using namespace bl::gui;

Towns::Towns(MapArea& ma)
: mapArea(ma)
, map(ma.editMap())
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
    nameEntry->setRequisition({100.f, 15.f});
    row->pack(Label::create("Name:"));
    row->pack(nameEntry, false, false);
    window->pack(row, true, false);

    row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    descEntry = TextEntry::create();
    descEntry->setRequisition({200.f, 15.f});
    row->pack(Label::create("Description:"));
    row->pack(descEntry, true, false);
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

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("PC Spawn:"), false, true);
    spawnSelect = ComboBox::create();
    spawnSelect->setMaxHeight(150.f);
    row->pack(spawnSelect, false, true);
    window->pack(row, true, false);

    flymapTxtr = TextureManager::load(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "FlyMap/background.png"));
    flyMap.setTexture(*flymapTxtr, true);
    flyMap.setScale(FlymapScale, FlymapScale);
    window->pack(Label::create("Fly map position:"));
    mapPosCanvas = Canvas::create(flymapTxtr->getSize().x / 2, flymapTxtr->getSize().y / 2);
    mapPosCanvas->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Towns::setMapPos, this, std::placeholders::_1));
    window->pack(mapPosCanvas, false, false);

    but = Button::create("Save");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Towns::onTownEdit, this));
    window->pack(but);
}

Element::Ptr Towns::getContent() { return content; }

std::uint8_t Towns::selected() const { return active; }

void Towns::setGUI(GUI* g) { gui = g; }

sf::Color Towns::getColor(std::uint8_t i) { return Catchables::getColor(i); }

void Towns::refresh() {
    scrollRegion->clearChildren(true);
    active = 0;
    noTownBut->setValue(true);

    for (unsigned int i = 0; i < map.towns.size(); ++i) {
        scrollRegion->pack(makeRow(i, map.towns[i].name), true, false);
    }

    refreshSpawns(NoSpawn);
}

void Towns::refreshSpawns(std::uint16_t s) {
    spawnSelect->clearOptions();
    spawnSelect->addOption("None");

    std::vector<std::uint16_t> spawns;
    spawns.reserve(map.spawns.size());
    for (const auto& spawn : map.spawns) { spawns.emplace_back(spawn.first); }
    std::sort(spawns.begin(), spawns.end());

    int opt = 0;
    for (unsigned int i = 0; i < spawns.size(); ++i) {
        spawnSelect->addOption(std::to_string(spawns[i]));
        if (spawns[i] == s) { opt = i + 1; }
    }
    spawnSelect->setSelectedOption(opt);
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
    descEntry->setInput(map.towns[i].description);
    playlistLabel->setText(map.towns[i].playlist);
    weatherSelect->setSelectedWeather(map.towns[i].weather);
    mapPos = map.towns[i].mapPos;
    refreshFlymapCanvas();
    refreshSpawns(map.towns[i].pcSpawn);
    mapArea.disableControls();
    gui->pack(window);
    window->setForceFocus(true);
}

void Towns::removeTown(std::uint8_t i) { map.removeTown(i); }

void Towns::onTownEdit() {
    core::map::Town t;
    t.name        = nameEntry->getInput();
    t.description = descEntry->getInput();
    t.playlist    = playlistLabel->getText();
    t.weather     = weatherSelect->selectedWeather();
    t.mapPos      = mapPos;
    if (spawnSelect->getSelectedOptionText() == "None") { t.pcSpawn = NoSpawn; }
    else { t.pcSpawn = std::atoi(spawnSelect->getSelectedOptionText().c_str()); }
    map.editTown(editing, t);
    closeWindow();
}

Box::Ptr Towns::makeRow(std::uint8_t i, const std::string& name) {
    Box::Ptr row = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    row->setColor(getColor(i + 1), sf::Color::Black);

    RadioButton::Ptr but = RadioButton::create(name, name, noTownBut->getRadioGroup());
    but->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
        active = i + 1;
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

void Towns::setMapPos(const Event& click) {
    const sf::Vector2f localPos =
        (click.mousePosition() - mapPosCanvas->getPosition()) / FlymapScale;
    mapPos = sf::Vector2i(localPos);
    refreshFlymapCanvas();
}

void Towns::refreshFlymapCanvas() {
    // TODO - BLIB_UPGRADE - update town flymap indicator rendering
    /* mapPosCanvas->getTexture().draw(flyMap);
     sf::CircleShape dot(5.f);
     dot.setPosition(sf::Vector2f(mapPos) * FlymapScale);
     dot.setOrigin({5.f, 5.f});
     dot.setFillColor(sf::Color::Red);
     dot.setOutlineColor(sf::Color::Black);
     dot.setOutlineThickness(1.f);
     mapPosCanvas->getTexture().draw(dot);
     mapPosCanvas->getTexture().display();*/
}

} // namespace page
} // namespace editor
