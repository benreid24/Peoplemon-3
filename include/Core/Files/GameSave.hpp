#ifndef CORE_FILES_GAMESAVE_HPP
#define CORE_FILES_GAMESAVE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>

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

    /**
     * @brief Lists all saves in the save directory
     *
     * @param result Vector to populate with found saves
     */
    static void listSaves(std::vector<GameSave>& result);

    /**
     * @brief Saves the game. Fires an event::GameSaving event for systems to add their data to the
     *        save before it is written
     *
     * @param name The name of the player
     * @param bus Event bus to fire the GameSaving event on
     * @return True if the game could be saved, false on error
     */
    static bool saveGame(const std::string& name, bl::event::Dispatcher& bus);

    /**
     * @brief Loads the save represented by this object and fires an event::GameSaveLoaded event to
     *        allow systems to populate their data
     *
     * @param bus Event bus to fire the GameSaveLoaded event on
     * @return True on success, false on error
     */
    bool load(bl::event::Dispatcher& bus) const;

    /**
     * @brief Deletes the game save
     *
     * @return True on success, false on error
     *
     */
    bool remove() const;
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
struct SerializableObject<core::file::GameSave> : public SerializableObjectBase {
    SerializableField<core::file::GameSave, unsigned long long> saveTime;
    SerializableField<core::file::GameSave, std::string> saveName;

    SerializableObject()
    : saveTime("saveTime", *this, &core::file::GameSave::saveTime)
    , saveName("name", *this, &core::file::GameSave::saveName) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
