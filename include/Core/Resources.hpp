#ifndef CORE_RESOURCES_HPP
#define CORE_RESOURCES_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Files/Conversation.hpp>
#include <Core/Files/NPC.hpp>
#include <Core/Files/Trainer.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Resources/Loaders.hpp>
#include <Core/Resources/RunWalkAnimations.hpp>
#include <Core/Resources/WalkAnimations.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

using TextureManager          = bl::resource::ResourceManager<sf::Texture>;
using ImageManager            = bl::resource::ResourceManager<sf::Image>;
using SoundManager            = bl::resource::ResourceManager<sf::SoundBuffer>;
using AnimationManager        = bl::resource::ResourceManager<bl::gfx::a2d::AnimationData>;
using FontManager             = bl::resource::ResourceManager<sf::VulkanFont>;
using MapManager              = bl::resource::ResourceManager<core::map::Map>;
using TilesetManager          = bl::resource::ResourceManager<core::map::Tileset>;
using ConversationManager     = bl::resource::ResourceManager<core::file::Conversation>;
using TrainerManager          = bl::resource::ResourceManager<core::file::Trainer>;
using NpcManager              = bl::resource::ResourceManager<core::file::NPC>;
using ItemDbManager           = bl::resource::ResourceManager<core::file::ItemDB>;
using MoveDbManager           = bl::resource::ResourceManager<core::file::MoveDB>;
using PeoplemonDbManager      = bl::resource::ResourceManager<core::file::PeoplemonDB>;
using WalkAnimationManager    = bl::resource::ResourceManager<core::res::WalkAnimations>;
using RunWalkAnimationManager = bl::resource::ResourceManager<core::res::RunWalkAnimations>;

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
