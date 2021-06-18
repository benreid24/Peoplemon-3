#include <Editor/Pages/Map.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Map::Map(core::system::Systems& s)
: Page(s)
, mapArea([this](const sf::Vector2f& p, const sf::Vector2i& t) { onMapClick(p, t); },
          std::bind(&Map::syncGui, this), s)
, activeTool(Tool::Metadata)
, activeSubtool(Subtool::Set)
, mapPicker(core::Properties::MapPath(), {"map", "p3m"},
            std::bind(&Map::doLoadMap, this, std::placeholders::_1),
            [this]() { mapPicker.close(); })
, playlistPicker(core::Properties::PlaylistPath(), {"plst"},
                 std::bind(&Map::onChoosePlaylist, this, std::placeholders::_1),
                 [this]() { playlistPicker.close(); }) {
    content = Box::create(LinePacker::create(LinePacker::Horizontal, 4), "maps");
    bl::gui::Box::Ptr controlPane =
        Box::create(LinePacker::create(LinePacker::Vertical, 4), "maps");

    bl::gui::Box::Ptr mapCtrlBox   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr newMapBut = Button::create("New Map");
    newMapBut->getSignal(Action::LeftClicked).willCall([this](const Action&, Element*) {
        if (checkUnsaved()) {
            makingNewMap = true;
            mapPicker.open(FilePicker::CreateNew, "New map", parent);
        }
    });
    bl::gui::Button::Ptr loadMapBut = Button::create("Load Map");
    loadMapBut->getSignal(Action::LeftClicked).willCall([this](const Action&, Element*) {
        if (checkUnsaved()) {
            makingNewMap = false;
            mapPicker.open(FilePicker::PickExisting, "Load map", parent);
        }
    });
    bl::gui::Button::Ptr saveMapBut = Button::create("Save Map");
    saveMapBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (!mapArea.editMap().editorSave()) {
            bl::dialog::tinyfd_messageBox(
                "Error Saving Map",
                std::string("Failed to save map: " + mapArea.editMap().name()).c_str(),
                "ok",
                "error",
                1);
        }
    });
    mapCtrlBox->pack(newMapBut);
    mapCtrlBox->pack(loadMapBut);
    mapCtrlBox->pack(saveMapBut);

    bl::gui::Box::Ptr tileBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Uniform));

    levelSelect = ComboBox::create("maps");
    levelSelect->getSignal(Action::ValueChanged).willAlwaysCall([this](const Action&, Element* e) {
        onLevelChange(dynamic_cast<ComboBox*>(e)->getSelectedOption());
    });
    box->pack(levelSelect, true, true);

    layerSelect = ComboBox::create("maps");
    box->pack(layerSelect, true, true);
    tileBox->pack(box, true, false);

    box = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::RadioButton::Ptr tileSetBut = RadioButton::create("Set", nullptr, "set");
    tileSetBut->setValue(true);
    tileSetBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        activeSubtool = Subtool::Set;
    });
    bl::gui::RadioButton::Ptr tileClearBut =
        RadioButton::create("Clear", tileSetBut->getRadioGroup(), "clear");
    tileClearBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        activeSubtool = Subtool::Clear;
    });
    bl::gui::RadioButton::Ptr tileSelectBut =
        RadioButton::create("Select", tileSetBut->getRadioGroup(), "select");
    tileSelectBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        activeSubtool = Subtool::Select;
    });
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
    pickPlaylistBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        playlistPicker.open(FilePicker::PickExisting, "Select Playlist", parent);
    });
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
    weatherEntry->getSignal(Action::ValueChanged).willAlwaysCall([this](const Action&, Element*) {
        const core::map::Weather::Type type =
            static_cast<core::map::Weather::Type>(weatherEntry->getSelectedOption());
        if (mapArea.editMap().weatherSystem().getType() != type) {
            mapArea.editMap().setWeather(type);
        }
    });
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

    RadioButton::Group* tilesetButGroup = tileSetBut->getRadioGroup();
    const auto editOpened               = [this, editBook, tilesetButGroup]() {
        activeTool    = Tool::MapEdit;
        activeSubtool = Subtool::None;

        if (editBook->getActivePageName() == "tiles") {
            if (tilesetButGroup->getActiveButton()->group() == "set") {
                activeSubtool = Subtool::Set;
            }
            else if (tilesetButGroup->getActiveButton()->group() == "clear") {
                activeSubtool = Subtool::Clear;
            }
            else if (tilesetButGroup->getActiveButton()->group() == "select") {
                activeSubtool = Subtool::Select;
            }
        }
        else if (editBook->getActivePageName() == "layers")
            layerPage.pack();
        else if (editBook->getActivePageName() == "levels")
            levelPage.pack();
    };

    bl::gui::Notebook::Ptr controlBook = Notebook::create("maps");
    controlBook->addPage("map", "Map", infoBox, [this]() { activeTool = Tool::Metadata; });
    controlBook->addPage("edit", "Edit", editBook, editOpened, editClosed);
    controlBook->addPage("obj", "Objects", objectBook);
    controlBook->addPage("events", "Scripts", eventBox);
    controlBook->addPage("ppl", "Peoplemon", peoplemonBox);

    controlPane->pack(mapCtrlBox, true, false);
    controlPane->pack(controlBook, true, false);
    controlPane->pack(tileset.getContent(), true, true);

    content->pack(controlPane, false, true);
    content->pack(mapArea.getContent(), true, true);

    mapArea.editMap().editorLoad("WorldMap.map");
    syncGui();
}

void Map::update(float) {
    // TODO
}

void Map::doLoadMap(const std::string& file) {
    BL_LOG_INFO << "New/Load map: " << file;
    mapPicker.close();

    if (makingNewMap) {
        // TODO - new map
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

void Map::onMapClick(const sf::Vector2f&, const sf::Vector2i& tiles) {
    BL_LOG_INFO << "Clicked (" << tiles.x << ", " << tiles.y << ")";

    switch (activeTool) {
    case Tool::MapEdit:
        switch (activeSubtool) {
        case Subtool::Set:
            switch (tileset.getActiveTool()) {
            case Tileset::Tiles:
                mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                          layerSelect->getSelectedOption(),
                                          tiles,
                                          tileset.getActiveTile(),
                                          false);
                break;
            case Tileset::Animations:
                mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                          layerSelect->getSelectedOption(),
                                          tiles,
                                          tileset.getActiveAnim(),
                                          true);
                break;
            case Tileset::CollisionTiles:
                // TODO
                break;
            case Tileset::CatchTiles:
                // TODO
                break;
            default:
                break;
            }
            break;

        case Subtool::Clear:
            switch (tileset.getActiveTool()) {
            case Tileset::Tiles:
                mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                          layerSelect->getSelectedOption(),
                                          tiles,
                                          core::map::Tile::Blank,
                                          false);
                break;
            case Tileset::Animations:
                mapArea.editMap().setTile(levelSelect->getSelectedOption(),
                                          layerSelect->getSelectedOption(),
                                          tiles,
                                          core::map::Tile::Blank,
                                          false);
                break;
            case Tileset::CollisionTiles:
                // TODO
                break;
            case Tileset::CatchTiles:
                // TODO
                break;
            default:
                break;
            }
            break;

        case Subtool::Select:
            // TODO

        default:
            break;
        }

    case Tool::Items:
    case Tool::Lights:
    case Tool::NPCs:
    case Tool::Peoplemon:
    case Tool::Scripts:
    case Tool::Spawns:
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

    nameEntry->setInput(mapArea.editMap().name());
    playlistLabel->setText(mapArea.editMap().playlistField);
    weatherEntry->setSelectedOption(static_cast<int>(mapArea.editMap().weatherField.getValue()));
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

} // namespace page
} // namespace editor
