#ifndef CORE_EVENTS_GAMESAVE_HPP
#define CORE_EVENTS_GAMESAVE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>

namespace core
{
namespace file
{
struct GameSave;
}

namespace event
{
/**
 * @brief Fired when the game is saving or loading. Allows systems to hook in their data
 *
 * @ingroup Events
 *
 */
struct GameSaveInitializing {
    /// Game save being initialized
    file::GameSave& gameSave;

    /// True when saving, false when loading
    bool saving;

    /**
     * @brief Construct a new Game Save Initializing object
     *
     * @param sv The game save to init
     * @param s True when saving, false when loading
     */
    GameSaveInitializing(file::GameSave& sv, bool s)
    : gameSave(sv)
    , saving(s) {}
};

/**
 * @brief Fired when a game save is loaded. Fired after the load is complete
 *
 * @ingroup Events
 *
 */
struct GameSaveLoaded {
    /// Modules can set this if they encounter a failure that should fail the entire load
    mutable std::string failMessage;
};

} // namespace event
} // namespace core

#endif
