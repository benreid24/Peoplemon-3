#include <BLIB/Engine/Configuration.hpp>
#include <Common/Properties.hpp>

namespace core
{
const std::string Properties::SpritesheetPath = "Resources/Images/Spritesheets";
const std::string Properties::MusicPath       = "Resources/Audio/Music";
const std::string Properties::MenuImagePath   = "Resources/Images/Menus";

namespace
{
sf::Font menuFont;
}

bool Properties::load() {
    // These must be set for playlists and animations to work properly
    bl::engine::Configuration::set("blib.playlist.song_path", MusicPath);
    bl::engine::Configuration::set("blib.animation.spritesheet_path", SpritesheetPath);

    if (!menuFont.loadFromFile("Resources/Fonts/Menu.ttf")) return false;

    return true;
}

const sf::Font& Properties::MenuFont() { return menuFont; }

} // namespace core
