#include <Core/Systems/World.hpp>

#include <Core/Resources.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
World::World(Systems& o)
: owner(o) {}

World::~World() {
    if (currentMap) currentMap->exit(owner, "GameClosing");
}

bool World::switchMaps(const std::string& file, int spawn) {
    if (file == "LastMap") {
        if (!previousMap) return false; // TODO - lazy load previous map from gamesave info

        currentMap->exit(owner, previousMap->name());
        owner.engine().entities().clear();
        if (!previousMap->enter(owner, 0, currentMap->name())) return false;
        std::swap(currentMap, previousMap);
        // TODO - reset player position
    }
    else {
        previousMap = Resources::maps().load(file).data;
        if (!previousMap) return false;

        if (currentMap) {
            currentMap->exit(owner, previousMap->name());
            // TODO - capture player position
            owner.engine().entities().clear();
        }
        std::swap(currentMap, previousMap);
        if (!currentMap->enter(owner, spawn, previousMap ? previousMap->name() : "NoPrevious"))
            return false;
    }
    return true;
}

map::Map& World::activeMap() { return *currentMap; }

const map::Map& World::activeMap() const { return *currentMap; }

void World::update(float dt) { currentMap->update(owner, dt); }

} // namespace system
} // namespace core
