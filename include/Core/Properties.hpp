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
 * @brief Common core application properties here. We may want to load these from a file or they may
 *        simply be constants. Some properties may be loaded into the engine configuration store
 *
 * @ingroup Core
 *
 */
class Properties : private bl::util::NonCopyable {
public:
    static constexpr int DefaultWindowWidth  = 800;
    static constexpr int DefaultWindowHeight = 600;
    static constexpr int DefaultWidthTiles   = 25;
    static constexpr int DefaultHeightTiles  = 19;

    static constexpr int PixelsPerTile = 32;

    static const std::string MenuImagePath;
    static const std::string SpritesheetPath;
    static const std::string MusicPath;

    static const sf::Font& MenuFont();

    static bool load();

private:
    Properties() = default;
};

} // namespace core

#endif
