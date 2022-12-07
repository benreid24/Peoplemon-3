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
    using Ref = typename bl::resource::Resource<T>::Ref;

    virtual Ref load(const std::string& path, const char* buffer, std::size_t len,
                     std::istream&) override {
        Ref ref(new T());
        constexpr bool DevEnabled = false; // TODO - flip when ready
        if constexpr (mode == Dev && DevEnabled) {
            bl::util::BufferIstreamBuf buf(const_cast<char*>(buffer), len);
            std::istream is(&buf);
            if (!ref->loadDev(is)) {
                BL_LOG_ERROR << "Failed to load resource: " << path;
                return nullptr;
            }
        }
        else {
            bl::serial::MemoryInputBuffer wrapper(buffer, len);
            bl::serial::binary::InputStream is(wrapper);
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
