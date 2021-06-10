#include <Editor/Pages/Map.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Map::Map(core::system::Systems& s)
: Page(s)
, levelPage(Layers::Level)
, layerPage(Layers::Layer)
, mapPicker(core::Properties::MapPath(), {"map", "p3m"},
            std::bind(&Map::doLoadMap, this, std::placeholders::_1),
            [this]() { mapPicker.close(); }) {
    content = Box::create(LinePacker::create(LinePacker::Horizontal, 4), "maps");
    bl::gui::Box::Ptr controlPane =
        Box::create(LinePacker::create(LinePacker::Vertical, 4), "maps");

    bl::gui::Box::Ptr mapCtrlBox   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr newMapBut = Button::create("New Map");
    newMapBut->getSignal(Action::LeftClicked).willCall([this](const Action&, Element*) {
        makingNewMap = true;
        mapPicker.open(FilePicker::CreateNew, "New map", parent);
    });
    bl::gui::Button::Ptr loadMapBut = Button::create("Load Map");
    loadMapBut->getSignal(Action::LeftClicked).willCall([this](const Action&, Element*) {
        makingNewMap = false;
        mapPicker.open(FilePicker::PickExisting, "Load map", parent);
    });
    bl::gui::Button::Ptr saveMapBut = Button::create("Save Map");
    mapCtrlBox->pack(newMapBut);
    mapCtrlBox->pack(loadMapBut);
    mapCtrlBox->pack(saveMapBut);

    bl::gui::Box::Ptr tileBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Uniform));

    levelSelect = ComboBox::create("maps");
    levelSelect->addOption("Level 0");
    levelSelect->addOption("Level 1");
    levelSelect->setSelectedOption(0);
    box->pack(levelSelect, true, true);

    layerSelect = ComboBox::create("maps");
    layerSelect->addOption("Layer 0");
    layerSelect->addOption("Layer 1");
    layerSelect->addOption("Layer 2 (ysort)");
    layerSelect->addOption("Layer 3");
    layerSelect->addOption("Layer 4 (top)");
    layerSelect->setSelectedOption(0);
    box->pack(layerSelect, true, true);
    tileBox->pack(box, true, false);

    box = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::RadioButton::Ptr tileSetBut = RadioButton::create("Set");
    tileSetBut->setValue(true);
    bl::gui::RadioButton::Ptr tileClearBut =
        RadioButton::create("Clear", tileSetBut->getRadioGroup());
    bl::gui::RadioButton::Ptr tileSelectBut =
        RadioButton::create("Select", tileSetBut->getRadioGroup());
    bl::gui::Button::Ptr tileDeselectBut = Button::create("Deselect");
    box->pack(tileSetBut, true, true);
    box->pack(tileClearBut, true, true);
    box->pack(tileSelectBut, true, true);
    box->pack(tileDeselectBut, true, true);
    tileBox->pack(box, true, false);

    bl::gui::Box::Ptr infoBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));

    box       = Box::create(LinePacker::create(LinePacker::Horizontal));
    nameEntry = TextEntry::create(1);
    box->pack(Label::create("Name:"), false, true);
    box->pack(nameEntry, true, true);
    row->pack(box, true, false);

    bl::gui::Button::Ptr resizeBut = Button::create("Resize Map");
    row->pack(resizeBut);
    infoBox->pack(row, true, false);

    row                                  = Box::create(LinePacker::create(LinePacker::Horizontal));
    playlistLabel                        = Label::create("playerlistFile.bplst");
    bl::gui::Button::Ptr pickPlaylistBut = Button::create("Pick Playlist");
    playlistLabel->setHorizontalAlignment(RenderSettings::Left);
    row->pack(pickPlaylistBut);
    row->pack(playlistLabel, true, false);
    infoBox->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    row->pack(Label::create("Weather:"));
    weatherEntry = ComboBox::create();
    weatherEntry->addOption("None");
    weatherEntry->addOption("AllRandom");
    weatherEntry->addOption("LightRain");
    weatherEntry->addOption("LightRainThunder");
    weatherEntry->addOption("HardRain");
    weatherEntry->addOption("HardRainThunder");
    weatherEntry->addOption("LightSnow");
    weatherEntry->addOption("LightSnowThunder");
    weatherEntry->addOption("HardSnow");
    weatherEntry->addOption("HardSnowThunder");
    weatherEntry->addOption("ThinFog");
    weatherEntry->addOption("ThickFog");
    weatherEntry->addOption("Sunny");
    weatherEntry->addOption("SandStorm");
    weatherEntry->addOption("WaterRandom");
    weatherEntry->addOption("SnowRandom");
    weatherEntry->addOption("DesertRandom");
    weatherEntry->setSelectedOption(0);
    weatherEntry->setMaxHeight(300);
    row->pack(weatherEntry);
    infoBox->pack(row, true, false);

    bl::gui::Notebook::Ptr editBook = Notebook::create("maps");
    editBook->addPage("tiles", "Tiles", tileBox);
    editBook->addPage(
        "layers",
        "Layers",
        layerPage.getContent(),
        [this]() { layerPage.pack(); },
        [this]() { layerPage.unpack(); });
    editBook->addPage(
        "levels",
        "Levels",
        levelPage.getContent(),
        [this]() { levelPage.pack(); },
        [this]() { levelPage.unpack(); });

    Box::Ptr spawnBox            = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box                          = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    RadioButton::Ptr spawnCreate = RadioButton::create("Spawn");
    spawnCreate->setValue(true);
    spawnDirEntry = ComboBox::create();
    spawnDirEntry->addOption("Up");
    spawnDirEntry->addOption("Right");
    spawnDirEntry->addOption("Down");
    spawnDirEntry->addOption("Left");
    spawnDirEntry->setSelectedOption(0);
    RadioButton::Ptr spawnRotate = RadioButton::create("Edit", spawnCreate->getRadioGroup());
    Label::Ptr label             = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr spawnDelete = RadioButton::create(label, spawnCreate->getRadioGroup());
    box->pack(spawnCreate);
    box->pack(spawnDirEntry);
    spawnBox->pack(box);
    box = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    box->pack(spawnRotate);
    box->pack(spawnDelete);
    spawnBox->pack(box);

    Box::Ptr npcBox           = Box::create(LinePacker::create(LinePacker::Horizontal, 8));
    RadioButton::Ptr npcSpawn = RadioButton::create("Spawn");
    RadioButton::Ptr npcEdit  = RadioButton::create("Edit", npcSpawn->getRadioGroup());
    label                     = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr npcDelete = RadioButton::create(label, npcSpawn->getRadioGroup());
    npcSpawn->setValue(true);
    npcBox->pack(npcSpawn);
    npcBox->pack(npcEdit);
    npcBox->pack(npcDelete);

    Box::Ptr itemBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    itemSpawnEntry   = ComboBox::create();
    itemIdLookup.reserve(core::item::Item::validIds().size());
    for (const core::item::Id item : core::item::Item::validIds()) {
        itemSpawnEntry->addOption(core::item::Item::getName(item));
        itemIdLookup.push_back(item);
    }
    itemSpawnEntry->setSelectedOption(0);
    RadioButton::Ptr itemSpawn = RadioButton::create("Spawn");
    itemSpawn->setValue(true);
    label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr itemDelete = RadioButton::create(label, itemSpawn->getRadioGroup());
    box->pack(itemSpawn);
    box->pack(itemSpawnEntry, true, false);
    itemBox->pack(box, true, false);
    itemBox->pack(itemDelete);

    Box::Ptr lightBox            = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box                          = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    RadioButton::Ptr lightCreate = RadioButton::create("Create/Modify");
    lightCreate->setValue(true);
    lightRadiusEntry = TextEntry::create();
    lightRadiusEntry->setRequisition({80, 0});
    lightRadiusEntry->setInput("100");
    box->pack(lightCreate, false, false);
    box->pack(Label::create("Radius (pixels):"), false, false);
    box->pack(lightRadiusEntry, true, false);
    lightBox->pack(box, true, false);
    label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr lightDelete = RadioButton::create(label, lightCreate->getRadioGroup());
    lightBox->pack(lightDelete);

    bl::gui::Notebook::Ptr objectBook = Notebook::create("maps");
    objectBook->addPage("spawns", "Spawns", spawnBox);
    objectBook->addPage("ai", "NPC's", npcBox);
    objectBook->addPage("items", "Items", itemBox);
    objectBook->addPage("lights", "Lights", lightBox);

    Box::Ptr eventBox = Box::create(LinePacker::create(LinePacker::Horizontal, 0));
    box               = Box::create(LinePacker::create(LinePacker::Vertical, 6));
    row               = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Enter:"));
    onEnterLabel = Label::create("onEnter.bs");
    onEnterLabel->setColor(sf::Color(20, 20, 220), sf::Color::Transparent);
    row->pack(onEnterLabel, true, false);
    box->pack(row, true, false);
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Exit:"));
    onExitLabel = Label::create("onExit.bs");
    onExitLabel->setColor(sf::Color(20, 20, 220), sf::Color::Transparent);
    row->pack(onExitLabel, true, false);
    box->pack(row, true, false);
    row                    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickButton = Button::create("Set OnEnter");
    row->pack(pickButton);
    pickButton = Button::create("Set OnExit");
    row->pack(pickButton);
    box->pack(row, true, false);
    eventBox->pack(box, true, true);
    box                          = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    RadioButton::Ptr createEvent = RadioButton::create("Create Event");
    createEvent->setValue(true);
    label = Label::create("Delete Event");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    box->pack(createEvent);
    box->pack(RadioButton::create("Edit Event", createEvent->getRadioGroup()));
    box->pack(RadioButton::create(label, createEvent->getRadioGroup()));
    eventBox->pack(Separator::create(Separator::Vertical));
    eventBox->pack(box, false, true);

    Box::Ptr peoplemonBox       = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    RadioButton::Ptr createZone = RadioButton::create("Create Catch Zone");
    RadioButton::Ptr editZone = RadioButton::create("Edit Catch Zone", createZone->getRadioGroup());
    label                     = Label::create("Delete Catch Zone");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr deleteZone = RadioButton::create(label, createZone->getRadioGroup());
    peoplemonBox->pack(createZone);
    peoplemonBox->pack(editZone);
    peoplemonBox->pack(deleteZone);

    const auto editClosed = [this]() {
        layerPage.unpack();
        levelPage.unpack();
    };
    const auto editOpened = [this, editBook]() {
        if (editBook->getActivePageName() == "layers")
            layerPage.pack();
        else if (editBook->getActivePageName() == "levels")
            levelPage.pack();
    };

    bl::gui::Notebook::Ptr controlBook = Notebook::create("maps");
    controlBook->addPage("map", "Map", infoBox);
    controlBook->addPage("edit", "Edit", editBook, editOpened, editClosed);
    controlBook->addPage("obj", "Objects", objectBook);
    controlBook->addPage("events", "Scripts", eventBox);
    controlBook->addPage("ppl", "Peoplemon", peoplemonBox);

    controlPane->pack(mapCtrlBox, true, false);
    controlPane->pack(controlBook, true, false);
    controlPane->pack(tileset.getContent(), true, true);

    content->pack(controlPane, false, true);
    content->pack(Label::create("Map canvas here"), true, true);
}

void Map::update(float dt) {
    // TODO
}

void Map::doLoadMap(const std::string& file) {
    BL_LOG_INFO << "New/Load map: " << file;
    mapPicker.close();

    if (makingNewMap) {
        // TODO - new map
    }
    else {
        // TODO - load map
    }
}

} // namespace page
} // namespace editor
