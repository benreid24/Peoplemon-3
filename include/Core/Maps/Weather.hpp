#ifndef CORE_MAPS_WEATHER_HPP
#define CORE_MAPS_WEATHER_HPP

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <memory>

namespace core
{
namespace map
{
namespace weather
{
class Base;
}

class Weather {
public:
    enum Type : std::uint8_t {
        None             = 0,
        AllRandom        = 1,
        LightRain        = 2,
        LightRainThunder = 3,
        HardRain         = 4,
        HardRainThunder  = 5,
        LightSnow        = 6,
        LightSnowThunder = 7,
        HardSnow         = 8,
        HardSnowThunder  = 9,
        ThinFog          = 10,
        ThickFog         = 11,
        Sunny            = 12,
        SandStorm        = 13,
        WaterRandom      = 14,
        SnowRandom       = 15,
        DesertRandom     = 16
    };

    Weather();

    ~Weather();

    void activate(const sf::FloatRect& area);

    void set(Type type);

    void update(float dt);

    void render(sf::RenderTarget& target, float residual) const;

private:
    enum State { Continuous, WaitingWeather, Stopping, WaitingStopped };

    Type type;
    std::unique_ptr<weather::Base> weather;
    State state;
    float stateTime;
    mutable sf::FloatRect area;

    void makeWeather();
};

} // namespace map
} // namespace core

#endif
