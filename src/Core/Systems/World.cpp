#include <Core/Systems/World.hpp>

#include <Core/Resources.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
World::World(Systems& o)
: owner(o) {}

bool World::switchMaps(const std::string& file, int spawn) {
    if (file == "LastMap") {
        // TODO - ensure that previous map is loaded on game save load or lazy load
        if (!previousMap) return false;
        currentMap->exit(owner);
        if (!previousMap->enter(owner, -1)) return false;
        std::swap(currentMap, previousMap);
        // TODO - reset player position
    }
    else {
        if (currentMap) {
            currentMap->exit(owner);
            previousMap = currentMap;
            // TODO - capture player position
        }
        currentMap = Resources::maps().load(file).data;
        if (!currentMap) return false;
        if (!currentMap->enter(owner, spawn)) return false;
    }
    return true;
}

map::Map& World::activeMap() { return *currentMap; }

const map::Map& World::activeMap() const { return *currentMap; }

void World::update(float dt) { currentMap->update(owner, dt); }

void World::render(sf::RenderTarget& target, float lag) {
    currentMap->render(target, lag);
    // TODO - render entities in y sort layer
}

} // namespace system
} // namespace core
