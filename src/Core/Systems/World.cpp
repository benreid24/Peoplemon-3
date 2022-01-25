#include <Core/Systems/World.hpp>

#include <Core/Resources.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
using Serializer = bl::serial::json::Serializer<World>;
}
World::World(Systems& o)
: owner(o) {}

World::~World() {
    if (currentMap) currentMap->exit(owner, "GameClosing");
}

void World::init() { owner.engine().eventBus().subscribe(this); }

bool World::switchMaps(const std::string& file, int spawn) {
    if (file == "LastMap") {
        if (!previousMap) {
            if (prevMapFile.empty()) {
                BL_LOG_ERROR << "No previous map to return to";
                return false;
            }
            previousMap = Resources::maps().load(prevMapFile).data;
            if (!previousMap) return false;
        }

        const component::Position ppos = prevPlayerPos;
        prevPlayerPos                  = owner.player().position();

        currentMap->exit(owner, previousMap->name());
        owner.engine().entities().clear();
        if (!previousMap->enter(owner, 0, currentMap->name(), ppos)) return false;
        std::swap(currentMap, previousMap);
        std::swap(prevMapFile, currentMapFile);
    }
    else {
        previousMap = Resources::maps().load(file).data;
        if (!previousMap) return false;
        prevMapFile    = currentMapFile;
        currentMapFile = file;

        if (currentMap) {
            currentMap->exit(owner, previousMap->name());
            prevPlayerPos = owner.player().position();
            owner.engine().entities().clear();
        }
        std::swap(currentMap, previousMap);
        if (!currentMap->enter(
                owner, spawn, previousMap ? previousMap->name() : "NoPrevious", prevPlayerPos))
            return false;
    }
    return true;
}

map::Map& World::activeMap() { return *currentMap; }

const map::Map& World::activeMap() const { return *currentMap; }

void World::update(float dt) { currentMap->update(owner, dt); }

void World::observe(const event::GameSaving& save) {
    playerPos = owner.player().position();
    Serializer::serializeInto(save.saveData, "world", *this);
}

void World::observe(const event::GameLoading& load) {
    if (!Serializer::deserializeFrom(load.saveData, "world", *this)) {
        load.failMessage = "Failed to load world data from save";
        return;
    }

    currentMap = Resources::maps().load(currentMapFile).data;
    if (!currentMap) {
        load.failMessage = "Failed to load map: " + currentMapFile;
        return;
    }
}

bool World::finishLoad() { return currentMap->enter(owner, 0, prevMapFile, playerPos); }

} // namespace system
} // namespace core
