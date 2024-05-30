#ifndef CORE_MAPS_WEATHER_THUNDER_HPP
#define CORE_MAPS_WEATHER_THUNDER_HPP

#include <BLIB/Audio.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace map
{
namespace weather
{
/**
 * @brief Simple helper class for creating thunder in different weather types
 *
 * @ingroup Weather
 *
 */
class Thunder {
public:
    /**
     * @brief Construct a new Thunder object
     *
     * @param enabled True to make thunder, false to do nothing
     * @param frequent True for frequent thunder, false for less frequent
     */
    Thunder(bool enabled, bool frequent);

    /**
     * @brief Stops the thunder sound
     *
     */
    ~Thunder();

    /**
     * @brief Prevents further thunder and fades out current strike if present
     *
     */
    void stop();

    /**
     * @brief Update the thunder
     *
     * @param dt Time elapsed in seconds since last call to update()
     */
    void update(float dt);

private:
    const bool enabled;
    const float minInterval;
    const float maxInterval;
    float timeSinceLastThunder;
    bl::audio::AudioSystem::Handle sound;
    bool stopping;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
