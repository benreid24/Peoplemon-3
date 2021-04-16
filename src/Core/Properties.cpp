#include <BLIB/Engine/Configuration.hpp>
#include <Core/Properties.hpp>

namespace core
{
const std::string Properties::WindowIconFile  = "Resources/Images/icon.png";
const std::string Properties::SpritesheetPath = "Resources/Images/Spritesheets";
const std::string Properties::MusicPath       = "Resources/Audio/Music";
const std::string Properties::MenuImagePath   = "Resources/Images/Menus";
const std::string Properties::MapPath         = "Resources/Maps/Maps";
const std::string Properties::TilesetPath     = "Resources/Maps/Tilesets";

namespace
{
sf::Font menuFont;
}

bool Properties::load() {
    // These must be set for playlists and animations to work properly
    bl::engine::Configuration::set("blib.playlist.song_path", MusicPath);
    bl::engine::Configuration::set("blib.animation.spritesheet_path", SpritesheetPath);

    // TODO - put font into resource manager
    if (!menuFont.loadFromFile("Resources/Fonts/Menu.ttf")) return false;

    return true;
}

const sf::Font& Properties::MenuFont() { return menuFont; }

} // namespace core
