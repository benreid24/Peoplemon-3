#ifndef CORE_EVENTS_GAMESAVE_HPP
#define CORE_EVENTS_GAMESAVE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when the game is saving. Allows arbitrary systems to add data to the save
 *
 * @ingroup Events
 *
 */
struct GameSaving {
    /// Top level game save data object
    mutable bl::serial::json::Group& saveData;
};

/**
 * @brief Fired when a game save is loaded. Allows arbitrary systems to pull from the save
 *
 * @ingroup Events
 *
 */
struct GameLoading {
    /// The top level data in the game save
    const bl::serial::json::Group& saveData;

    /// Modules can set this if they encounter a failure that should fail the entire load
    mutable std::string failMessage;
};

} // namespace event
} // namespace core

#endif
