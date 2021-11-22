#include <Core/Properties.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Files/Util.hpp>
#include <BLIB/Resources.hpp>

namespace core
{
namespace
{
const std::string ConfigFile = "configuration.cfg";
bool inEditor                = false;

namespace defaults
{
const std::string WindowIconFile = "Resources/Images/icon.png";
constexpr int WindowWidth        = 800;
constexpr int WindowHeight       = 600;

constexpr int PixelsPerTile    = 32;
constexpr int ExtraRenderTiles = 10;
constexpr int LightingWidth    = 25;
constexpr int LightingHeight   = 19;

const std::string MenuImagePath   = "Resources/Images/Menus";
const std::string SpritesheetPath = "Resources/Images/Spritesheets";
const std::string MusicPath       = "Resources/Audio/Music";
const std::string PlaylistPath    = "Resources/Audio/Playlists";
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
const std::string SnowFlakeFile   = "Resources/Images/Weather/snowflake.png";
const std::string FogFile         = "Resources/Images/Weather/fog.png";
const std::string SandPatchFile   = "Resources/Images/Weather/sandMain.png";
const std::string SandSwirlFile   = "Resources/Images/Weather/sandSwirl.png";

constexpr float FrequentThunderMinInt   = 8.f;
constexpr float FrequentThunderMaxInt   = 15.f;
constexpr float InfrequentThunderMinInt = 15.f;
constexpr float InfrequentThunderMaxInt = 40.f;

constexpr unsigned int LightRainDropCount  = 700;
constexpr unsigned int HardRainDropCount   = 1300;
constexpr unsigned int LightSnowFlakeCount = 1500;
constexpr unsigned int HardSnowFlakeCount  = 4000;

constexpr unsigned int ThickFogAlpha = 135;
constexpr unsigned int ThinFogAlpha  = 90;

constexpr int LightRainLightModifier = 22;
constexpr int HardRainLightModifier  = 35;
constexpr int LightSnowLightModifier = 15;
constexpr int HardSnowLightModifier  = 30;
constexpr int ThinFogLightModifier   = 20;
constexpr int ThickFogLightModifier  = 50;
constexpr int SunnyLightModifier     = 20;
constexpr int SandstormLightModifier = 30;

const std::string NpcFileExtension          = "npc";
const std::string NpcPath                   = "Resources/Characters/NPCs";
const std::string TrainerFileExtension      = "tnr";
const std::string TrainerPath               = "Resources/Characters/Trainers";
const std::string ConversationPath          = "Resources/Characters/Conversations";
const std::string ConversationFileExtension = "conv";

const std::string CharacterAnimationPath = "Resources/Characters/Animations";
constexpr float CharacterMoveSpeed       = 81.f;
constexpr float FastCharacterMoveSpeed   = 205.f;

const std::string BoyPlayerAnims  = bl::file::Util::joinPath(CharacterAnimationPath, "BoyPlayer");
const std::string GirlPlayerAnims = bl::file::Util::joinPath(CharacterAnimationPath, "GirlPlayer");

const float CharacterSpinPeriod = 1.75f;

const std::string ItemMetadataFile = "Resources/Config/items.json";
const std::string ItemSprite       = "Resources/Images/item.png";

const float GhostWriterSpeed   = 16.f;
const std::string TextboxFile  = "Resources/Images/Menus/HUD/messageBox.png";
const unsigned int HudFontSize = 20;

const std::string ScriptPath = "Resources/Scripts";

constexpr float ScreenFadePeriod = 2.f;

const std::string PeoplemonDBFile = "Resources/Config/peoplemon.db";
const std::string MoveDBFile      = "Resources/Config/moves.db";

} // namespace defaults

bl::resource::Resource<sf::Font>::Ref menuFont;
} // namespace

bool Properties::load(bool ie) {
    inEditor = ie;

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
    bl::engine::Configuration::set("core.music.playlist_path", defaults::PlaylistPath);

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
    bl::engine::Configuration::set("core.weather.snowflake", defaults::SnowFlakeFile);
    bl::engine::Configuration::set("core.weather.fog", defaults::FogFile);
    bl::engine::Configuration::set("core.weather.sandpatch", defaults::SandPatchFile);
    bl::engine::Configuration::set("core.weather.sandswirl", defaults::SandSwirlFile);

    bl::engine::Configuration::set("core.weather.thunder.freq.min",
                                   defaults::FrequentThunderMinInt);
    bl::engine::Configuration::set("core.weather.thunder.freq.max",
                                   defaults::FrequentThunderMaxInt);
    bl::engine::Configuration::set("core.weather.thunder.infreq.min",
                                   defaults::InfrequentThunderMinInt);
    bl::engine::Configuration::set("core.weather.thunder.infreq.max",
                                   defaults::InfrequentThunderMaxInt);

    bl::engine::Configuration::set("core.weather.lightrain_particles",
                                   defaults::LightRainDropCount);
    bl::engine::Configuration::set("core.weather.hardrain_particles", defaults::HardRainDropCount);
    bl::engine::Configuration::set("core.weather.lightsnow_particles",
                                   defaults::LightSnowFlakeCount);
    bl::engine::Configuration::set("core.weather.hardsnow_particles", defaults::HardSnowFlakeCount);

    bl::engine::Configuration::set("core.weather.fog.thick_alpha", defaults::ThickFogAlpha);
    bl::engine::Configuration::set("core.weather.fog.thin_alpha", defaults::ThinFogAlpha);

    bl::engine::Configuration::set("core.weather.lightrain.light",
                                   defaults::LightRainLightModifier);
    bl::engine::Configuration::set("core.weather.hardrain.light", defaults::HardRainLightModifier);
    bl::engine::Configuration::set("core.weather.lightsnow.light",
                                   defaults::LightSnowLightModifier);
    bl::engine::Configuration::set("core.weather.hardsnow.light", defaults::HardSnowLightModifier);
    bl::engine::Configuration::set("core.weather.thinfog.light", defaults::ThinFogLightModifier);
    bl::engine::Configuration::set("core.weather.thickfog.light", defaults::ThickFogLightModifier);
    bl::engine::Configuration::set("core.weather.sunny.light", defaults::SunnyLightModifier);
    bl::engine::Configuration::set("core.weather.sandstorm.light",
                                   defaults::SandstormLightModifier);

    bl::engine::Configuration::set("core.npc.extension", defaults::NpcFileExtension);
    bl::engine::Configuration::set("core.npc.path", defaults::NpcPath);
    bl::engine::Configuration::set("core.trainer.extension", defaults::TrainerFileExtension);
    bl::engine::Configuration::set("core.trainer.path", defaults::TrainerPath);
    bl::engine::Configuration::set("core.conversation.path", defaults::ConversationPath);
    bl::engine::Configuration::set("core.conversation.extension",
                                   defaults::ConversationFileExtension);

    bl::engine::Configuration::set("core.character.animpath", defaults::CharacterAnimationPath);
    bl::engine::Configuration::set("core.character.speed", defaults::CharacterMoveSpeed);
    bl::engine::Configuration::set("core.character.fastspeed", defaults::FastCharacterMoveSpeed);

    bl::engine::Configuration::set("core.player.boy_anims", defaults::BoyPlayerAnims);
    bl::engine::Configuration::set("core.player.girl_anims", defaults::GirlPlayerAnims);

    bl::engine::Configuration::set("core.character.spin_period", defaults::CharacterSpinPeriod);

    bl::engine::Configuration::set("core.items.config", defaults::ItemMetadataFile);
    bl::engine::Configuration::set("core.items.sprite", defaults::ItemSprite);

    bl::engine::Configuration::set("blib.interface.ghost_speed", defaults::GhostWriterSpeed);
    bl::engine::Configuration::set("core.hud.textbox_file", defaults::TextboxFile);
    bl::engine::Configuration::set("core.hud.font_size", defaults::HudFontSize);

    bl::engine::Configuration::set("blib.script.path", defaults::ScriptPath);

    bl::engine::Configuration::set("game.main.fadeout", defaults::ScreenFadePeriod);

    bl::engine::Configuration::set("core.pplmn.dbfile", defaults::PeoplemonDBFile);
    bl::engine::Configuration::set("core.moves.dbfile", defaults::MoveDBFile);

    if (!bl::engine::Configuration::load(ConfigFile)) {
        BL_LOG_INFO << "Failed to load configuration file, using defaults";
    }
    bl::engine::Configuration::log();
    bl::engine::Configuration::save(ConfigFile); // ensure defaults saved if changed

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

bool Properties::InEditor() { return inEditor; }

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

const std::string& Properties::PlaylistPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.music.playlist_path", defaults::PlaylistPath);
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

const std::string& Properties::SnowFlakeFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.snowflake", defaults::SnowFlakeFile);
    return val;
}

const std::string& Properties::FogFile() {
    static const std::string val =
        bl::engine::Configuration::getOrDefault<std::string>("core.weather.fog", defaults::FogFile);
    return val;
}

const std::string& Properties::SandPatchFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.sandpatch", defaults::SandPatchFile);
    return val;
}

const std::string& Properties::SandSwirlFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.weather.sandswirl", defaults::SandSwirlFile);
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

unsigned int Properties::LightSnowParticleCount() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.lightsnow_particles",
                                                                 defaults::LightSnowFlakeCount);
}

unsigned int Properties::HardSnowParticleCount() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.hardsnow_particles",
                                                                 defaults::HardSnowFlakeCount);
}

float Properties::FrequentThunderMinInterval() {
    return bl::engine::Configuration::getOrDefault<float>("core.weather.thunder.freq.min",
                                                          defaults::FrequentThunderMinInt);
}

float Properties::FrequentThunderMaxInterval() {
    return bl::engine::Configuration::getOrDefault<float>("core.weather.thunder.freq.max",
                                                          defaults::FrequentThunderMaxInt);
}

float Properties::InfrequentThunderMinInterval() {
    return bl::engine::Configuration::getOrDefault<float>("core.weather.thunder.infreq.min",
                                                          defaults::InfrequentThunderMinInt);
}

float Properties::InfrequentThunderMaxInterval() {
    return bl::engine::Configuration::getOrDefault<float>("core.weather.thunder.infreq.max",
                                                          defaults::InfrequentThunderMaxInt);
}

unsigned int Properties::ThickFogAlpha() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.fog.thick_alpha",
                                                                 defaults::ThickFogAlpha);
}

unsigned int Properties::ThinFogAlpha() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.weather.fog.thin_alpha",
                                                                 defaults::ThinFogAlpha);
}

int Properties::LightRainLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.lightrain.light",
                                                        defaults::LightRainLightModifier);
}

int Properties::HardRainLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.hardrain.light",
                                                        defaults::HardRainLightModifier);
}

int Properties::LightSnowLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.lightsnow.light",
                                                        defaults::LightSnowLightModifier);
}

int Properties::HardSnowLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.hardsnow.light",
                                                        defaults::HardSnowLightModifier);
}

int Properties::ThinFogLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.thinfog.light",
                                                        defaults::ThinFogLightModifier);
}

int Properties::ThickFogLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.thickfog.light",
                                                        defaults::ThickFogLightModifier);
}

int Properties::SunnyLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.sunny.light",
                                                        defaults::SunnyLightModifier);
}

int Properties::SandstormLightModifier() {
    return bl::engine::Configuration::getOrDefault<int>("core.weather.sandstorm.light",
                                                        defaults::SandstormLightModifier);
}

const std::string& Properties::NpcFileExtension() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.npc.extension", defaults::NpcFileExtension);
    return val;
}

const std::string& Properties::NpcPath() {
    static const std::string val =
        bl::engine::Configuration::getOrDefault<std::string>("core.npc.path", defaults::NpcPath);
    return val;
}

const std::string& Properties::TrainerFileExtension() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.trainer.extension", defaults::TrainerFileExtension);
    return val;
}

const std::string& Properties::TrainerPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.trainer.path", defaults::TrainerPath);
    return val;
}

const std::string& Properties::ConversationPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.conversation.path", defaults::ConversationPath);
    return val;
}

const std::string& Properties::ConversationFileExtension() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.conversation.extension", defaults::ConversationFileExtension);
    return val;
}

const std::string& Properties::CharacterAnimationPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.character.animpath", defaults::CharacterAnimationPath);
    return val;
}

float Properties::CharacterMoveSpeed() {
    return bl::engine::Configuration::getOrDefault<float>("core.character.speed",
                                                          defaults::CharacterMoveSpeed);
}

float Properties::FastCharacterMoveSpeed() {
    return bl::engine::Configuration::getOrDefault<float>("core.character.fastspeed",
                                                          defaults::FastCharacterMoveSpeed);
}

const std::string& Properties::PlayerAnimations(player::Gender g) {
    switch (g) {
    case player::Gender::Boy:
        return bl::engine::Configuration::get<std::string>("core.player.boy_anims");
    case player::Gender::Girl:
        return bl::engine::Configuration::get<std::string>("core.player.girl_anims");
    default:
        BL_LOG_ERROR << "Unknown player gender: " << static_cast<int>(g);
        return defaults::BoyPlayerAnims;
    }
}

float Properties::CharacterSpinPeriod() {
    return bl::engine::Configuration::getOrDefault<float>("core.character.spin_period",
                                                          defaults::CharacterSpinPeriod);
}

const std::string& Properties::ItemMetadataFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.items.config", defaults::ItemMetadataFile);
    return val;
}

const std::string& Properties::ItemSprite() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.items.sprite", defaults::ItemSprite);
    return val;
}

float Properties::GhostWriterSpeed() {
    return bl::engine::Configuration::getOrDefault<float>("blib.interface.ghost_speed",
                                                          defaults::GhostWriterSpeed);
}

const std::string& Properties::TextboxFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.hud.textbox_file", defaults::TextboxFile);
    return val;
}

unsigned int Properties::HudFontSize() {
    return bl::engine::Configuration::getOrDefault<unsigned int>("core.hud.font_size",
                                                                 defaults::HudFontSize);
}

const std::string& Properties::ScriptPath() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "blib.script.path", defaults::ScriptPath);
    return val;
}

float Properties::ScreenFadePeriod() {
    static const float v = bl::engine::Configuration::getOrDefault<float>(
        "game.main.fadeout", defaults::ScreenFadePeriod);
    return v;
}

const std::string& Properties::PeoplemonDBFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.pplmn.dbfile", defaults::PeoplemonDBFile);
    return val;
}

const std::string& Properties::MoveDBFile() {
    static const std::string val = bl::engine::Configuration::getOrDefault<std::string>(
        "core.moves.dbfile", defaults::PeoplemonDBFile);
    return val;
}

} // namespace core
