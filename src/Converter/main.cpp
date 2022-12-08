#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

using namespace core;

void convertConversations() {
    std::vector<std::string> files;
    bl::util::FileUtil::listDirectory(Properties::ConversationPath());
    for (const std::string& path : files) {
        file::Conversation conv;
        if (!conv.load(path)) {
            BL_LOG_ERROR << "Skipping bad conversation file: " << path;
            continue;
        }
        if (conv.save(path)) { BL_LOG_INFO << "Converted conversation: " << path; }
        else { BL_LOG_ERROR << "Failed to convert conversation: " << path; }
    }
}

void convertTrainers() {
    std::vector<std::string> files;
    bl::util::FileUtil::listDirectory(Properties::TrainerPath());
    for (const std::string& path : files) {
        file::Trainer trainer;
        if (!trainer.load(path)) {
            BL_LOG_ERROR << "Skipping bad trainer file: " << path;
            continue;
        }
        if (trainer.save(path)) { BL_LOG_INFO << "Converted trainer: " << path; }
        else { BL_LOG_ERROR << "Failed to convert trainer: " << path; }
    }
}

void convertNpcs() {
    std::vector<std::string> files;
    bl::util::FileUtil::listDirectory(Properties::NpcPath());
    for (const std::string& path : files) {
        file::NPC npc;
        if (!npc.load(path)) {
            BL_LOG_ERROR << "Skipping bad npc file: " << path;
            continue;
        }
        if (npc.save(path)) { BL_LOG_INFO << "Converted npc: " << path; }
        else { BL_LOG_ERROR << "Failed to convert npc: " << path; }
    }
}

void convertMaps() {
    std::vector<std::string> files;
    bl::util::FileUtil::listDirectory(Properties::MapPath());
    for (const std::string& path : files) {
        map::Map map;
        bl::serial::binary::InputFile input(path);
        if (!map.loadProd(input)) {
            BL_LOG_ERROR << "Skipping bad map file: " << path;
            continue;
        }
        if (map.save(path)) { BL_LOG_INFO << "Converted map: " << path; }
        else { BL_LOG_ERROR << "Failed to convert map: " << path; }
    }
}

void convertTilesets() {
    std::vector<std::string> files;
    bl::util::FileUtil::listDirectory(Properties::TilesetPath());
    for (const std::string& path : files) {
        map::Tileset tileset;
        bl::serial::binary::InputFile input(path);
        if (!tileset.loadProd(input)) {
            BL_LOG_ERROR << "Skipping bad tileset file: " << path;
            continue;
        }
        if (tileset.save(path)) { BL_LOG_INFO << "Converted tileset: " << path; }
        else { BL_LOG_ERROR << "Failed to convert tileset: " << path; }
    }
}

int main() {
    res::installProdLoaders();

    BL_LOG_INFO << "Converting item db";
    file::ItemDB itemDb;
    if (!itemDb.load()) {
        BL_LOG_ERROR << "Failed to load item db";
        return 1;
    }
    if (!itemDb.save()) {
        BL_LOG_ERROR << "Failed to convert item db";
        return 1;
    }
    BL_LOG_INFO << "Converted item db";

    BL_LOG_INFO << "Converting move db";
    file::MoveDB moveDb;
    if (!moveDb.load()) {
        BL_LOG_ERROR << "Failed to load move db";
        return 1;
    }
    if (!moveDb.save()) {
        BL_LOG_ERROR << "Failed to convert move db";
        return 1;
    }
    BL_LOG_INFO << "Converted move db";

    BL_LOG_INFO << "Converting peoplemon db";
    file::PeoplemonDB pplDb;
    if (!pplDb.load()) {
        BL_LOG_ERROR << "Failed to load peoplemon db";
        return 1;
    }
    if (!pplDb.save()) {
        BL_LOG_ERROR << "Failed to convert peoplemon db";
        return 1;
    }
    BL_LOG_INFO << "Converted peoplemon db";
}
