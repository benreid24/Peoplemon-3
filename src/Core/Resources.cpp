#include <Core/Resources.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace res
{
namespace
{
const std::string BundlePath = "data";
}

void installDevLoaders() {
    TilesetManager::installLoader<TilesetDevLoader>();
    MapManager::installLoader<MapDevLoader>();

    // TODO - other loaders
}

void installProdLoaders() {
    if (!bl::resource::FileSystem::useBundle(BundlePath)) {
        BL_LOG_CRITICAL << "Unable to mount resource bundles";
        std::exit(1);
    }

    TilesetManager::installLoader<TilesetProdLoader>();
    MapManager::installLoader<MapProdLoader>();

    // TODO - other loaders
}

bool createBundles() {
    // TODO - setup bundler config and create file handlers
    return false;
}

} // namespace res
} // namespace core
