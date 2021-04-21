#ifndef CORE_MAPS_WEATHER_THUNDER_HPP
#define CORE_MAPS_WEATHER_THUNDER_HPP

#include <BLIB/Media/Audio.hpp>
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
     * @param minInterval Minimum time in seconds between lightning
     * @param maxInterval Maximum time in seconds between lightning
     */
    Thunder(bool enabled, float minInterval, float maxInterval);

    /**
     * @brief Stops the thunder sound
     *
     */
    ~Thunder();

    /**
     * @brief Update the thunder
     *
     * @param dt Time elapsed in seconds since last call to update()
     */
    void update(float dt);

    /**
     * @brief Render the lightning if present
     *
     * @param target The target to render to
     * @param residual Time not accounted for in update()
     */
    void render(sf::RenderTarget& target, float residual) const;

private:
    const bool enabled;
    const float minInterval;
    const float maxInterval;
    float timeSinceLastThunder;
    mutable sf::RectangleShape lightning;
    bl::resource::Resource<sf::SoundBuffer>::Ref sound;
    bl::audio::AudioSystem::Handle soundHandle;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
