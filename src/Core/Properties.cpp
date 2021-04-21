#include <Core/Properties.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Resources.hpp>

namespace core
{
namespace
{
namespace defaults
{
const std::string WindowIconFile = "Resources/Images/icon.png";
const int WindowWidth            = 800;
const int WindowHeight           = 600;

const int PixelsPerTile    = 32;
const int ExtraRenderTiles = 10;
const int LightingWidth    = 25;
const int LightingHeight   = 19;

const std::string MenuImagePath   = "Resources/Images/Menus";
const std::string SpritesheetPath = "Resources/Images/Spritesheets";
const std::string MusicPath       = "Resources/Audio/Music";
const std::string MenuFont        = "Resources/Fonts/Menu.ttf";

const std::string MapPath          = "Resources/Maps/Maps";
const std::string TilesetPath      = "Resources/Maps/Tilesets";
const std::string MapTilePath      = "Resources/Maps/Tiles/Sprites";
const std::string MapAnimationPath = "Resources/Maps/Tiles/Animations";

const std::string ThunderSoundFile   = "Resources/Audio/Sounds/Weather/thunder.wav";
const std::string LightRainSoundFile = "Resources/Audio/Sounds/Weather/lightRain.wav";
const std::string HardRainSoundFile  = "Resources/Audio/Sounds/Weather/hardRain.wav";

const std::string RainDropFile    = "Resources/Images/Weather/raindrop.png";
const std::string RainSplash1File = "Resources/Images/Weather/rainSplash1.png";
const std::string RainSplash2File = "Resources/Images/Weather/rainSplash2.png";

const unsigned int LightRainDropCount = 700;
const unsigned int HardRainDropCount  = 1300;

} // namespace defaults

bl::resource::Resource<sf::Font>::Ref menuFont;
} // namespace

bool Properties::load() {
    // These must be set for playlists and animations to work properly
    bl::engine::Configuration::set("blib.playlist.song_path", defaults::MusicPath);
    bl::engine::Configuration::set("blib.animation.spritesheet_path", defaults::SpritesheetPath);

    bl::engine::Configuration::set("core.window.icon", defaults::WindowIconFile);
    bl::engine::Configuration::set("core.window.width", defaults::WindowWidth);
    bl::engine::Configuration::set("core.window.height", defaults::WindowHeight);

    bl::engine::Configuration::set("core.render.pixels_per_tile", defaults::PixelsPerTile);
    bl::engine::Configuration::set("core.render.extra_render_tiles", defaults::ExtraRenderTiles);
    bl::engine::Configuration::set("core.render.lighting_width", defaults::LightingWidth);
    bl::engine::Configuration::set("core.render.lighting_height", defaults::LightingHeight);

    bl::engine::Configuration::set("core.menu.image_path", defaults::MenuImagePath);
    bl::engine::Configuration::set("core.menu.primary_font", defaults::MenuFont);
    bl::engine::Configuration::set("core.music.path", defaults::MusicPath);

    bl::engine::Configuration::set("core.map.path", defaults::MapPath);
    bl::engine::Configuration::set("core.map.tileset_path", defaults::TilesetPath);
    bl::engine::Configuration::set("core.map.tile_path", defaults::MapTilePath);
    bl::engine::Configuration::set("core.map.anim_path", defaults::MapAnimationPath);

    bl::engine::Configuration::set("core.weather.thunder", defaults::ThunderSoundFile);
    bl::engine::Configuration::set("core.weather.lightrain", defaults::LightRainSoundFile);
    bl::engine::Configuration::set("core.weather.hardrain", defaults::HardRainSoundFile);

    bl::engine::Configuration::set("core.weather.raindrop", defaults::RainDropFile);
    bl::engine::Configuration::set("core.weather.rainsplash1", defaults::RainSplash1File);
    bl::engine::Configuration::set("core.weather.rainsplash2", defaults::RainSplash2File);

    bl::engine::Configuration::set("core.weather.lightrain_particles",
                                   defaults::LightRainDropCount);
    bl::engine::Configuration::set("core.weather.hardrain_particles", defaults::HardRainDropCount);

    if (!bl::engine::Configuration::load("configuration.cfg")) {
        BL_LOG_INFO << "Failed to load configuration file, using defaults";
    }

    menuFont = bl::engine::Resources::fonts()
                   .load(bl::engine::Configuration::get<std::string>("core.menu.primary_font"))
                   .data;
    if (!menuFont) return false;

    return true;
}

const std::string& Properties::WindowIconFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.window.icon", defaults::WindowIconFile);
    return val;
}

int Properties::WindowWidth() {
    static const int val =
        bl::engine::Configuration::getOrDefault<int>("core.window.width", defaults::WindowWidth);
    return val;
}

int Properties::WindowHeight() {
    static const int val =
        bl::engine::Configuration::getOrDefault<int>("core.window.height", defaults::WindowHeight);
    return val;
}

int Properties::PixelsPerTile() {
    static const int val = bl::engine::Configuration::getOrDefault<int>(
        "core.render.pixels_per_tile", defaults::PixelsPerTile);
    return val;
}

int Properties::ExtraRenderTiles() {
    static const int val = bl::engine::Configuration::getOrDefault<int>(
        "core.render.extra_render_tiles", defaults::ExtraRenderTiles);
    return val;
}

int Properties::LightingWidthTiles() {
    static const int val = bl::engine::Configuration::getOrDefault<int>(
        "core.render.lighting_width", defaults::LightingWidth);
    return val;
}

int Properties::LightingHeightTiles() {
    static const int val = bl::engine::Configuration::getOrDefault<int>(
        "core.render.lighting_height", defaults::LightingHeight);
    return val;
}

const std::string& Properties::MenuImagePath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.menu.image_path", defaults::MenuImagePath);
    return val;
}

const std::string& Properties::SpritesheetPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "blib.animation.spritesheet_path", defaults::SpritesheetPath);
    return val;
}

const std::string& Properties::MusicPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.music.path", defaults::MusicPath);
    return val;
}

const std::string& Properties::MapPath() {
    static const std::string val =
        bl::engine::Configuration::getOrDefault<std::string>("core.map.path", defaults::MapPath);
    return val;
}

const std::string& Properties::TilesetPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.map.tileset_path", defaults::TilesetPath);
    return val;
}

const std::string& Properties::MapTilePath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.map.tile_path", defaults::MapTilePath);
    return val;
}

const std::string& Properties::MapAnimationPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.map.anim_path", defaults::MapAnimationPath);
    return val;
}

const sf::Font& Properties::MenuFont() { return *menuFont; }

const std::string& Properties::ThunderSoundFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.thunder", defaults::ThunderSoundFile);
    return val;
}

const std::string& Properties::LightRainSoundFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.lightrain", defaults::LightRainSoundFile);
    return val;
}

const std::string& Properties::HardRainSoundFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.hardrain", defaults::HardRainSoundFile);
    return val;
}

const std::string& Properties::RainDropFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.raindrop", defaults::RainDropFile);
    return val;
}

const std::string& Properties::RainSplash1File() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.rainsplash1", defaults::RainSplash1File);
    return val;
}

const std::string& Properties::RainSplash2File() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.rainsplash2", defaults::RainSplash2File);
    return val;
}

unsigned int Properties::LightRainParticleCount() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.lightrain_particles",
                                                                 defaults::LightRainDropCount);
}

unsigned int Properties::HardRainParticleCount() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.hardrain_particles",
                                                                 defaults::HardRainDropCount);
}

} // namespace core
