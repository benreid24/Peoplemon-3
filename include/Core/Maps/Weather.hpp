#ifndef CORE_MAPS_WEATHER_HPP
#define CORE_MAPS_WEATHER_HPP

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <memory>

namespace core
{
namespace system
{
class Systems;
}

namespace map
{
namespace weather
{
class Base;
}

/**
 * @brief Parent weather system for maps. Manages active weather
 *
 * @ingroup Weather
 * @ingroup Maps
 *
 */
class Weather {
public:
    /**
     * @brief The type of weather
     *
     */
    enum Type : std::uint8_t {
        /// No weather will occur
        None = 0,

        /// All types of weather may occur over time
        AllRandom = 1,

        /// Light rain with no thunder
        LightRain = 2,

        /// Light rain with thunder
        LightRainThunder = 3,

        /// Hard rain with no thunder
        HardRain = 4,

        /// Hard rain with thunder
        HardRainThunder = 5,

        /// Light snow with no thunder
        LightSnow = 6,

        /// Light snow with thunder
        LightSnowThunder = 7,

        /// Hard snow with no thunder
        HardSnow = 8,

        /// Hard snow with thunder
        HardSnowThunder = 9,

        /// Thin fog covers the area
        ThinFog = 10,

        /// Thick fog obscures everything
        ThickFog = 11,

        /// A very sunny day with puslating light
        Sunny = 12,

        /// A sandstorm ravages you
        SandStorm = 13,

        /// Periodically triggers one of LightRain, HardRain, LightRainThunder, HardRainThunder
        WaterRandom = 14,

        /// Periodically triggers one of LightSnow, HardSnow, LightSnowThunder, HardSnowThunder
        SnowRandom = 15,

        /// Periodically triggers one of Sunny, Sandstorm
        DesertRandom = 16
    };

    /**
     * @brief Initializes the weather system with None
     *
     */
    Weather();

    /**
     * @brief Terminates active weather
     *
     */
    ~Weather();

    /**
     * @brief Activates the weather system
     *
     * @param area Initial area to spawn particles in
     */
    void activate(const sf::FloatRect& area);

    /**
     * @brief Sets the current weather type
     *
     * @param type The type of weather to transition to
     */
    void set(Type type);

    /**
     * @brief Updates the current weather
     *
     * @param systems The primary game systems
     * @param dt Time elapsed since last call to update() in seconds
     */
    void update(system::Systems& systems, float dt);

    /**
     * @brief Renders the current weather
     *
     * @param target The target to render to
     * @param residual Residual time not yet accounted for in update
     */
    void render(sf::RenderTarget& target, float residual) const;

private:
    enum State { Continuous, WaitingWeather, Stopping, WaitingStopped };

    Type type;
    std::unique_ptr<weather::Base> weather;
    State state;
    float stateTime;
    mutable sf::FloatRect area;

    void makeWeather(system::Systems& systems);
};

} // namespace map
} // namespace core

#endif
