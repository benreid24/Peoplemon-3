#ifndef CORE_SYSTEMS_WORLD_HPP
#define CORE_SYSTEMS_WORLD_HPP

#include <Core/Maps/Map.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief System for managing the current map and previous maps
 *
 * @ingroup Systems
 *
 */
class World {
public:
    /**
     * @brief Creates the world system
     *
     * @param systems A reference to the parent Systems object
     */
    World(Systems& systems);

    /**
     * @brief Switches the current map to the map in the given file
     *
     * @param newMap Filename of new map, or "LastMap" for previous map
     * @param spawnId The id to spawn at. Ignored if "LastMap" is specified
     * @return True if the new map could be loaded and switched to, false on error
     */
    bool switchMaps(const std::string& newMap, int spawnId);

    /**
     * @brief Returns a reference to the active map
     *
     */
    map::Map& activeMap();

    /**
     * @brief Returns a const reference to the active map
     *
     */
    const map::Map& activeMap() const;

    /**
     * @brief Updates the current map
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    void update(float dt);

    /**
     * @brief Renders the current map and entities to the given target
     *
     * @param target Target to render to
     * @param lag Time elapsed not yet accounted for in update
     */
    void render(sf::RenderTarget& target, float lag);

private:
    Systems& owner;
    bl::resource::Resource<map::Map>::Ref currentMap;
    bl::resource::Resource<map::Map>::Ref previousMap;
};

} // namespace system
} // namespace core

#endif
