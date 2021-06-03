#include <Editor/Pages/Map.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Map::Map(core::system::Systems& s)
: Page(s) {
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
    levelSelect->setSelectedOption(0);
    tileSetBut = RadioButton::create("Set");
    tileSetBut->setValue(true);
    tileClearBut    = RadioButton::create("Clear", tileSetBut->getRadioGroup());
    tileSelectBut   = RadioButton::create("Select", tileSetBut->getRadioGroup());
    tileDeselectBut = Button::create("Clear Selection");
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
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4, LinePacker::Uniform));

    box       = Box::create(LinePacker::create(LinePacker::Horizontal));
    nameLabel = Label::create("Name:");
    nameEntry = TextEntry::create(1);
    box->pack(nameLabel, false, true);
    box->pack(nameEntry, true, true);
    row->pack(box, true, false);

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

    row             = Box::create(LinePacker::create(LinePacker::Horizontal));
    playlistLabel   = Label::create("playerlistFile.bplst");
    pickPlaylistBut = Button::create("Pick Playlist");
    row->pack(pickPlaylistBut);
    row->pack(playlistLabel);
    infoBox->pack(row);

    controlBook = Notebook::create("maps");
    controlBook->addPage("map", "Map", infoBox);
    controlBook->addPage("tiles", "Edit", tileBox);
    controlBook->addPage("layers", "Layers", Label::create("Layer controls here"));
    controlBook->addPage("spawns", "Spawns", Label::create("Player spawn controls here"));
    controlBook->addPage("ai", "NPC's", Label::create("NPC controls here"));
    controlBook->addPage("items", "Items", Label::create("Item controls here"));
    controlBook->addPage("events", "Scripts", Label::create("Event controls here"));
    controlBook->addPage("lights", "Lights", Label::create("Light controls here"));
    controlBook->addPage("ppl", "Peoplemon", Label::create("Peoplemon controls here"));

    controlPane->pack(mapCtrlBox, true, false);
    controlPane->pack(controlBook, true, false);
    controlPane->pack(Label::create("Tileset here"), true, true);

    content->pack(controlPane, false, true);
    content->pack(Label::create("Map canvas here"), true, true);
}

void Map::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
