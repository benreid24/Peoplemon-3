#include <BLIB/Engine/Configuration.hpp>
#include <Properties.hpp>

const std::string Properties::SpritesheetPath = "Resources/Images/Spritesheets";
const std::string Properties::MusicPath       = "Resources/Audio/Music";

bool Properties::load() {
    // These must be set for playlists and animations to work properly
    bl::engine::Configuration::set("blib.playlist.song_path", MusicPath);
    bl::engine::Configuration::set("blib.animation.spritesheet_path", SpritesheetPath);
    return true;
}
