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
std::string nameToFile(const std::string& name) {
    return bl::util::FileUtil::joinPath(Properties::SaveDirectory(), name) + "." +
           Properties::SaveExtension();
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
        const bl::serial::json::Value data = bl::serial::json::loadFromFile(file);
        const auto* g                      = data.getAsGroup();
        if (!g) {
            BL_LOG_ERROR << "Bad save file: " << file;
            continue;
        }

        GameSave save;
        if (!bl::serial::json::Serializer<GameSave>::deserialize(save, data)) {
            BL_LOG_ERROR << "Failed to read save data: " << file;
            continue;
        }
        result.emplace_back(std::move(save));
    }

    std::sort(result.begin(), result.end());
}

bool GameSave::saveGame(const std::string& name, bl::event::Dispatcher& bus) {
    const std::string file = nameToFile(name);

    GameSave save;
    save.saveName = name;
    save.saveTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    bl::serial::json::Value data     = bl::serial::json::Serializer<GameSave>::serialize(save);
    bl::serial::json::Group& allData = *data.getAsGroup();
    bus.dispatch<event::GameSaving>({allData});

    bl::serial::json::saveToFile(file, allData);
    BL_LOG_INFO << "Saved game to: " << file;
    return true; // TODO - modify above to return boolean
}

bool GameSave::load(bl::event::Dispatcher& bus) const {
    const std::string file             = nameToFile(saveName);
    const bl::serial::json::Group data = bl::serial::json::loadFromFile(file);
    event::GameLoading loader{data, ""};
    bus.dispatch<event::GameLoading>(loader, false);
    if (!loader.failMessage.empty()) {
        BL_LOG_ERROR << "Failed to load save file'" << file << "': " << loader.failMessage;
        return false;
    }
    return true;
}

bool GameSave::remove() const {
    const std::string file = nameToFile(saveName);
    return bl::util::FileUtil::deleteFile(file);
}

bool GameSave::operator<(const GameSave& rhs) const {
    if (saveTime < rhs.saveTime) return true;
    return !(tolower(saveName) < tolower(rhs.saveName));
}

} // namespace file
} // namespace core
