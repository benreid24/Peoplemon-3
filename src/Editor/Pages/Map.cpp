#include <Editor/Pages/Map.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Map::Map(core::system::Systems& s)
: Page(s)
, levelPage(Layers::Level)
, layerPage(Layers::Layer) {
    content     = Box::create(LinePacker::create(LinePacker::Horizontal, 4), "maps");
    controlPane = Box::create(LinePacker::create(LinePacker::Vertical, 4), "maps");

    mapCtrlBox = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    newMapBut  = Button::create("New Map");
    loadMapBut = Button::create("Load Map");
    saveMapBut = Button::create("Save Map");
    mapCtrlBox->pack(newMapBut);
    mapCtrlBox->pack(loadMapBut);
    mapCtrlBox->pack(saveMapBut);

    tileBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));

    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Horizontal));
    levelSelect  = ComboBox::create("maps");
    levelSelect->addOption("Layer 0");
    levelSelect->addOption("Layer 1");
    levelSelect->addOption("Layer 2 (ysort)");
    levelSelect->addOption("Layer 3");
    levelSelect->addOption("Layer 4 (top)");
    levelSelect->setSelectedOption(0);
    tileSetBut = RadioButton::create("Set");
    tileSetBut->setValue(true);
    tileClearBut    = RadioButton::create("Clear", tileSetBut->getRadioGroup());
    tileSelectBut   = RadioButton::create("Select", tileSetBut->getRadioGroup());
    tileDeselectBut = Button::create("Deselect");
    box->pack(levelSelect, false, true);
    box->pack(tileSetBut, true, true);
    box->pack(tileClearBut, true, true);
    box->pack(tileSelectBut, true, true);
    box->pack(tileDeselectBut, true, true);
    tileBox->pack(box, true, false);

    box            = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    tileLevelUpBut = Button::create("Selection Level Up");
    tileLayerUpBut = Button::create("Selection Layer Up");
    box->pack(tileLevelUpBut, true, true);
    box->pack(tileLayerUpBut, true, true);
    tileBox->pack(box, true, false);

    box              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    tileLevelDownBut = Button::create("Selection Level Down");
    tileLayerDownBut = Button::create("Selection Layer Down");
    box->pack(tileLevelDownBut, true, true);
    box->pack(tileLayerDownBut, true, true);
    tileBox->pack(box, true, false);

    infoBox      = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));

    box       = Box::create(LinePacker::create(LinePacker::Horizontal));
    nameLabel = Label::create("Name:");
    nameEntry = TextEntry::create(1);
    box->pack(nameLabel, false, true);
    box->pack(nameEntry, true, true);
    row->pack(box, true, false);

    resizeBut = Button::create("Resize Map");
    row->pack(resizeBut);
    infoBox->pack(row, true, false);

    row             = Box::create(LinePacker::create(LinePacker::Horizontal));
    playlistLabel   = Label::create("playerlistFile.bplst");
    pickPlaylistBut = Button::create("Pick Playlist");
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
    row->pack(weatherEntry);
    infoBox->pack(row, true, false);

    editBook = Notebook::create("maps");
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

    objectBook = Notebook::create("maps");
    objectBook->addPage("spawns", "Spawns", Label::create("Player spawn controls here"));
    objectBook->addPage("ai", "NPC's", Label::create("NPC controls here"));
    objectBook->addPage("items", "Items", Label::create("Item controls here"));
    objectBook->addPage("lights", "Lights", Label::create("Light controls here"));

    const auto editClosed = [this]() {
        layerPage.unpack();
        levelPage.unpack();
    };
    const auto editOpened = [this]() {
        if (editBook->getActivePageName() == "layers")
            layerPage.pack();
        else if (editBook->getActivePageName() == "levels")
            levelPage.pack();
    };

    controlBook = Notebook::create("maps");
    controlBook->addPage("map", "Map", infoBox);
    controlBook->addPage("edit", "Edit", editBook, editOpened, editClosed);
    controlBook->addPage("obj", "Objects", objectBook);
    controlBook->addPage("events", "Scripts", Label::create("Event controls here"));
    controlBook->addPage("ppl", "Peoplemon", Label::create("Peoplemon controls here"));

    controlPane->pack(mapCtrlBox, true, false);
    controlPane->pack(controlBook, true, false);
    controlPane->pack(tileset.getContent(), true, true);

    content->pack(controlPane, false, true);
    content->pack(Label::create("Map canvas here"), true, true);
}

void Map::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
