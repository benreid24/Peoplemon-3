#include <Core/Resources.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace
{
struct TilesetLoader : public bl::resource::Loader<map::Tileset> {
    virtual ~TilesetLoader() = default;

    virtual bl::resource::Resource<map::Tileset>::Ref load(const std::string& uri) override {
        bl::resource::Resource<map::Tileset>::Ref ref(new map::Tileset());
        if (!ref->load(uri)) {
            BL_LOG_ERROR << "Failed to load tileset: " << uri;
            return nullptr;
        }
        return ref;
    }
} tilesetLoader;

struct MapLoader : public bl::resource::Loader<map::Map> {
    virtual ~MapLoader() = default;

    virtual bl::resource::Resource<map::Map>::Ref load(const std::string& uri) override {
        bl::resource::Resource<map::Map>::Ref ref(new map::Map());
        if (!ref->load(uri)) {
            BL_LOG_ERROR << "Failed to load map: " << uri;
            return nullptr;
        }
        return ref;
    }
} mapLoader;

} // namespace

Resources::Resources()
: _tilesets(tilesetLoader)
, _maps(mapLoader) {}

bl::resource::Manager<map::Tileset>& Resources::tilesets() { return get()._tilesets; }

bl::resource::Manager<map::Map>& Resources::maps() { return get()._maps; }

Resources& Resources::get() {
    static Resources resources;
    return resources;
}

} // namespace core
