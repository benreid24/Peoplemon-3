#include <Editor/Pages/Map.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace page
{
namespace
{
bool isNum(const char* s) {
    for (unsigned int i = 0; i < strlen(s); ++i) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}
} // namespace

using namespace bl::gui;

Map::Map(core::system::Systems& s)
: Page(s)
, mapArea([this](const sf::Vector2f& p, const sf::Vector2i& t) { onMapClick(p, t); },
          std::bind(&Map::syncGui, this), s)
, tileset([this](core::map::Tile::IdType id, bool isAnim) {
    mapArea.editMap().removeAllTiles(id, isAnim);
})
, levelPage([this](unsigned int l, bool v) { mapArea.editMap().setLevelVisible(l, v); },
            [this](unsigned int l, bool up) { mapArea.editMap().shiftLevel(l, up); },
            [this]() { mapArea.editMap().appendLevel(); })
, layerPage([this](unsigned int l) { mapArea.editMap().appendBottomLayer(l); },
            [this](unsigned int l) { mapArea.editMap().appendYsortLayer(l); },
            [this](unsigned int l) { mapArea.editMap().appendTopLayer(l); },
            [this](unsigned int level, unsigned int layer) {
                mapArea.editMap().removeLayer(level, layer);
            },
            [this](unsigned int level, unsigned int layer, bool up) {
                mapArea.editMap().shiftLayer(level, layer, up);
            },
            [this](unsigned int level, unsigned int layer, bool visible) {
                mapArea.editMap().setLayerVisible(level, layer, visible);
            })
, activeTool(Tool::Metadata)
, activeSubtool(Subtool::Set)
, selectionState(NoSelection)
, mapPicker(core::Properties::MapPath(), {"map", "p3m"},
            std::bind(&Map::doLoadMap, this, std::placeholders::_1),
            [this]() { mapPicker.close(); })
, newMapWindow(std::bind(&Map::makeNewMap, this, std::placeholders::_1, std::placeholders::_2,
                         std::placeholders::_3, std::placeholders::_4, std::placeholders::_5))
, playlistPicker(core::Properties::PlaylistPath(), {"plst"},
                 std::bind(&Map::onChoosePlaylist, this, std::placeholders::_1),
                 [this]() { playlistPicker.close(); })
, scriptSelector(std::bind(&Map::onChooseScript, this, std::placeholders::_1), []() {})
, choosingOnloadScript(false)
, eventEditor(std::bind(&Map::onEventEdit, this, std::placeholders::_1, std::placeholders::_2))
, characterEditor(
      std::bind(&Map::onCharacterEdit, this, std::placeholders::_1, std::placeholders::_2)) {
    content = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    content->setOutlineThickness(0.f);
    Box::Ptr controlPane = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    Box::Ptr mapCtrlBox   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr newMapBut = Button::create("New Map");
    newMapBut->getSignal(Event::LeftClicked).willCall([this](const Event&, Element*) {
        if (checkUnsaved()) {
            makingNewMap = true;
            mapArea.disableControls();
            mapPicker.open(FilePicker::CreateNew, "New map", parent);
        }
    });
    Button::Ptr loadMapBut = Button::create("Load Map");
    loadMapBut->getSignal(Event::LeftClicked).willCall([this](const Event&, Element*) {
        if (checkUnsaved()) {
            makingNewMap = false;
            mapArea.disableControls();
            mapPicker.open(FilePicker::PickExisting, "Load map", parent);
        }
    });
    saveMapBut = Button::create("Save Map");
    saveMapBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (!mapArea.editMap().editorSave()) {
            bl::dialog::tinyfd_messageBox(
                "Error Saving Map",
                std::string("Failed to save map: " + mapArea.editMap().name()).c_str(),
                "ok",
                "error",
                1);
        }
        else {
            tileset.markSaved();
        }
    });
    mapCtrlBox->pack(newMapBut);
    mapCtrlBox->pack(loadMapBut);
    mapCtrlBox->pack(saveMapBut);

    Box::Ptr tileBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Uniform));

    levelSelect = ComboBox::create();
    levelSelect->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element* e) {
        onLevelChange(dynamic_cast<ComboBox*>(e)->getSelectedOption());
    });
    box->pack(levelSelect, true, true);

    layerSelect = ComboBox::create();
    box->pack(layerSelect, true, true);
    tileBox->pack(box, true, false);

    box                         = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    RadioButton::Ptr tileSetBut = RadioButton::create("Set", "set");
    tileSetBut->setValue(true);
    tileSetBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        activeSubtool = Subtool::Set;
    });
    RadioButton::Ptr tileClearBut =
        RadioButton::create("Clear", "clear", tileSetBut->getRadioGroup());
    tileClearBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        activeSubtool = Subtool::Clear;
    });
    RadioButton::Ptr tileSelectBut =
        RadioButton::create("Select", "select", tileSetBut->getRadioGroup());
    tileSelectBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        activeSubtool = Subtool::Select;
    });
    Button::Ptr tileDeselectBut = Button::create("Deselect");
    tileDeselectBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        selectionState = NoSelection;
    });
    Button::Ptr selectAllBut = Button::create("All");
    selectAllBut->setTooltip("Select all tiles");
    selectAllBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        selectionState = SelectionMade;
        selection      = {sf::Vector2i(0, 0), mapArea.editMap().sizeTiles()};
    });
    box->pack(tileSetBut, true, true);
    box->pack(tileClearBut, true, true);
    box->pack(tileSelectBut, true, true);
    box->pack(selectAllBut, true, true);
    box->pack(tileDeselectBut, true, true);
    tileBox->pack(box, true, false);

    Box::Ptr infoBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr row     = Box::create(LinePacker::create(LinePacker::Horizontal, 4));

    box       = Box::create(LinePacker::create(LinePacker::Horizontal));
    nameEntry = TextEntry::create(1);
    nameEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        mapArea.editMap().setName(nameEntry->getInput());
    });
    box->pack(Label::create("Name:"), false, true);
    box->pack(nameEntry, true, true);
    row->pack(box, true, false);

    Button::Ptr resizeBut = Button::create("Resize Map");
    // TODO - implement map resizing
    row->pack(resizeBut);
    infoBox->pack(row, true, false);

    row                         = Box::create(LinePacker::create(LinePacker::Horizontal));
    playlistLabel               = Label::create("playerlistFile.bplst");
    Button::Ptr pickPlaylistBut = Button::create("Pick Playlist");
    pickPlaylistBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        mapArea.disableControls();
        playlistPicker.open(FilePicker::PickExisting, "Select Playlist", parent);
    });
    playlistLabel->setHorizontalAlignment(RenderSettings::Left);
    row->pack(pickPlaylistBut);
    row->pack(playlistLabel, true, false);
    infoBox->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    row->pack(Label::create("Weather:"));
    weatherEntry = ComboBox::create();
    weatherEntry->setTooltip("Set the weather for the entire map");
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
    weatherEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        const core::map::Weather::Type type =
            static_cast<core::map::Weather::Type>(weatherEntry->getSelectedOption());
        if (mapArea.editMap().weatherSystem().getType() != type) {
            mapArea.editMap().setWeather(type);
        }
    });
    row->pack(weatherEntry);
    infoBox->pack(row, true, false);

    box              = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    tempWeatherEntry = ComboBox::create();
    tempWeatherEntry->addOption("None");
    tempWeatherEntry->addOption("AllRandom");
    tempWeatherEntry->addOption("LightRain");
    tempWeatherEntry->addOption("LightRainThunder");
    tempWeatherEntry->addOption("HardRain");
    tempWeatherEntry->addOption("HardRainThunder");
    tempWeatherEntry->addOption("LightSnow");
    tempWeatherEntry->addOption("LightSnowThunder");
    tempWeatherEntry->addOption("HardSnow");
    tempWeatherEntry->addOption("HardSnowThunder");
    tempWeatherEntry->addOption("ThinFog");
    tempWeatherEntry->addOption("ThickFog");
    tempWeatherEntry->addOption("Sunny");
    tempWeatherEntry->addOption("SandStorm");
    tempWeatherEntry->addOption("WaterRandom");
    tempWeatherEntry->addOption("SnowRandom");
    tempWeatherEntry->addOption("DesertRandom");
    tempWeatherEntry->setSelectedOption(0);
    tempWeatherEntry->setMaxHeight(300);
    Button::Ptr but = Button::create("Set Weather");
    but->setTooltip("Sets the current weather. Does not save weather to map file");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const core::map::Weather::Type type =
            static_cast<core::map::Weather::Type>(tempWeatherEntry->getSelectedOption());
        if (mapArea.editMap().weatherSystem().getType() != type) {
            mapArea.editMap().weatherSystem().set(type);
        }
    });
    row->pack(tempWeatherEntry, false, true);
    row->pack(but, false, true);
    box->pack(row, true, false);

    row          = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    timeSetEntry = ComboBox::create();
    timeSetEntry->addOption("Morning");
    timeSetEntry->addOption("Noon");
    timeSetEntry->addOption("Evening");
    timeSetEntry->addOption("Midnight");
    timeSetEntry->setSelectedOption(0);
    but = Button::create("Set Time");
    but->setTooltip("Sets the current time. Does not save to map file");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        switch (timeSetEntry->getSelectedOption()) {
        case 0:
            systems.clock().set({6, 0}); // 6 am
            break;
        case 1:
            systems.clock().set({12, 0}); // noon
            break;
        case 2:
            systems.clock().set({18, 0}); // 6 pm
            break;
        case 3:
        default:
            systems.clock().set({0, 0}); // midnight
            break;
        }
    });
    row->pack(timeSetEntry, false, true);
    row->pack(but, false, true);
    box->pack(row, true, false);

    Notebook::Ptr editBook = Notebook::create();
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
    editBook->addPage("state", "State", box);

    Box::Ptr spawnBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box               = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    spawnCreate       = RadioButton::create("Spawn", "spawn");
    spawnCreate->setTooltip("Create player spawns for when entering the map");
    spawnCreate->setValue(true);
    spawnDirEntry = ComboBox::create();
    spawnDirEntry->addOption("Up");
    spawnDirEntry->addOption("Right");
    spawnDirEntry->addOption("Down");
    spawnDirEntry->addOption("Left");
    spawnDirEntry->setSelectedOption(0);
    spawnRotate      = RadioButton::create("Rotate", "rotate", spawnCreate->getRadioGroup());
    Label::Ptr label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    spawnDelete = RadioButton::create(label, "delete", spawnCreate->getRadioGroup());
    box->pack(spawnCreate);
    box->pack(spawnDirEntry);
    spawnBox->pack(box);
    box = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    box->pack(spawnRotate);
    box->pack(spawnDelete);
    spawnBox->pack(box);

    Box::Ptr npcBox = Box::create(LinePacker::create(LinePacker::Vertical, 8));
    npcSpawn        = RadioButton::create("Spawn", "spawn");
    npcEdit         = RadioButton::create("Edit", "edit", npcSpawn->getRadioGroup());
    label           = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    npcDelete = RadioButton::create(label, "delete", npcSpawn->getRadioGroup());
    npcSpawn->setValue(true);
    npcBox->pack(npcSpawn);
    npcBox->pack(npcEdit);
    npcBox->pack(npcDelete);

    Box::Ptr itemBox = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    box              = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    itemSelector     = component::ItemSelector::create();
    itemSpawn        = RadioButton::create("Spawn/Edit", "spawn");
    itemSpawn->setValue(true);
    itemRead = RadioButton::create("Read", "read", itemSpawn->getRadioGroup());
    itemRead->setTooltip("Populate this form with details from an item in the map");
    itemHidden = CheckButton::create("Hidden item");
    itemHidden->setTooltip("Check this to make the item invisible");
    label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    itemDelete = RadioButton::create(label, "delete", itemSpawn->getRadioGroup());
    box->pack(itemSpawn);
    box->pack(itemRead);
    box->pack(itemDelete);
    itemBox->pack(box, true, false);
    box = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box->pack(itemSelector, true, false);
    box->pack(itemHidden);
    itemBox->pack(box, true, true);

    Box::Ptr lightBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box               = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    lightCreateOrEdit = RadioButton::create("Create/Modify", "create");
    lightCreateOrEdit->setValue(true);
    lightRadiusEntry = TextEntry::create();
    lightRadiusEntry->setMode(TextEntry::Mode::Integer);
    lightRadiusEntry->setRequisition({80, 0});
    lightRadiusEntry->setInput("100");
    box->pack(lightCreateOrEdit, false, false);
    box->pack(Label::create("Radius (pixels):"), false, false);
    box->pack(lightRadiusEntry, true, false);
    lightBox->pack(box, true, false);
    label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    lightRemove = RadioButton::create(label, "delete", lightCreateOrEdit->getRadioGroup());
    lightBox->pack(lightRemove);

    objectBook = Notebook::create();
    objectBook->addPage("spawns", "Spawns", spawnBox, [this]() { activeTool = Tool::Spawns; });
    objectBook->addPage("ai", "Characters", npcBox, [this]() { activeTool = Tool::NPCs; });
    objectBook->addPage("items", "Items", itemBox, [this]() { activeTool = Tool::Items; });
    objectBook->addPage("lights", "Lights", lightBox, [this]() { activeTool = Tool::Lights; });

    Box::Ptr eventBox      = Box::create(LinePacker::create(LinePacker::Horizontal, 2));
    ScrollArea::Ptr scroll = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 6));
    scroll->setMaxSize({320.f, 3000.f});
    scroll->setNeverShowVerticalScrollbar(true);
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Enter:"));
    onEnterLabel = Label::create("onEnter.bs");
    onEnterLabel->setColor(sf::Color(20, 20, 220), sf::Color::Transparent);
    row->pack(onEnterLabel, true, false);
    scroll->pack(row, true, false);
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Exit:"));
    onExitLabel = Label::create("onExit.bs");
    onExitLabel->setColor(sf::Color(20, 20, 220), sf::Color::Transparent);
    row->pack(onExitLabel, true, false);
    scroll->pack(row, true, false);
    row                    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickButton = Button::create("Set OnEnter");
    pickButton->setTooltip("Set the script that runs when the player enters the map");
    pickButton->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        choosingOnloadScript = true;
        mapArea.disableControls();
        scriptSelector.open(parent, mapArea.editMap().getOnEnterScript());
    });
    row->pack(pickButton);
    pickButton = Button::create("Set OnExit");
    pickButton->setTooltip("Set the script that runs when the player exits the map");
    pickButton->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        choosingOnloadScript = false;
        mapArea.disableControls();
        scriptSelector.open(parent, mapArea.editMap().getOnExitScript());
    });
    row->pack(pickButton);
    scroll->pack(row, true, false);
    eventBox->pack(scroll, true, true);
    box              = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    createEventRadio = RadioButton::create("Create Event", "create");
    createEventRadio->setValue(true);
    label = Label::create("Delete Event");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    box->pack(createEventRadio);
    editEventRadio = RadioButton::create("Edit Event", "edit", createEventRadio->getRadioGroup());
    box->pack(editEventRadio);
    deleteEventRadio = RadioButton::create(label, "delete", createEventRadio->getRadioGroup());
    box->pack(deleteEventRadio);
    eventBox->pack(Separator::create(Separator::Vertical));
    eventBox->pack(box, false, true);

    Box::Ptr peoplemonBox       = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    RadioButton::Ptr createZone = RadioButton::create("Create Catch Zone", "create");
    RadioButton::Ptr editZone =
        RadioButton::create("Edit Catch Zone", "edit", createZone->getRadioGroup());
    label = Label::create("Delete Catch Zone");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr deleteZone = RadioButton::create(label, "delete", createZone->getRadioGroup());
    peoplemonBox->pack(createZone);
    peoplemonBox->pack(editZone);
    peoplemonBox->pack(deleteZone);

    const auto editClosed = [this]() {
        layerPage.unpack();
        levelPage.unpack();
    };

    RadioButton::Group* tilesetButGroup = tileSetBut->getRadioGroup();
    const auto editOpened               = [this, editBook, tilesetButGroup]() {
        activeTool    = Tool::MapEdit;
        activeSubtool = Subtool::None;

        if (editBook->getActivePageName() == "tiles") {
            if (tilesetButGroup->getActiveButton()->getName() == "set") {
                activeSubtool = Subtool::Set;
            }
            else if (tilesetButGroup->getActiveButton()->getName() == "clear") {
                activeSubtool = Subtool::Clear;
            }
            else if (tilesetButGroup->getActiveButton()->getName() == "select") {
                activeSubtool = Subtool::Select;
            }
        }
        else if (editBook->getActivePageName() == "layers")
            layerPage.pack();
        else if (editBook->getActivePageName() == "levels")
            levelPage.pack();
    };

    const auto onObjectActive = [this]() {
        if (objectBook->getActivePageName() == "spawns") { activeTool = Tool::Spawns; }
        else if (objectBook->getActivePageName() == "items") {
            activeTool = Tool::Items;
        }
        else if (objectBook->getActivePageName() == "ai") {
            activeTool = Tool::NPCs;
        }
        else if (objectBook->getActivePageName() == "lights") {
            activeTool = Tool::Lights;
        }
    };

    Notebook::Ptr controlBook = Notebook::create();
    controlBook->addPage("map", "Props", infoBox, [this]() { activeTool = Tool::Metadata; });
    controlBook->addPage("edit", "Map", editBook, editOpened, editClosed);
    controlBook->addPage("obj", "Entities", objectBook, onObjectActive);
    controlBook->addPage("events", "Scripts", eventBox, [this]() { activeTool = Tool::Events; });
    controlBook->addPage(
        "ppl", "Peoplemon", peoplemonBox, [this]() { activeTool = Tool::Peoplemon; });

    controlPane->pack(mapCtrlBox, true, false);
    controlPane->pack(controlBook, true, false);
    controlPane->pack(tileset.getContent(), true, true);

    content->pack(controlPane, false, true);
    content->pack(mapArea.getContent(), true, true);

    mapArea.editMap().editorLoad("WorldMap.map");
    syncGui();
}

void Map::update(float) {
    if (selectionState == SelectionMade) { mapArea.editMap().showSelection(selection); }
    else if (selectionState == Selecting) {
        mapArea.editMap().showSelection({selection.left, selection.top, -1, -1});
    }
    else {
        mapArea.editMap().showSelection({0, 0, 0, 0});
    }

    switch (tileset.getActiveTool()) {
    case Tileset::CollisionTiles:
        mapArea.editMap().setRenderOverlay(component::EditMap::RenderOverlay::Collisions,
                                           levelSelect->getSelectedOption());
        break;

    case Tileset::CatchTiles:
        mapArea.editMap().setRenderOverlay(component::EditMap::RenderOverlay::CatchTiles,
                                           levelSelect->getSelectedOption());
        break;

    default:
        switch (activeTool) {
        case Tool::Events:
            mapArea.editMap().setRenderOverlay(component::EditMap::RenderOverlay::Events, 0);
            break;

        case Tool::Spawns:
            mapArea.editMap().setRenderOverlay(component::EditMap::RenderOverlay::Spawns,
                                               levelSelect->getSelectedOption());
            break;

        default:
            mapArea.editMap().setRenderOverlay(component::EditMap::RenderOverlay::None, 0);
            break;
        }
        break;
    }

    if (mapArea.editMap().unsavedChanges() || tileset.unsavedChanges()) {
        saveMapBut->setColor(sf::Color(200, 185, 20), sf::Color::Red);
    }
    else {
        saveMapBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Map::doLoadMap(const std::string& file) {
    mapPicker.close();

    if (makingNewMap) {
        const std::string f =
            bl::util::FileUtil::getExtension(file) == "map" ? file : file + ".map";
        newMapWindow.show(parent, f);
    }
    else {
        if (!mapArea.editMap().editorLoad(file)) {
            bl::dialog::tinyfd_messageBox("Error Loading Map",
                                          std::string("Failed to load map: " + file).c_str(),
                                          "ok",
                                          "error",
                                          1);
        }
    }

    syncGui();
}

void Map::makeNewMap(const std::string& file, const std::string& name, const std::string& tileset,
                     unsigned int w, unsigned int h) {
    mapArea.editMap().newMap(file, name, tileset, w, h);
}

void Map::onMapClick(const sf::Vector2f& pixels, const sf::Vector2i& tiles) {
    BL_LOG_INFO << "Clicked (" << tiles.x << ", " << tiles.y << ")";

    switch (activeTool) {
    case Tool::MapEdit:
        switch (activeSubtool) {
        case Subtool::Set:
            switch (tileset.getActiveTool()) {
            case Tileset::Tiles:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setTileArea(levelSelect->getSelectedOption(),
                                                  layerSelect->getSelectedOption(),
                                                  selection,
                                                  tileset.getActiveTile(),
                                                  false);
                }
                else {
                    mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                              layerSelect->getSelectedOption(),
                                              tiles,
                                              tileset.getActiveTile(),
                                              false);
                }
                break;
            case Tileset::Animations:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setTileArea(levelSelect->getSelectedOption(),
                                                  layerSelect->getSelectedOption(),
                                                  selection,
                                                  tileset.getActiveAnim(),
                                                  true);
                }
                else {
                    mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                              layerSelect->getSelectedOption(),
                                              tiles,
                                              tileset.getActiveAnim(),
                                              true);
                }
                break;
            case Tileset::CollisionTiles:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setCollisionArea(
                        levelSelect->getSelectedOption(), selection, tileset.getActiveCollision());
                }
                else {
                    mapArea.editMap().setCollision(
                        levelSelect->getSelectedOption(), tiles, tileset.getActiveCollision());
                }
                break;
            case Tileset::CatchTiles:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setCatchArea(
                        levelSelect->getSelectedOption(), selection, tileset.getActiveCatch());
                }
                else {
                    mapArea.editMap().setCatch(
                        levelSelect->getSelectedOption(), tiles, tileset.getActiveCatch());
                }
                break;
            default:
                break;
            }
            break;

        case Subtool::Clear:
            switch (tileset.getActiveTool()) {
            case Tileset::Tiles:
            case Tileset::Animations:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setTileArea(levelSelect->getSelectedOption(),
                                                  layerSelect->getSelectedOption(),
                                                  selection,
                                                  core::map::Tile::Blank,
                                                  false);
                }
                else {
                    mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                              layerSelect->getSelectedOption(),
                                              tiles,
                                              core::map::Tile::Blank,
                                              false);
                }
                break;
            case Tileset::CollisionTiles:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setCollisionArea(
                        levelSelect->getSelectedOption(), selection, core::map::Collision::Open);
                }
                else {
                    mapArea.editMap().setCollision(
                        levelSelect->getSelectedOption(), tiles, core::map::Collision::Open);
                }
                break;
            case Tileset::CatchTiles:
                if (selectionState == SelectionMade) {
                    mapArea.editMap().setCatchArea(
                        levelSelect->getSelectedOption(), selection, core::map::Catch::NoEncounter);
                }
                else {
                    mapArea.editMap().setCatch(
                        levelSelect->getSelectedOption(), tiles, core::map::Catch::NoEncounter);
                }
                break;
            default:
                break;
            }
            break;

        case Subtool::Select:
            switch (selectionState) {
            case SelectionMade:
            case NoSelection:
                selectionState = Selecting;
                selection.left = tiles.x;
                selection.top  = tiles.y;
                break;
            case Selecting:
                selectionState = SelectionMade;
                {
                    const int minX = std::min(selection.left, tiles.x);
                    const int minY = std::min(selection.top, tiles.y);
                    const int maxX = std::max(selection.left, tiles.x);
                    const int maxY = std::max(selection.top, tiles.y);
                    selection      = {minX, minY, maxX - minX + 1, maxY - minY + 1};
                }
                break;
            default:
                selectionState = NoSelection;
                break;
            }
            break;

        default:
            break;
        }
        break;

    case Tool::Events:
        if (createEventRadio->getValue()) {
            mapArea.disableControls();
            eventEditor.open(parent, nullptr, tiles);
        }
        else if (editEventRadio->getValue()) {
            const core::map::Event* e = mapArea.editMap().getEvent(tiles);
            if (e) { eventEditor.open(parent, e, tiles); }
        }
        else if (deleteEventRadio->getValue()) {
            const core::map::Event* e = mapArea.editMap().getEvent(tiles);
            if (e) {
                std::stringstream ss;
                ss << e->script << '\n';
                ss << "Delete event?";
                std::string s = ss.str();
                for (char& c : s) {
                    if (c == '"' || c == '\'') { c = '`'; }
                }
                if (1 == bl::dialog::tinyfd_messageBox(
                             "Delete Event?", s.c_str(), "yesno", "warning", 0)) {
                    mapArea.editMap().removeEvent(e);
                }
            }
        }
        break;

    case Tool::Spawns:
        if (spawnCreate->getValue()) {
            const char* id = bl::dialog::tinyfd_inputBox("Spawn ID", "Enter spawn id: ", "0");
            if (id) {
                if (isNum(id)) {
                    const unsigned int n = std::atoi(id);
                    if (mapArea.editMap().spawnIdUnused(n)) {
                        mapArea.editMap().addSpawn(levelSelect->getSelectedOption(),
                                                   tiles,
                                                   n,
                                                   static_cast<core::component::Direction>(
                                                       spawnDirEntry->getSelectedOption()));
                    }
                }
            }
        }
        else if (spawnRotate->getValue()) {
            mapArea.editMap().rotateSpawn(levelSelect->getSelectedOption(), tiles);
        }
        else if (spawnDelete->getValue()) {
            mapArea.editMap().removeSpawn(levelSelect->getSelectedOption(), tiles);
        }
        break;

    case Tool::NPCs:
        if (npcSpawn->getValue()) {
            const core::map::CharacterSpawn* s =
                mapArea.editMap().getNpcSpawn(levelSelect->getSelectedOption(), tiles);
            if (!s) {
                mapArea.disableControls();
                characterEditor.open(parent, levelSelect->getSelectedOption(), tiles, nullptr);
            }
        }
        else if (npcEdit->getValue()) {
            const core::map::CharacterSpawn* s =
                mapArea.editMap().getNpcSpawn(levelSelect->getSelectedOption(), tiles);
            if (s) {
                mapArea.disableControls();
                characterEditor.open(parent, levelSelect->getSelectedOption(), tiles, s);
            }
        }
        else if (npcDelete->getValue()) {
            const core::map::CharacterSpawn* s =
                mapArea.editMap().getNpcSpawn(levelSelect->getSelectedOption(), tiles);
            if (s) {
                const std::string msg = "Delete spawn for character: " + s->file + "?";
                if (1 == bl::dialog::tinyfd_messageBox(
                             "Delete Character?", msg.c_str(), "yesno", "warning", 0)) {
                    mapArea.editMap().removeNpcSpawn(s);
                }
            }
        }
        break;

    case Tool::Items:
        if (itemSpawn->getValue()) {
            mapArea.editMap().addOrEditItem(levelSelect->getSelectedOption(),
                                            tiles,
                                            itemSelector->currentItem(),
                                            !itemHidden->getValue());
        }
        else if (itemRead->getValue()) {
            const auto i = mapArea.editMap().getItem(levelSelect->getSelectedOption(), tiles);
            if (i.first != core::item::Id::Unknown) {
                itemSelector->setItem(i.first);
                itemHidden->setValue(!i.second);
            }
        }
        else if (itemDelete->getValue()) {
            mapArea.editMap().removeItem(levelSelect->getSelectedOption(), tiles);
        }
        break;

    case Tool::Lights:
        if (lightCreateOrEdit->getValue()) {
            mapArea.editMap().setLight(sf::Vector2i(pixels),
                                       std::atoi(lightRadiusEntry->getInput().c_str()));
        }
        else if (lightRemove->getValue()) {
            mapArea.editMap().removeLight(sf::Vector2i(pixels));
        }
        break;

    case Tool::Peoplemon:
    case Tool::Metadata:
    default:
        break;
    }
}

void Map::onChoosePlaylist(const std::string& file) {
    mapArea.editMap().setPlaylist(file);
    playlistLabel->setText(file);
    playlistPicker.close();
}

bool Map::checkUnsaved() {
    if (mapArea.editMap().unsavedChanges()) {
        return bl::dialog::tinyfd_messageBox(
                   "Unsaved Changes",
                   std::string(mapArea.editMap().name() + " has unsaved changes, discard them?")
                       .c_str(),
                   "yesno",
                   "warning",
                   0) == 1;
    }
    return true;
}

void Map::syncGui() {
    tileset.loadTileset(mapArea.editMap().tilesetField);
    levelSelect->clearOptions();
    for (unsigned int i = 0; i < mapArea.editMap().levelCount(); ++i) {
        levelSelect->addOption("Level " + std::to_string(i));
    }
    levelSelect->setSelectedOption(0);
    onLevelChange(0);
    levelPage.sync(mapArea.editMap().levelFilter);
    layerPage.sync(mapArea.editMap().levels, mapArea.editMap().layerFilter);

    nameEntry->setInput(mapArea.editMap().name());
    playlistLabel->setText(mapArea.editMap().playlistField);
    weatherEntry->setSelectedOption(static_cast<int>(mapArea.editMap().weatherField));

    onEnterLabel->setText(mapArea.editMap().getOnEnterScript());
    onExitLabel->setText(mapArea.editMap().getOnExitScript());
}

void Map::onLevelChange(unsigned int l) {
    if (l >= mapArea.editMap().levels.size()) {
        BL_LOG_ERROR << "Out of range level: " << l;
        return;
    }

    auto& level           = mapArea.editMap().levels[l];
    const unsigned int bc = level.bottomLayers().size();
    const unsigned int yc = level.ysortLayers().size();
    const unsigned int tc = level.topLayers().size();

    layerSelect->clearOptions();
    unsigned int i = 0;
    for (unsigned int j = 0; j < bc; ++j, ++i) {
        layerSelect->addOption("Layer " + std::to_string(i));
    }
    for (unsigned int j = 0; j < yc; ++j, ++i) {
        layerSelect->addOption("Layer " + std::to_string(i) + " (ysort)");
    }
    for (unsigned int j = 0; j < tc; ++j, ++i) {
        layerSelect->addOption("Layer " + std::to_string(i));
    }
    layerSelect->setSelectedOption(0);
}

void Map::onChooseScript(const std::string& s) {
    if (choosingOnloadScript) { mapArea.editMap().setOnEnterScript(s); }
    else {
        mapArea.editMap().setOnExitScript(s);
    }
}

void Map::onEventEdit(const core::map::Event* orig, const core::map::Event& val) {
    if (orig) { mapArea.editMap().editEvent(orig, val); }
    else {
        mapArea.editMap().createEvent(val);
    }
    mapArea.enableControls();
}

void Map::onCharacterEdit(const core::map::CharacterSpawn* orig,
                          const core::map::CharacterSpawn& spawn) {
    if (orig) { mapArea.editMap().editNpcSpawn(orig, spawn); }
    else {
        mapArea.editMap().addNpcSpawn(spawn);
    }
    mapArea.enableControls();
}

} // namespace page
} // namespace editor
