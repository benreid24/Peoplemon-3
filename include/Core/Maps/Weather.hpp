#ifndef CORE_MAPS_WEATHER_HPP
#define CORE_MAPS_WEATHER_HPP

#include <BLIB/Engine/Engine.hpp>
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
class Map;

namespace weather
{
struct Base;
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

        /// A very sunny day with pulsating light
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
     * @param engine The game engine instance
     * @param map The map that the weather is in
     */
    void activate(bl::engine::Engine& engine, Map& map);

    /**
     * @brief Sets the current weather type
     *
     * @param type The type of weather to transition to
     * @param immediate True to skip the stop phase of the current weather
     */
    void set(Type type, bool immediate = false);

    /**
     * @brief Returns the current type of weather
     *
     */
    Type getType() const;

    /**
     * @brief Updates the current weather
     *
     * @param dt Time elapsed since last call to update() in seconds
     */
    void update(float dt);

private:
    enum State { Continuous, WaitingWeather, Stopping, WaitingStopped };

    Type type;
    std::unique_ptr<weather::Base> weather;
    State state;
    float stateTime;
    bl::engine::Engine* engine;
    Map* owner;

    void makeWeather();
};

} // namespace map
} // namespace core

#endif
