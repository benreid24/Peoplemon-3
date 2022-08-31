#ifndef CORE_PROPERTIES_HPP
#define CORE_PROPERTIES_HPP

#include <BLIB/Media/Audio/AudioSystem.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Player/Gender.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>

/**
 * @defgroup Core
 * @brief Collection of core classes that do not belong anywhere else
 *
 */

/// Core classes and functionality for both the editor and game
namespace core
{
/**
 * @brief Wrapper around bl::engine::Configuration. Provides application configuration in variables
 *        that may be accessed without performing look ups, and hides the string config names
 *
 * @ingroup Core
 *
 */
class Properties : private bl::util::NonCopyable {
public:
    static const std::string& WindowIconFile();
    static int WindowWidth();
    static int WindowHeight();
    static bool InEditor();

    static const std::string& SaveDirectory();
    static const std::string& SaveExtension();
    static const std::string& ControlsFile();

    static int PixelsPerTile();
    static int ExtraRenderTiles();
    static int LightingWidthTiles();
    static int LightingHeightTiles();

    static const sf::Font& MenuFont();
    static const std::string& MenuImagePath();
    static const std::string& ImagePath();
    static const std::string& AnimationPath();
    static const std::string& SoundPath();

    static const std::string& SpritesheetPath();
    static const std::string& MusicPath();
    static const std::string& PlaylistPath();

    static const std::string& MapPath();
    static const std::string& TilesetPath();
    static const std::string& MapTilePath();
    static const std::string& MapAnimationPath();

    static const std::string& ThunderSoundFile();
    static const std::string& LightRainSoundFile();
    static const std::string& HardRainSoundFile();

    static const std::string& RainDropFile();
    static const std::string& RainSplash1File();
    static const std::string& RainSplash2File();
    static const std::string& SnowFlakeFile();
    static const std::string& FogFile();
    static const std::string& SandPatchFile();
    static const std::string& SandSwirlFile();

    static float FrequentThunderMinInterval();
    static float FrequentThunderMaxInterval();
    static float InfrequentThunderMinInterval();
    static float InfrequentThunderMaxInterval();

    static unsigned int LightRainParticleCount();
    static unsigned int HardRainParticleCount();
    static unsigned int LightSnowParticleCount();
    static unsigned int HardSnowParticleCount();

    static unsigned int ThinFogAlpha();
    static unsigned int ThickFogAlpha();

    static int LightRainLightModifier();
    static int HardRainLightModifier();
    static int LightSnowLightModifier();
    static int HardSnowLightModifier();
    static int ThinFogLightModifier();
    static int ThickFogLightModifier();
    static int SunnyLightModifier();
    static int SandstormLightModifier();

    static const std::string& NpcFileExtension();
    static const std::string& NpcPath();
    static const std::string& TrainerFileExtension();
    static const std::string& TrainerPath();
    static const std::string& ConversationPath();
    static const std::string& ConversationFileExtension();
    static const std::string& LegacyPeoplemonPath();

    static const std::string& CharacterAnimationPath();
    static float CharacterMoveSpeed();
    static float FastCharacterMoveSpeed();
    static float CharacterSpinPeriod();

    static const std::string& PlayerAnimations(player::Gender gender);

    static const std::string& ItemMetadataFile();
    static const std::string& ItemSprite();

    static float GhostWriterSpeed();
    static const std::string& TextboxFile();
    static unsigned int HudFontSize();

    static const std::string& ScriptPath();

    static float ScreenFadePeriod();

    static const std::string& PeoplemonDBFile();
    static const std::string& MoveDBFile();

    static sf::Color HPBarColor(float percent);
    static sf::Color HPBarColor(unsigned int hp, unsigned int maxHp);
    static bl::resource::Resource<sf::Texture>::Ref AilmentTexture(pplmn::Ailment ailment);
    static const std::string& PeoplemonImageFolder();

    static bl::audio::AudioSystem::Handle MenuMoveSound();
    static bl::audio::AudioSystem::Handle MenuMoveFailSound();
    static bl::audio::AudioSystem::Handle MenuBackSound();
    static const std::string& TrainerExclaimImage();
    static const std::string& TrainerExclaimSound();

    /**
     * @brief Loads from the config file and sets defaults. Must be called before using any
     *        properties
     *
     * @param inEditor True if in the editor, false if in the game
     * @return True if loaded without error, false for failure
     */
    static bool load(bool inEditor);

private:
    Properties() = default;
};

} // namespace core

#endif
