#ifndef CORE_PROPERTIES_HPP
#define CORE_PROPERTIES_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics/Font.hpp>
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

    static int PixelsPerTile();
    static int ExtraRenderTiles();
    static int LightingWidthTiles();
    static int LightingHeightTiles();

    static const sf::Font& MenuFont();
    static const std::string& MenuImagePath();

    static const std::string& SpritesheetPath();
    static const std::string& MusicPath();

    static const std::string& MapPath();
    static const std::string& TilesetPath();
    static const std::string& MapTilePath();
    static const std::string& MapAnimationPath();

    /**
     * @brief Loads from the config file and sets defaults. Must be called before using any
     *        properties
     *
     * @return True if loaded without error, false for failure
     */
    static bool load();

private:
    Properties() = default;
};

} // namespace core

#endif
