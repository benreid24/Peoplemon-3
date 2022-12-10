#include <Core/Resources.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources/BundleHandlers.hpp>

namespace core
{
namespace res
{
namespace
{
const std::string BundlePath = "data";
} // namespace

void installDevLoaders() {
    TilesetManager::installLoader<TilesetDevLoader>();
    MapManager::installLoader<MapDevLoader>();
    ConversationManager::installLoader<ConversationDevLoader>();
    NpcManager::installLoader<NpcDevLoader>();
    TrainerManager::installLoader<TrainerDevLoader>();
    ItemDbManager::installLoader<ItemDBDevLoader>();
    MoveDbManager::installLoader<MoveDBDevLoader>();
    PeoplemonDbManager::installLoader<PeoplemonDBDevLoader>();
}

void installProdLoaders() {
    if (!bl::resource::FileSystem::useBundle(BundlePath)) {
        BL_LOG_CRITICAL << "Unable to mount resource bundles";
        std::exit(1);
    }

    TilesetManager::installLoader<TilesetProdLoader>();
    MapManager::installLoader<MapProdLoader>();
    ConversationManager::installLoader<ConversationProdLoader>();
    NpcManager::installLoader<NpcProdLoader>();
    TrainerManager::installLoader<TrainerProdLoader>();
    ItemDbManager::installLoader<ItemDBProdLoader>();
    MoveDbManager::installLoader<MoveDBProdLoader>();
    PeoplemonDbManager::installLoader<PeoplemonDBProdLoader>();
}

bool createBundles() {
    using namespace bl::resource;
    constexpr auto BundleAllFiles = bundle::BundleSource::BundleAllFiles;
    constexpr auto BundleForEachRecurse =
        bundle::BundleSource::CreateBundleForEachContainedRecursive;
    constexpr auto BundleEachTopLevel = bundle::BundleSource::CreateBundleForEachContained;

    Bundler bundler(
        bundle::Config(BundlePath)
            .addBundleSource({"Resources/Animations/Battle", BundleAllFiles})
            .addBundleSource({"Resources/Animations/Moves", BundleEachTopLevel})

            .addBundleSource({"Resources/Audio/Playlists", BundleForEachRecurse})
            .addBundleSource({"Resources/Audio/Music", BundleAllFiles})
            .addBundleSource({"Resources/Audio/Sounds", BundleAllFiles})

            .addBundleSource({"Resources/Maps/Maps", BundleForEachRecurse})
            .addBundleSource({"Resources/Maps/Tilesets", BundleForEachRecurse})
            .addBundleSource({"Resources/Characters/Trainers", BundleAllFiles})
            .addBundleSource({"Resources/Characters/NPCs", BundleAllFiles})
            .addBundleSource({"Resources/Characters/Animations", BundleAllFiles})

            .addBundleSource({"Resources/Config", BundleAllFiles})
            .addBundleSource({"Resources/Scripts", BundleAllFiles})
            .addBundleSource({"Resources/Fonts", BundleAllFiles})

            .addBundleSource({"Resources/Images/Menus", BundleEachTopLevel})
            .addBundleSource({"Resources/Images/Weather", BundleAllFiles})
            .addBundleSource({"Resources/Images/Battle", BundleAllFiles})
            .addBundleSource({"Resources/Images/Peoplemon", BundleAllFiles})
            .addBundleSource({"Resources/Images/Spritesheets", BundleAllFiles})
            .addBundleSource({"Resources/Images", BundleAllFiles})

            .addFileHandler<bundle::AnimationHandler>(".*\\.anim")
            .addFileHandler<bundle::PlaylistHandler>(".*\\.plst", true)
            .addFileHandler<PeoplemonBundleHandler<file::ItemDB>>(".*items\\.db")
            .addFileHandler<PeoplemonBundleHandler<file::MoveDB>>(".*moves\\.db")
            .addFileHandler<PeoplemonBundleHandler<file::PeoplemonDB>>(".*peoplemon\\.db")
            .addFileHandler<PeoplemonBundleHandler<file::Conversation>>(".*\\.conv")
            .addFileHandler<PeoplemonBundleHandler<file::NPC>>(".*\\.npc")
            .addFileHandler<PeoplemonBundleHandler<file::Trainer>>(".*\\.tnr")
            .addFileHandler<PeoplemonBundleHandler<map::Map>>(".*\\.map")
            .addFileHandler<PeoplemonBundleHandler<map::Tileset>>(".*\\.tlst")

            .withCatchAllDirectory("Resources")
            .build());
    return bundler.createBundles();
}

} // namespace res
} // namespace core
