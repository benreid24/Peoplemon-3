#include <Editor/Pages/Subpages/Tileset.hpp>

#include <BLIB/Engine.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Tileset::Tileset() {
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
    auto txtr = bl::engine::Resources::textures().load("EditorResources/Collisions/all.png").data;
    RadioButton::Group* group = nullptr;
    for (unsigned int i = 0; i < 57; ++i) {
        Image::Ptr img = Image::create(txtr);
        img->scaleToSize({56, 56});
        RadioButton::Ptr button = RadioButton::create(img, group); // TODO - special button
        group                   = button->getRadioGroup();
        tilesBox->pack(button);
    }
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

    animsBox = Box::create(LinePacker::create(LinePacker::Vertical)); // TODO - grid packer
    animsBox->pack(Label::create("Animations will go here in a grid"), true, true);
    animPage->pack(animsBox, true, true);

    content->addPage("tile", "Tiles", tilePage);
    content->addPage("anim", "Animations", animPage);
    content->addPage("col", "Collisions", collisions.getContent());
    content->addPage("catch", "Catch Tiles", catchables.getContent());
}

Element::Ptr Tileset::getContent() { return content; }

} // namespace page
} // namespace editor
