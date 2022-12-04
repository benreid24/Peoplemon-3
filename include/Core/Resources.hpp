#ifndef CORE_RESOURCES_HPP
#define CORE_RESOURCES_HPP

#include <BLIB/Media/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Resources/BundleHandlers.hpp>
#include <Core/Resources/Loaders.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

using TextureManager   = bl::resource::ResourceManager<sf::Texture>;
using ImageManager     = bl::resource::ResourceManager<sf::Image>;
using SoundManager     = bl::resource::ResourceManager<sf::SoundBuffer>;
using AnimationManager = bl::resource::ResourceManager<bl::gfx::AnimationData>;
using FontManager      = bl::resource::ResourceManager<sf::Font>;
using MapManager       = bl::resource::ResourceManager<core::map::Map>;
using TilesetManager   = bl::resource::ResourceManager<core::map::Tileset>;

/**
 * @addtogroup CoreResources
 * @ingroup Core
 * @brief Collection of utilities for resource bundling and management
 *
 */

namespace core
{
/// @brief Collection of utilities for resource bundling and management
namespace res
{
/**
 * @brief Sets up the BLIB resource managers in dev mode
 *
 * @ingroup CoreResources
 */
void installDevLoaders();

/**
 * @brief Sets up the BLIB resource managers in prod mode and mounts the resource bundles
 *
 * @ingroup CoreResources
 */
void installProdLoaders();

/**
 * @brief Creates the resource bundles
 *
 * @return True if the bundles could be created, false on error
 * @ingroup CoreResources
 */
bool createBundles();

} // namespace res
} // namespace core

#endif
