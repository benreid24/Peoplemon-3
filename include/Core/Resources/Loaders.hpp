#ifndef CORE_RESOURCES_LOADERS_HPP
#define CORE_RESOURCES_LOADERS_HPP

#include <BLIB/Resources.hpp>
#include <Core/Files/Conversation.hpp>
#include <Core/Files/ItemDB.hpp>
#include <Core/Files/MoveDB.hpp>
#include <Core/Files/NPC.hpp>
#include <Core/Files/PeoplemonDB.hpp>
#include <Core/Files/Trainer.hpp>
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
 * @ingroup CoreResources
 */
template<typename T, Mode mode>
struct PeoplemonLoader : public bl::resource::LoaderBase<T> {
    using Ref = typename bl::resource::Ref<T>;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      T& result) override {
        if constexpr (mode == Dev) {
            // worldmap exceeds git size limit in json so load in binary in dev mode
            if (path == "Resources/Maps/Maps/WorldMap.map") {
                bl::serial::MemoryInputBuffer wrapper(buffer, len);
                bl::serial::binary::InputStream is(wrapper);
                if (!result.loadProd(is)) {
                    BL_LOG_ERROR << "Failed to load resource: " << path;
                    return false;
                }
            }
            else {
                bl::util::BufferIstreamBuf buf(const_cast<char*>(buffer), len);
                std::istream is(&buf);
                if (!result.loadDev(is)) {
                    BL_LOG_ERROR << "Failed to load resource: " << path;
                    return false;
                }
            }
        }
        else {
            bl::serial::MemoryInputBuffer wrapper(buffer, len);
            bl::serial::binary::InputStream is(wrapper);
            if (!result.loadProd(is)) {
                BL_LOG_ERROR << "Failed to load resource: " << path;
                return false;
            }
        }
        return true;
    }
};

using TilesetDevLoader  = PeoplemonLoader<map::Tileset, Dev>;
using TilesetProdLoader = PeoplemonLoader<map::Tileset, Prod>;

using MapDevLoader  = PeoplemonLoader<map::Map, Dev>;
using MapProdLoader = PeoplemonLoader<map::Map, Prod>;

using ConversationDevLoader  = PeoplemonLoader<file::Conversation, Dev>;
using ConversationProdLoader = PeoplemonLoader<file::Conversation, Prod>;

using NpcDevLoader  = PeoplemonLoader<file::NPC, Dev>;
using NpcProdLoader = PeoplemonLoader<file::NPC, Prod>;

using TrainerDevLoader  = PeoplemonLoader<file::Trainer, Dev>;
using TrainerProdLoader = PeoplemonLoader<file::Trainer, Prod>;

using ItemDBDevLoader  = PeoplemonLoader<file::ItemDB, Dev>;
using ItemDBProdLoader = PeoplemonLoader<file::ItemDB, Prod>;

using PeoplemonDBDevLoader  = PeoplemonLoader<file::PeoplemonDB, Dev>;
using PeoplemonDBProdLoader = PeoplemonLoader<file::PeoplemonDB, Prod>;

using MoveDBDevLoader  = PeoplemonLoader<file::MoveDB, Dev>;
using MoveDBProdLoader = PeoplemonLoader<file::MoveDB, Prod>;

} // namespace res
} // namespace core

#endif
