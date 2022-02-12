#ifndef CORE_FILES_GAMESAVE_HPP
#define CORE_FILES_GAMESAVE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Player/Bag.hpp>
#include <Core/Player/Gender.hpp>

namespace core
{
namespace file
{
/**
 * @brief Represents a game save and provides functionality to load and save
 *
 * @ingroup Files
 *
 */
struct GameSave {
    /// Local timestamp that the save was modified on
    unsigned long long saveTime;

    /// The name of the save
    std::string saveName;

    /// Stores pointers to the actual data to save/load from
    struct InteractDataPointers {
        std::unordered_map<std::string, std::unordered_set<std::string>>* talkedto;
        std::unordered_set<std::string>* convFlags;
    } interaction;

    /// Stores pointers to the actual data to save/load from
    struct PlayerDataPointers {
        std::string* playerName;
        player::Gender* sex;
        player::Bag* inventory;
        long* monei;
        std::vector<pplmn::OwnedPeoplemon>* peoplemon;
    } player;

    /// Stores pointers to the actual data to save/load from
    struct WorldDataPointers {
        std::string* currentMap;
        std::string* prevMap;
        component::Position* playerPos;
        component::Position* prevPlayerPos;
    } world;

    /// Stores pointers to the actual data to save/load from
    struct ScriptDataPointers {
        std::unordered_map<std::string, bl::script::Value>* entries;
    } scripts;

    /**
     * @brief Initializes all pointers to the local members
     *
     */
    GameSave();

    /**
     * @brief Lists all saves in the save directory
     *
     * @param result Vector to populate with found saves
     */
    static void listSaves(std::vector<GameSave>& result);

    /**
     * @brief Saves the game. Fires an event::GameSaveInitializing event for systems to add their
     * data to the save before it is written
     *
     * @param name The name of the player
     * @param bus Event bus to fire the GameSaveInitializing event on. May be nullptr
     * @return True if the game could be saved, false on error
     */
    static bool saveGame(const std::string& name, bl::event::Dispatcher& bus);

    /**
     * @brief Loads the save represented by this object and fires an event::GameSaveLoaded event to
     *        allow systems to populate their data
     *
     * @param bus Event bus to fire the GameSaveLoaded event on. May be nullptr
     * @return True on success, false on error
     */
    bool load(bl::event::Dispatcher* bus);

    /**
     * @brief Saves the data back to the file it was loaded from
     *
     */
    void editorSave();

    /**
     * @brief Deletes the game save
     *
     * @return True on success, false on error
     *
     */
    bool remove() const;

    /**
     * @brief For sorting
     *
     * @param rhs The save to compare to
     * @return True if should come first, false if not
     */
    bool operator<(const GameSave& rhs) const;

    /**
     * @brief Instantiates the save data in this object and sets all the pointers to it. Useful for
     *        loading a save without populating the game systems
     *
     */
    void useLocalData();

private:
    struct Data {
        std::unordered_map<std::string, std::unordered_set<std::string>> talkedto;
        std::unordered_set<std::string> convFlags;
        std::string playerName;
        player::Gender sex;
        player::Bag inventory;
        long monei;
        std::vector<pplmn::OwnedPeoplemon> peoplemon;
        std::string currentMap;
        std::string prevMap;
        component::Position playerPos;
        component::Position prevPlayerPos;
        std::unordered_map<std::string, bl::script::Value> entries;
    };

    std::optional<Data> localData;
    std::string sourceFile;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
namespace json
{
template<>
struct SerializableObject<core::file::GameSave::InteractDataPointers>
: public SerializableObjectBase {
    using I = core::file::GameSave::InteractDataPointers;
    using M = std::unordered_map<std::string, std::unordered_set<std::string>>;
    using F = std::unordered_set<std::string>;

    SerializableField<I, M*> talkedTo;
    SerializableField<I, F*> flags;

    SerializableObject()
    : talkedTo("talked", *this, &I::talkedto)
    , flags("flags", *this, &I::convFlags) {}
};

template<>
struct SerializableObject<core::file::GameSave::WorldDataPointers> : SerializableObjectBase {
    using World = core::file::GameSave::WorldDataPointers;
    using Pos   = core::component::Position;

    SerializableField<World, std::string*> currentMap;
    SerializableField<World, std::string*> prevMap;
    SerializableField<World, Pos*> playerPos;
    SerializableField<World, Pos*> prevPlayerPos;

    SerializableObject()
    : currentMap("current", *this, &World::currentMap)
    , prevMap("previous", *this, &World::prevMap)
    , playerPos("position", *this, &World::playerPos)
    , prevPlayerPos("prevPos", *this, &World::prevPlayerPos) {}
};

template<>
struct SerializableObject<core::file::GameSave::PlayerDataPointers>
: public SerializableObjectBase {
    using Player = core::file::GameSave::PlayerDataPointers;

    SerializableField<Player, std::string*> name;
    SerializableField<Player, core::player::Gender*> gender;
    SerializableField<Player, core::player::Bag*> bag;
    SerializableField<Player, std::vector<core::pplmn::OwnedPeoplemon>*> peoplemon;
    SerializableField<Player, long*> money;

    SerializableObject()
    : name("name", *this, &Player::playerName)
    , gender("gender", *this, &Player::sex)
    , bag("bag", *this, &Player::inventory)
    , peoplemon("peoplemon", *this, &Player::peoplemon)
    , money("money", *this, &Player::monei) {}
};

template<>
struct SerializableObject<core::file::GameSave::ScriptDataPointers>
: public SerializableObjectBase {
    using S = core::file::GameSave::ScriptDataPointers;
    using M = std::unordered_map<std::string, bl::script::Value>;

    SerializableField<S, M*> entries;

    SerializableObject()
    : entries("saveEntries", *this, &S::entries) {}
};

template<>
struct SerializableObject<core::file::GameSave> : public SerializableObjectBase {
    using GS = core::file::GameSave;
    SerializableField<GS, unsigned long long> saveTime;
    SerializableField<GS, std::string> saveName;
    SerializableField<GS, GS::PlayerDataPointers> player;
    SerializableField<GS, GS::InteractDataPointers> interaction;
    SerializableField<GS, GS::WorldDataPointers> world;
    SerializableField<GS, GS::ScriptDataPointers> script;

    SerializableObject()
    : saveTime("saveTime", *this, &GS::saveTime)
    , saveName("name", *this, &GS::saveName)
    , player("player", *this, &GS::player)
    , interaction("interaction", *this, &GS::interaction)
    , world("world", *this, &GS::world)
    , script("script", *this, &GS::scripts) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
