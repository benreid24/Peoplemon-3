#include <Editor/Pages/Subpages/Tileset.hpp>

#include <BLIB/Engine.hpp>
#include <Core/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Tileset::Tileset()
: tool(Active::Tiles)
, activeTile(core::map::Tile::Blank)
, activeAnim(core::map::Tile::Blank) {
    content = Notebook::create("tileset");

    bl::gui::Box::Ptr tilePage      = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    bl::gui::Box::Ptr tileButBox    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr addTileBut = Button::create("Add Tile");
    bl::gui::Button::Ptr importSpritesheetBut = Button::create("Add Tilesheet");
    bl::gui::Button::Ptr delTileBut           = Button::create("Delete Tile");
    delTileBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    tileButBox->pack(addTileBut, false, true);
    tileButBox->pack(importSpritesheetBut, false, true);
    tileButBox->pack(delTileBut, false, true);
    tilePage->pack(tileButBox, true, false);

    tilesBox               = Box::create(GridPacker::createDynamicGrid(GridPacker::Rows, 300, 10));
    ScrollArea::Ptr scroll = ScrollArea::create(LinePacker::create(LinePacker::Vertical));
    scroll->pack(tilesBox, true, true);
    tilePage->pack(scroll, true, true);

    bl::gui::Box::Ptr animPage      = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    bl::gui::Box::Ptr animButBox    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr addAnimBut = Button::create("Add Animation");
    bl::gui::Button::Ptr delAnimBut = Button::create("Delete Animation");
    delAnimBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    animButBox->pack(addAnimBut);
    animButBox->pack(delAnimBut);
    animPage->pack(animButBox);

    animsBox = Box::create(GridPacker::createDynamicGrid(GridPacker::Rows, 300, 10));
    scroll   = ScrollArea::create(LinePacker::create(LinePacker::Vertical));
    scroll->pack(animsBox, true, true);
    animPage->pack(animsBox, true, true);

    content->addPage("tile", "Tiles", tilePage, [this]() { tool = Active::Tiles; });
    content->addPage("anim", "Animations", animPage, [this]() { tool = Active::Animations; });
    content->addPage(
        "col", "Collisions", collisions.getContent(), [this]() { tool = Active::CollisionTiles; });
    content->addPage(
        "catch", "Catch Tiles", catchables.getContent(), [this]() { tool = Active::CatchTiles; });

    loadTileset("Worldtileset.tlst");
}

Element::Ptr Tileset::getContent() { return content; }

Tileset::Active Tileset::getActiveTool() const { return tool; }

core::map::Tile::IdType Tileset::getActiveTile() const { return activeTile; }

core::map::Tile::IdType Tileset::getActiveAnim() const { return activeAnim; }

bool Tileset::loadTileset(const std::string& file) {
    auto newTileset = core::Resources::tilesets().load(file).data;
    if (!newTileset) return false;
    if (newTileset.get() != tileset.get()) {
        tileset = newTileset;
        updateGui();
    }
    return true;
}

void Tileset::updateGui() {
    tilesBox->clearChildren(true);
    animsBox->clearChildren(true);

    // tiles
    RadioButton::Group* group = nullptr;
    for (const auto& pair : tileset->getTiles()) {
        Image::Ptr img = Image::create(pair.second);
        img->scaleToSize({56, 56});
        component::HighlightRadioButton::Ptr button =
            component::HighlightRadioButton::create(img, group);
        button->getSignal(Action::LeftClicked)
            .willAlwaysCall([this, pair](const Action&, Element*) { activeTile = pair.first; });
        if (!group) {
            activeTile = pair.first;
            button->setValue(true);
        }
        group = button->getRadioGroup();
        tilesBox->pack(button);
    }

    // animations
    group = nullptr;
    for (const auto& pair : tileset->getAnims()) {
        Animation::Ptr anim = Animation::create(pair.second);
        anim->scaleToSize({56, 56});
        component::HighlightRadioButton::Ptr button =
            component::HighlightRadioButton::create(anim, group);
        button->getSignal(Action::LeftClicked)
            .willAlwaysCall([this, pair](const Action&, Element*) { activeAnim = pair.first; });
        if (!group) {
            activeTile = pair.first;
            button->setValue(true);
        }
        group = button->getRadioGroup();
        animsBox->pack(button);
    }
}

} // namespace page
} // namespace editor
