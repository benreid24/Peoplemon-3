#ifndef CORE_EVENTS_WEATHER_HPP
#define CORE_EVENTS_WEATHER_HPP

#include <Core/Maps/Weather.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when weather starts
 *
 * @ingroup Events
 */
struct WeatherStarted {
    /**
     * @brief Creates a weather started event
     *
     * @param type The type of weather that started
     */
    WeatherStarted(map::Weather::Type type)
    : type(type) {}

    /// The type of weather that started
    const map::Weather::Type type;
};

/**
 * @brief Fired when weather stops
 *
 * @ingroup Events
 */
struct WeatherStopped {
    /**
     * @brief Creates a weather stopped event
     *
     * @param type The type of weather that stopped
     */
    WeatherStopped(map::Weather::Type type)
    : type(type) {}

    /// The type of weather that stopped
    const map::Weather::Type type;
};

/**
 * @brief Fired when thunder happens
 *
 * @ingroup Events
 */
struct Thundered {};

} // namespace event
} // namespace core

#endif
