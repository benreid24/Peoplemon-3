#ifndef CORE_FILES_GAMESAVE_HPP
#define CORE_FILES_GAMESAVE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Player/Bag.hpp>
#include <Core/Player/Gender.hpp>
#include <Core/Player/StorageSystem.hpp>
#include <Core/Systems/Clock.hpp>
#include <Core/Systems/Trainers.hpp>

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

    /// Same as the player name but always valid
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
        std::string* whiteoutMap;
        unsigned int* whiteoutSpawn;
        unsigned int* repelSteps;
        std::array<std::vector<pplmn::StoredPeoplemon>, player::StorageSystem::BoxCount>* storage;
        std::unordered_set<std::string>* visitedTowns;
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

    /// Stores the date in game
    struct ClockPointers {
        system::Clock::Time* time;
    } clock;

    /// Stores defeated trainer information
    struct TrainerPointers {
        std::unordered_set<std::string>* defeated;
    } trainers;

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
     * @brief Loads the game from the given save file
     *
     * @param sourceFile The file to load from
     * @param bus The event bus to use when loading
     * @return True on success, false on error
     */
    static bool loadFromFile(const std::string& sourceFile, bl::event::Dispatcher& bus);

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

    /**
     * @brief Returns the filename for the given save name
     *
     * @param saveName The name of the player in the save
     * @return The filename to load or save from
     *
     */
    static std::string filename(const std::string& saveName);

private:
    struct Data {
        std::unordered_map<std::string, std::unordered_set<std::string>> talkedto;
        std::unordered_set<std::string> convFlags;
        std::string playerName;
        player::Gender sex;
        player::Bag inventory;
        long monei;
        std::vector<pplmn::OwnedPeoplemon> peoplemon;
        std::string whiteoutMap;
        unsigned int whiteoutSpawn;
        unsigned int repelSteps;
        std::array<std::vector<pplmn::StoredPeoplemon>, player::StorageSystem::BoxCount> storage;
        std::unordered_set<std::string> visitedTowns;
        std::string currentMap;
        std::string prevMap;
        component::Position playerPos;
        component::Position prevPlayerPos;
        std::unordered_map<std::string, bl::script::Value> entries;
        system::Clock::Time clockTime;
        std::unordered_set<std::string> defeatedTrainers;
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
template<>
struct SerializableObject<core::file::GameSave::InteractDataPointers>
: public SerializableObjectBase {
    using I = core::file::GameSave::InteractDataPointers;
    using M = std::unordered_map<std::string, std::unordered_set<std::string>>;
    using F = std::unordered_set<std::string>;

    SerializableField<1, I, M*> talkedTo;
    SerializableField<2, I, F*> flags;

    SerializableObject()
    : talkedTo("talked", *this, &I::talkedto, SerializableFieldBase::Required{})
    , flags("flags", *this, &I::convFlags, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::GameSave::WorldDataPointers> : SerializableObjectBase {
    using World = core::file::GameSave::WorldDataPointers;
    using Pos   = core::component::Position;

    SerializableField<1, World, std::string*> currentMap;
    SerializableField<2, World, std::string*> prevMap;
    SerializableField<3, World, Pos*> playerPos;
    SerializableField<4, World, Pos*> prevPlayerPos;

    SerializableObject()
    : currentMap("current", *this, &World::currentMap, SerializableFieldBase::Required{})
    , prevMap("previous", *this, &World::prevMap, SerializableFieldBase::Required{})
    , playerPos("position", *this, &World::playerPos, SerializableFieldBase::Required{})
    , prevPlayerPos("prevPos", *this, &World::prevPlayerPos, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::GameSave::PlayerDataPointers>
: public SerializableObjectBase {
    using Player = core::file::GameSave::PlayerDataPointers;

    SerializableField<1, Player, std::string*> name;
    SerializableField<2, Player, core::player::Gender*> gender;
    SerializableField<3, Player, core::player::Bag*> bag;
    SerializableField<4, Player, std::vector<core::pplmn::OwnedPeoplemon>*> peoplemon;
    SerializableField<5, Player, long*> money;
    SerializableField<6, Player, std::string*> whiteoutMap;
    SerializableField<7, Player, unsigned int*> whiteoutSpawn;
    SerializableField<8, Player, unsigned int*> repelSteps;
    SerializableField<9, Player,
                      std::array<std::vector<core::pplmn::StoredPeoplemon>,
                                 core::player::StorageSystem::BoxCount>*>
        storage;
    SerializableField<10, Player, std::unordered_set<std::string>*> visitedTowns;

    SerializableObject()
    : name("name", *this, &Player::playerName, SerializableFieldBase::Required{})
    , gender("gender", *this, &Player::sex, SerializableFieldBase::Required{})
    , bag("bag", *this, &Player::inventory, SerializableFieldBase::Required{})
    , peoplemon("peoplemon", *this, &Player::peoplemon, SerializableFieldBase::Required{})
    , money("money", *this, &Player::monei, SerializableFieldBase::Required{})
    , whiteoutMap("whiteoutMap", *this, &Player::whiteoutMap, SerializableFieldBase::Required{})
    , whiteoutSpawn("whiteoutSpawn", *this, &Player::whiteoutSpawn,
                    SerializableFieldBase::Required{})
    , repelSteps("repelSteps", *this, &Player::repelSteps, SerializableFieldBase::Required{})
    , storage("storage", *this, &Player::storage, SerializableFieldBase::Optional{})
    , visitedTowns("visitedTowns", *this, &Player::visitedTowns,
                   SerializableFieldBase::Optional{}) {}
};

template<>
struct SerializableObject<core::file::GameSave::ScriptDataPointers>
: public SerializableObjectBase {
    using S = core::file::GameSave::ScriptDataPointers;
    using M = std::unordered_map<std::string, bl::script::Value>;

    SerializableField<1, S, M*> entries;

    SerializableObject()
    : entries("saveEntries", *this, &S::entries, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::GameSave::ClockPointers> : public SerializableObjectBase {
    using C = core::file::GameSave::ClockPointers;
    using T = core::system::Clock::Time;

    SerializableField<1, C, T*> time;

    SerializableObject()
    : time("time", *this, &C::time, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::GameSave::TrainerPointers> : public SerializableObjectBase {
    using T = core::file::GameSave::TrainerPointers;
    using D = std::unordered_set<std::string>;

    SerializableField<1, T, D*> defeated;

    SerializableObject()
    : defeated("defeated", *this, &T::defeated, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::GameSave> : public SerializableObjectBase {
    using GS = core::file::GameSave;
    SerializableField<1, GS, unsigned long long> saveTime;
    SerializableField<2, GS, GS::PlayerDataPointers> player;
    SerializableField<3, GS, GS::InteractDataPointers> interaction;
    SerializableField<4, GS, GS::WorldDataPointers> world;
    SerializableField<5, GS, GS::ScriptDataPointers> script;
    SerializableField<6, GS, GS::ClockPointers> clock;

    SerializableObject()
    : saveTime("saveTime", *this, &GS::saveTime, SerializableFieldBase::Required{})
    , player("player", *this, &GS::player, SerializableFieldBase::Required{})
    , interaction("interaction", *this, &GS::interaction, SerializableFieldBase::Required{})
    , world("world", *this, &GS::world, SerializableFieldBase::Required{})
    , script("script", *this, &GS::scripts, SerializableFieldBase::Required{})
    , clock("clock", *this, &GS::clock, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
