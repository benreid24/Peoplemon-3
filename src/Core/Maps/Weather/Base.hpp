#ifndef CORE_MAPS_WEATHER_BASE_HPP
#define CORE_MAPS_WEATHER_BASE_HPP

#include <SFML/Graphics.hpp>

/**
 * @addtogroup Weather
 * @ingroup Maps
 * @brief Collection of weather type implementations
 *
 */

namespace core
{
namespace map
{
/// Collection of different weather types
namespace weather
{
/**
 * @brief Base class for all weather types
 *
 * @ingroup Weather
 *
 */
struct Base {
    /**
     * @brief Destroy the Base object
     *
     */
    virtual ~Base() = default;

    /**
     * @brief Update the weather
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Render the weather to the target. Take note of the target render area for spawning new
     *        particles
     *
     * @param target The target to render to
     * @param residual Residual time between calls to update()
     */
    virtual void render(sf::RenderTarget& target, float residual) const = 0;

    /**
     * @brief Start the weather using the initial area
     *
     * @param initialView The area the camera can currently see
     */
    virtual void start(const sf::FloatRect& initialView) = 0;

    /**
     * @brief Stop the weather
     *
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
