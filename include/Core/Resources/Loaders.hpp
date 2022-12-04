#ifndef CORE_RESOURCES_LOADERS_HPP
#define CORE_RESOURCES_LOADERS_HPP

#include <BLIB/Resources.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Maps/Tileset.hpp>

namespace core
{
namespace res
{
enum Mode { Dev, Prod };

/**
 * @brief Generic resource loader for conforming Peoplemon resource types. Resource types must be
 *        default constructable and have loadDev and loadProd methods
 *
 * @tparam T The type of resource to load
 * @tparam mode The mode to load in
 */
template<typename T, Mode mode>
struct PeoplemonLoader : public bl::resource::LoaderBase<T> {
    using Ref = bl::resource::Resource<T>::Ref;

    virtual Ref load(const std::string& path, const std::vector<char>& data,
                     std::istream&) override {
        Ref ref(new T());
        // TODO - json in dev mode, binary in prod mode
        bl::serial::MemoryInputBuffer wrapper(data);
        bl::serial::binary::InputStream is(wrapper);
        if constexpr (mode == Dev) {
            if (!ref->loadDev(is)) {
                BL_LOG_ERROR << "Failed to load resource: " << path;
                return nullptr;
            }
        }
        else {
            if (!ref->loadProd(is)) {
                BL_LOG_ERROR << "Failed to load resource: " << path;
                return nullptr;
            }
        }
        return ref;
    }
};

using TilesetDevLoader  = PeoplemonLoader<map::Tileset, Dev>;
using TilesetProdLoader = PeoplemonLoader<map::Tileset, Prod>;

using MapDevLoader  = PeoplemonLoader<map::Map, Dev>;
using MapProdLoader = PeoplemonLoader<map::Map, Prod>;

// TODO - implement/refactor for characters, db files, scripts

} // namespace res
} // namespace core

#endif