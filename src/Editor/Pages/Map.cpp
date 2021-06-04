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
    content = Box::create(LinePacker::create(LinePacker::Horizontal, 4), "maps");
    bl::gui::Box::Ptr controlPane =
        Box::create(LinePacker::create(LinePacker::Vertical, 4), "maps");

    bl::gui::Box::Ptr mapCtrlBox    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr newMapBut  = Button::create("New Map");
    bl::gui::Button::Ptr loadMapBut = Button::create("Load Map");
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

    Box::Ptr lightBox            = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    box                          = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    RadioButton::Ptr lightCreate = RadioButton::create("Create");
    lightCreate->setValue(true);
    lightRadiusEntry = TextEntry::create();
    lightRadiusEntry->setRequisition({80, 0});
    lightRadiusEntry->setInput("100");
    box->pack(lightCreate, false, false);
    box->pack(Label::create("Radius (pixels):"), false, false);
    box->pack(lightRadiusEntry, true, false);
    lightBox->pack(box, true, false);
    Label::Ptr label = Label::create("Delete");
    label->setColor(sf::Color(200, 20, 20), sf::Color::Transparent);
    RadioButton::Ptr lightDelete = RadioButton::create(label, lightCreate->getRadioGroup());
    lightBox->pack(lightDelete);

    bl::gui::Notebook::Ptr objectBook = Notebook::create("maps");
    objectBook->addPage("spawns", "Spawns", Label::create("Player spawn controls here"));
    objectBook->addPage("ai", "NPC's", Label::create("NPC controls here"));
    objectBook->addPage("items", "Items", Label::create("Item controls here"));
    objectBook->addPage("lights", "Lights", lightBox);

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
