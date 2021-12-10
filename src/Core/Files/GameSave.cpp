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
} // namespace

void GameSave::listSaves(std::vector<GameSave>& result) {
    const std::vector<std::string> saveFiles = bl::util::FileUtil::listDirectory(
        Properties::SaveDirectory(), Properties::SaveExtension(), false);

    result.reserve(saveFiles.size());
    for (const std::string& file : saveFiles) {
        const bl::serial::json::Value data = bl::serial::json::loadFromFile(file);
        const auto g                       = data.getAsGroup();
        if (!g.has_value()) {
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
}

bool GameSave::saveGame(const std::string& name, bl::event::Dispatcher& bus) {
    const std::string file = nameToFile(name);

    GameSave save;
    save.saveName = name;
    save.saveTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    const auto data                 = bl::serial::json::Serializer<GameSave>::serialize(save);
    bl::serial::json::Group allData = data.getAsGroup().value();
    bus.dispatch<event::GameSaving>({allData});

    bl::serial::json::saveToFile(file, allData);
}

bool GameSave::load(bl::event::Dispatcher& bus) const {
    const std::string file             = nameToFile(saveName);
    const bl::serial::json::Group data = bl::serial::json::loadFromFile(file);
    event::GameLoading loader{data};
    bus.dispatch<event::GameLoading>(loader);
    if (!loader.failMessage.empty()) {
        BL_LOG_ERROR << "Failed to load save file'" << file << "': " << loader.failMessage;
        return false;
    }
    return true;
}

} // namespace file
} // namespace core
