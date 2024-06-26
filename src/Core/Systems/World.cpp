#include <Core/Systems/World.hpp>

#include <Core/Events/Maps.hpp>
#include <Core/Files/GameSave.hpp>
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

void World::init(bl::engine::Engine&) { bl::event::Dispatcher::subscribe(this); }

void World::earlyCleanup() {
    currentMap.release();
    previousMap.release();
}

bool World::switchMaps(const std::string& file, int spawn) {
    if (file == "LastMap") {
        if (!previousMap) {
            if (prevMapFile.empty()) {
                BL_LOG_ERROR << "No previous map to return to";
                return false;
            }
            previousMap = MapManager::load(map::Map::getMapFile(prevMapFile));
            if (!previousMap) return false;
        }

        const bl::tmap::Position ppos = prevPlayerPos;
        prevPlayerPos                 = owner.player().position();

        currentMap->exit(owner, previousMap->name());
        owner.engine().ecs().destroyAllWorldEntities();
        if (!previousMap->enter(owner, 0, currentMap->name(), ppos)) return false;
        std::swap(currentMap, previousMap);
        std::swap(prevMapFile, currentMapFile);
    }
    else {
        // ensure we always have a whiteout location
        if (owner.player().state().whiteoutMap.empty()) {
            owner.player().state().whiteoutMap   = file;
            owner.player().state().whiteoutSpawn = spawn;
        }

        previousMap = MapManager::load(map::Map::getMapFile(file));
        if (!previousMap) return false;
        prevMapFile    = currentMapFile;
        currentMapFile = file;

        if (currentMap) {
            currentMap->exit(owner, previousMap->name());
            prevPlayerPos = owner.player().position();
            owner.engine().ecs().destroyAllWorldEntities();
        }
        std::swap(currentMap, previousMap);
        if (!currentMap->enter(
                owner, spawn, previousMap ? previousMap->name() : "NoPrevious", prevPlayerPos))
            return false;
    }

    return true;
}

void World::whiteout(const std::string& map, int spawn) {
    BL_LOG_INFO << "Whiting out to " << map << " (" << spawn << ")";
    previousMap.release();
    prevMapFile.clear();
    bl::event::Dispatcher::dispatch<event::SwitchMapTriggered>({map, spawn});
}

void World::setWhiteoutMap(unsigned int spawn) {
    owner.player().state().whiteoutMap   = currentMapFile;
    owner.player().state().whiteoutSpawn = spawn;
}

map::Map& World::activeMap() { return *currentMap; }

const map::Map& World::activeMap() const { return *currentMap; }

void World::update(std::mutex&, float dt, float, float, float) {
    if (currentMap) { currentMap->update(dt); }
}

void World::observe(const event::GameSaveInitializing& save) {
    if (save.saving) { playerPos = owner.player().position(); }
    save.gameSave.world.currentMap    = &currentMapFile;
    save.gameSave.world.prevMap       = &prevMapFile;
    save.gameSave.world.playerPos     = &playerPos;
    save.gameSave.world.prevPlayerPos = &prevPlayerPos;
}

void World::observe(const event::GameSaveLoaded& load) {
    currentMap = MapManager::load(map::Map::getMapFile(currentMapFile));
    if (!currentMap) {
        load.failMessage = "Failed to load map: " + currentMapFile;
        return;
    }
    if (!currentMap->enter(owner, 0, prevMapFile, playerPos)) {
        load.failMessage = "Failed to enter map: " + currentMapFile;
    }
}

} // namespace system
} // namespace core
