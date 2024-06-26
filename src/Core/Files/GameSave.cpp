#include <Core/Files/GameSave.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <Core/Events/GameSave.hpp>
#include <Core/Properties.hpp>
#include <chrono>

namespace core
{
namespace file
{
namespace
{
bool parseSaveName(const std::string& path, std::string& name, long& time) {
    std::string base = bl::util::FileUtil::getBaseName(path);
    name             = base;
    time             = 0;
    return true;
    // TODO - add method to stat file to blib fileutil
    /*
    const auto i     = base.find_last_of('_');
    if (i == std::string::npos) return false;
    name = base.substr(0, i);

    base = base.substr(i + 1);
    for (char t : base) {
        if (!std::isdigit(t)) return false;
    }
    time = std::atol(base.c_str());
    return true;*/
}

std::string tolower(const std::string& s) {
    std::string r(s);
    for (auto& c : r) { c = std::tolower(c); }
    return r;
}
} // namespace

void GameSave::listSaves(std::vector<GameSave>& result) {
    const std::vector<std::string> saveFiles = bl::util::FileUtil::listDirectory(
        Properties::SaveDirectory(), Properties::SaveExtension(), false);

    result.reserve(saveFiles.size());
    for (const std::string& file : saveFiles) {
        std::string name;
        long time;
        if (!parseSaveName(file, name, time)) {
            BL_LOG_ERROR << "Bad save file: " << file;
            continue;
        }

        result.emplace_back();
        result.back().saveName   = std::move(name);
        result.back().saveTime   = time;
        result.back().sourceFile = file;
    }

    std::sort(result.begin(), result.end());
}

bool GameSave::saveGame(const std::string& name) {
    GameSave save;
    save.saveName = name;
    save.saveTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (!Properties::InEditor()) {
        bl::event::Dispatcher::dispatch<event::GameSaveInitializing>({save, true});
    }

    const std::string file           = filename(name);
    bl::serial::json::Value data     = bl::serial::json::Serializer<GameSave>::serialize(save);
    bl::serial::json::Group& allData = *data.getAsGroup();
    bl::serial::json::saveToFile(file, allData);
    BL_LOG_INFO << "Saved game to: " << file;
    return true; // TODO - modify above to return boolean
}

bool GameSave::loadFromFile(const std::string& file) {
    GameSave save;
    save.sourceFile = file;
    return save.load();
}

void GameSave::editorSave() {
    bl::serial::json::Value data     = bl::serial::json::Serializer<GameSave>::serialize(*this);
    bl::serial::json::Group& allData = *data.getAsGroup();
    bl::serial::json::saveToFile(filename(*player.playerName), allData);
}

bool GameSave::load() {
    bl::serial::json::Group data;
    if (!bl::serial::json::loadFromFile(sourceFile, data)) {
        BL_LOG_ERROR << "Failed to parse game save";
        return false;
    }
    if (!Properties::InEditor()) {
        bl::event::Dispatcher::dispatch<event::GameSaveInitializing>({*this, false});
    }
    if (!bl::serial::json::Serializer<GameSave>::deserialize(*this, {data})) return false;

    if (!Properties::InEditor()) {
        event::GameSaveLoaded finish{""};
        bl::event::Dispatcher::dispatch<event::GameSaveLoaded>(finish);
        if (!finish.failMessage.empty()) {
            BL_LOG_ERROR << "Failed to load save file'" << sourceFile
                         << "': " << finish.failMessage;
            return false;
        }
    }

    return true;
}

bool GameSave::remove() const { return bl::util::FileUtil::deleteFile(sourceFile); }

bool GameSave::operator<(const GameSave& rhs) const {
    if (saveTime < rhs.saveTime) return true;
    return !(tolower(saveName) < tolower(rhs.saveName));
}

void GameSave::useLocalData() {
    localData.emplace();
    auto& d = localData.value();

    player.inventory              = &d.inventory;
    player.monei                  = &d.monei;
    player.peoplemon              = &d.peoplemon;
    player.playerName             = &d.playerName;
    player.sex                    = &d.sex;
    player.whiteoutMap            = &d.whiteoutMap;
    player.whiteoutSpawn          = &d.whiteoutSpawn;
    player.repelSteps             = &d.repelSteps;
    player.storage                = &d.storage;
    player.visitedTowns           = &d.visitedTowns;
    player.seenPeoplemon          = &d.seenPeoplemon;
    player.firstSightingLocations = &d.firstSightingLocations;

    interaction.convFlags = &d.convFlags;
    interaction.talkedto  = &d.talkedto;
    scripts.entries       = &d.entries;

    world.currentMap    = &d.currentMap;
    world.playerPos     = &d.playerPos;
    world.prevMap       = &d.prevMap;
    world.prevPlayerPos = &d.prevPlayerPos;

    clock.time = &d.clockTime;

    trainers.defeated = &d.defeatedTrainers;
}

GameSave::GameSave() {
    player.inventory              = nullptr;
    player.monei                  = nullptr;
    player.peoplemon              = nullptr;
    player.playerName             = nullptr;
    player.sex                    = nullptr;
    player.whiteoutMap            = nullptr;
    player.whiteoutSpawn          = nullptr;
    player.repelSteps             = nullptr;
    player.storage                = nullptr;
    player.visitedTowns           = nullptr;
    player.seenPeoplemon          = nullptr;
    player.firstSightingLocations = nullptr;

    interaction.convFlags = nullptr;
    interaction.talkedto  = nullptr;
    scripts.entries       = nullptr;

    world.currentMap    = nullptr;
    world.playerPos     = nullptr;
    world.prevMap       = nullptr;
    world.prevPlayerPos = nullptr;

    clock.time = nullptr;

    trainers.defeated = nullptr;
}

std::string GameSave::filename(const std::string& name) {
    return bl::util::FileUtil::joinPath(Properties::SaveDirectory(), name) + "." +
           Properties::SaveExtension();
}

} // namespace file
} // namespace core
