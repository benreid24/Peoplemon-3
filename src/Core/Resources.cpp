#include <Core/Resources.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace res
{
namespace
{
const std::string BundlePath = "data";

// TODO - flip when ready
constexpr bool MountBundle    = false;
constexpr bool DevModeEnabled = true;
} // namespace

void installDevLoaders() {
    if (!DevModeEnabled) {
        installProdLoaders();
        return;
    }

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
    if (MountBundle) {
        if (!bl::resource::FileSystem::useBundle(BundlePath)) {
            BL_LOG_CRITICAL << "Unable to mount resource bundles";
            std::exit(1);
        }
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
    // TODO - setup bundler config and create file handlers
    return false;
}

} // namespace res
} // namespace core
