#ifndef CORE_RESOURCES_HPP
#define CORE_RESOURCES_HPP

#include <BLIB/Resources.hpp>

#include <Core/Maps/Map.hpp>
#include <Core/Maps/Tileset.hpp>

namespace core
{
/**
 * @brief Collection of resource manager singletons for Peoplemon specific resources
 *
 * @ingroup Core
 *
 */
class Resources {
public:
    /**
     * @brief Returns the tileset resource manager
     *
     */
    static bl::resource::Manager<map::Tileset>& tilesets();

    /**
     * @brief Returns the map resource manager
     *
     */
    static bl::resource::Manager<map::Map>& maps();

private:
    bl::resource::Manager<map::Tileset> _tilesets;
    bl::resource::Manager<map::Map> _maps;

    Resources();
    static Resources& get();
};

} // namespace core

#endif
