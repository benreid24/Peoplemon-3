#include <Editor/Pages/Subpages/Tileset.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Tileset::Tileset() {
    content = Notebook::create("tileset");

    tilePage             = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    tileButBox           = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    addTileBut           = Button::create("Add Tile");
    importSpritesheetBut = Button::create("Add Tilesheet");
    delTileBut           = Button::create("Delete Tile");
    tileButBox->pack(addTileBut, false, true);
    tileButBox->pack(importSpritesheetBut, false, true);
    tileButBox->pack(delTileBut, false, true);
    tilePage->pack(tileButBox, true, false);

    tilesBox = Box::create(LinePacker::create(LinePacker::Vertical)); // TODO - grid packer
    tilesBox->pack(Label::create("Tiles will appear in a grid here"), true, true);
    tilePage->pack(tilesBox, true, true);

    animPage   = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    animButBox = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    addAnimBut = Button::create("Add Animation");
    delAnimBut = Button::create("Delete Animation");
    animButBox->pack(addAnimBut);
    animButBox->pack(delAnimBut);
    animPage->pack(animButBox);

    animsBox = Box::create(LinePacker::create(LinePacker::Vertical)); // TODO - grid packer
    animsBox->pack(Label::create("Animations will go here in a grid"), true, true);
    animPage->pack(animsBox, true, true);

    content->addPage("tile", "Tiles", tilePage);
    content->addPage("anim", "Animations", animPage);
    content->addPage("col", "Collisions", Label::create("Collisions here"));
    content->addPage("catch", "Catch Tiles", Label::create("Catch tiles here"));
}

Element::Ptr Tileset::getContent() { return content; }

} // namespace page
} // namespace editor
