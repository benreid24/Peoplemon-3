#ifndef CORE_MAPS_WEATHER_BASE_HPP
#define CORE_MAPS_WEATHER_BASE_HPP

#include <BLIB/Engine/Engine.hpp>
#include <Core/Maps/Weather.hpp>
#include <SFML/Graphics.hpp>

/**
 * @addtogroup Weather
 * @ingroup Maps
 * @brief Collection of weather type implementations
 */

namespace core
{
namespace map
{
class Map;

/// Collection of different weather types
namespace weather
{
/**
 * @brief Base class for all weather types
 *
 * @ingroup Weather
 */
struct Base {
    /**
     * @brief Destroy the Base object
     */
    virtual ~Base() = default;

    /**
     * @brief Returns the type of weather this is
     *
     * @return Weather::Type The type of weather this is
     */
    virtual Weather::Type type() const = 0;

    /**
     * @brief Update the weather
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Start the weather using the initial area
     *
     * @param engine The game engine instance
     * @param renderTarget The render target the weather will be rendered to
     * @param map The map the weather is in
     */
    virtual void start(bl::engine::Engine& engine, bl::rc::RenderTarget& renderTarget,
                       Map& map) = 0;

    /**
     * @brief Stop the weather
     */
    virtual void stop() = 0;

    /**
     * @brief Returns true when the weather has completed stopping
     *
     * @return True if no particles remain, false if not finished stopping
     */
    virtual bool stopped() const = 0;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
