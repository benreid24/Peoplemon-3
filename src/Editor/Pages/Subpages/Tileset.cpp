#include <Editor/Pages/Subpages/Tileset.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>
#include <Editor/Pages/Subpages/MapArea.hpp>

namespace editor
{
namespace page
{
namespace
{
std::string makeCopyName(const std::string& dest, const std::string& file) {
    const std::string& base  = bl::util::FileUtil::getFilename(file);
    const std::string result = bl::util::FileUtil::joinPath(dest, base);
    if (bl::util::FileUtil::exists(result)) {
        std::stringstream ss;
        ss << bl::util::FileUtil::getBaseName(base) << "_" << std::hex
           << bl::util::Random::get<int>(1000, 10000000) << "."
           << bl::util::FileUtil::getExtension(base);
        return ss.str();
    }
    return base;
}
} // namespace

using namespace bl::gui;

Tileset::Tileset(const DeleteCb& dcb, MapArea& map)
: deleteCb(dcb)
, catchables(map.editMap())
, towns(map)
, tool(Active::Tiles)
, activeTile(core::map::Tile::Blank)
, activeAnim(core::map::Tile::Blank)
, dirty(false) {
    content = Notebook::create();

    bl::gui::Box::Ptr tilePage      = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    bl::gui::Box::Ptr tileButBox    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    bl::gui::Button::Ptr addTileBut = Button::create("Add Tile");
    addTileBut->setTooltip("Import an image as a tile");
    addTileBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const char* filters[] = {"*.png", "*.jpg", "*.bmp", "*.gif"};
        const char* file =
            bl::dialog::tinyfd_openFileDialog("Add tile(s)", nullptr, 4, filters, "Image files", 1);
        if (file) {
            std::stringstream ss(file);
            std::string tile;
            while (std::getline(ss, tile, '|')) {
                sf::Image img;
                std::string filename = makeCopyName(core::Properties::MapTilePath(), tile);
                filename             = bl::util::FileUtil::getBaseName(filename) + ".png";
                if (!img.loadFromFile(tile)) {
                    BL_LOG_ERROR << "Failed to load tile: " << tile;
                    continue;
                }
                if (!img.saveToFile(
                        bl::util::FileUtil::joinPath(core::Properties::MapTilePath(), filename))) {
                    BL_LOG_ERROR << "Failed to copy tile: " << tile << " -> " << filename;
                    continue;
                }
                tileset->addTexture(filename);
            }
            dirty = true;
            updateGui();
        }
    });
    bl::gui::Button::Ptr importSpritesheetBut = Button::create("Add Tilesheet");
    importSpritesheetBut->setTooltip("Import a spritesheet as many tiles");
    // TODO - implement sprite sheet import
    bl::gui::Button::Ptr delTileBut = Button::create("Delete Tile");
    delTileBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (1 == bl::dialog::tinyfd_messageBox("Remove tile?",
                                               "Are you sure you want to delete this tile?\nThis "
                                               "action cannot be undone and clears edit history",
                                               "yesno",
                                               "warning",
                                               0)) {
            deleteCb(activeTile, false);
            tileset->removeTexture(activeTile);
            dirty = true;
            updateGui();
        }
    });
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
    addAnimBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const char* filters[] = {"*.anim"};
        const char* file      = bl::dialog::tinyfd_openFileDialog(
            "Add animation", nullptr, 1, filters, "Animation files", 0);
        if (file) {
            const std::string animFile = makeCopyName(core::Properties::MapAnimationPath(), file);
            bl::gfx::AnimationData anim;
            if (!anim.load(file)) {
                bl::dialog::tinyfd_messageBox(
                    "Bad Animation", "Failed to load animation", "ok", "error", 0);
                return;
            }

            bl::util::FileUtil::copyFile(
                file, bl::util::FileUtil::joinPath(core::Properties::MapAnimationPath(), animFile));
            const std::string sp = bl::util::FileUtil::joinPath(bl::util::FileUtil::getPath(file),
                                                                anim.spritesheetFile());
            if (bl::util::FileUtil::exists(sp)) {
                bl::util::FileUtil::copyFile(
                    sp,
                    bl::util::FileUtil::joinPath(core::Properties::SpritesheetPath(),
                                                 anim.spritesheetFile()));
            }

            tileset->addAnimation(animFile);
            dirty = true;
            updateGui();
        }
    });
    bl::gui::Button::Ptr delAnimBut = Button::create("Delete Animation");
    delAnimBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (1 ==
            bl::dialog::tinyfd_messageBox("Remove animation?",
                                          "Are you sure you want to delete this animation?\nThis "
                                          "action cannot be undone and clears edit history",
                                          "yesno",
                                          "warning",
                                          0)) {
            deleteCb(activeAnim, true);
            tileset->removeAnimation(activeAnim);
            dirty = true;
            updateGui();
        }
    });
    delAnimBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    animButBox->pack(addAnimBut);
    animButBox->pack(delAnimBut);
    animPage->pack(animButBox);

    animsBox = Box::create(GridPacker::createDynamicGrid(GridPacker::Rows, 300, 10));
    scroll   = ScrollArea::create(LinePacker::create(LinePacker::Vertical));
    scroll->pack(animsBox, true, true);
    animPage->pack(scroll, true, true);

    content->setMaxTabWidth(250.f);
    content->addPage("tile", "Tiles", tilePage, [this]() { tool = Active::Tiles; });
    content->addPage("anim", "Animations", animPage, [this]() { tool = Active::Animations; });
    content->addPage("towns", "Towns", towns.getContent(), [this]() { tool = Active::TownTiles; });
    content->addPage(
        "col", "Collisions", collisions.getContent(), [this]() { tool = Active::CollisionTiles; });
    content->addPage(
        "catch", "Catch Tiles", catchables.getContent(), [this]() { tool = Active::CatchTiles; });
    content->addPage("level", "Level Transitions", levelTransitions.getContent(), [this]() {
        tool = Active::LevelTiles;
    });

    loadTileset("Worldtileset.tlst");
}

void Tileset::setGUI(const GUI::Ptr& gui) {
    catchables.setGUI(gui);
    towns.setGUI(gui);
}

Element::Ptr Tileset::getContent() { return content; }

Tileset::Active Tileset::getActiveTool() const { return tool; }

core::map::Tile::IdType Tileset::getActiveTile() const { return activeTile; }

core::map::Tile::IdType Tileset::getActiveAnim() const { return activeAnim; }

core::map::Collision Tileset::getActiveCollision() const { return collisions.selected(); }

std::uint8_t Tileset::getActiveCatch() const { return catchables.selected(); }

std::uint8_t Tileset::getActiveTown() const { return towns.selected(); }

core::map::LevelTransition Tileset::getActiveLevel() const { return levelTransitions.getActive(); }

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
        Image::Ptr img = Image::create(pair->second);
        img->scaleToSize({56, 56});
        component::HighlightRadioButton::Ptr button =
            component::HighlightRadioButton::create(img, group);
        button->getSignal(Event::LeftClicked).willAlwaysCall([this, pair](const Event&, Element*) {
            activeTile = pair->first;
        });
        if (!group) {
            activeTile = pair->first;
            button->setValue(true);
        }
        group = button->getRadioGroup();
        tilesBox->pack(button);
    }

    // animations
    group = nullptr;
    for (const auto& pair : tileset->getAnims()) {
        Animation::Ptr anim = Animation::create(pair->second);
        anim->scaleToSize({56, 56});
        component::HighlightRadioButton::Ptr button =
            component::HighlightRadioButton::create(anim, group);
        button->getSignal(Event::LeftClicked).willAlwaysCall([this, pair](const Event&, Element*) {
            activeAnim = pair->first;
        });
        if (!group) {
            activeAnim = pair->first;
            button->setValue(true);
        }
        group = button->getRadioGroup();
        animsBox->pack(button);
    }

    catchables.refresh();
    towns.refresh();
}

bool Tileset::unsavedChanges() const { return dirty; }

void Tileset::markSaved() { dirty = false; }

void Tileset::refresh() {
    catchables.refresh();
    towns.refresh();
}

} // namespace page
} // namespace editor
