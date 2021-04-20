#include <Core/Maps/Weather.hpp>

#include <BLIB/Util/Random.hpp>

#include "Weather/Base.hpp"
#include "Weather/Rain.hpp"

namespace core
{
namespace map
{
namespace
{
bool isRandom(Weather::Type t) {
    switch (t) {
    case Weather::AllRandom:
    case Weather::WaterRandom:
    case Weather::SnowRandom:
    case Weather::DesertRandom:
        return true;
    default:
        return false;
    }
}
} // namespace

Weather::Weather()
: type(None)
, state(Continuous)
, stateTime(-1.f) {}

Weather::~Weather() { weather.reset(); }

void Weather::set(Type t) {
    if (t != type) {
        type  = t;
        state = Stopping;
        if (weather) weather->stop();
    }
}

void Weather::update(float dt) {
    if (weather) weather->update(dt);

    switch (state) {
    case WaitingWeather:
        stateTime -= dt;
        if (stateTime <= 0.f) {
            stateTime = -1.f;
            weather->stop();
            state = Stopping;
        }
        break;

    case WaitingStopped:
        stateTime -= dt;
        if (stateTime <= 0.f) {
            stateTime = bl::util::Random::get<float>(300.f, 600.f);
            state     = WaitingWeather;
            makeWeather();
        }
        break;

    case Stopping:
        if (!weather || weather->stopped()) {
            weather.reset();
            switch (type) {
            case AllRandom:
            case WaterRandom:
            case SnowRandom:
            case DesertRandom:
                state     = WaitingStopped;
                stateTime = bl::util::Random::get<float>(200.f, 400.f);
                break;
            case None:
                state = Continuous;
                break;
            default:
                state = Continuous;
                makeWeather();
                break;
            }
        }
        break;

    default:
        break;
    }
}

void Weather::render(sf::RenderTarget& target, float lag) const {
    if (weather) weather->render(target, lag);
}

void Weather::makeWeather() {
    const auto makeRain = [this](bool hard, bool thunder) {
        this->weather.reset(new weather::Rain(hard, thunder));
    };
    const auto makeSnow = [this](bool hard, bool thunder) {
        // TODO - snow
    };
    const auto makeFog = [this](bool thick) {
        // TODO - fog
    };
    const auto makeSunny = [this]() {
        // TODO - sunny
    };
    const auto makeSandstorm = [this]() {
        // TODO - sandstorm
    };

    using Maker          = std::function<void()>;
    const Maker makers[] = {
        [&makeFog]() { makeFog(true); }, // all
        [&makeFog]() { makeFog(false); },
        [&makeRain]() { makeRain(false, false); }, // rain
        [&makeRain]() { makeRain(false, true); },
        [&makeRain]() { makeRain(true, false); },
        [&makeRain]() { makeRain(true, true); },
        [&makeSnow]() { makeSnow(false, false); }, // snow
        [&makeSnow]() { makeSnow(false, true); },
        [&makeSnow]() { makeSnow(true, false); },
        [&makeSnow]() { makeSnow(true, true); },
        makeSunny, // desert
        makeSandstorm,
    };

    switch (type) {
    case AllRandom:
        makers[bl::util::Random::get<unsigned int>(0, 12)](); // [0, 11]
        break;
    case WaterRandom:
        makers[bl::util::Random::get<unsigned int>(2, 6)](); //[2, 5]
        break;
    case SnowRandom:
        makers[bl::util::Random::get<unsigned int>(6, 10)](); //[6, 9]
        break;
    case DesertRandom:
        makers[bl::util::Random::get<unsigned int>(10, 12)](); // [10, 11]
        break;
    case LightRain:
        makeRain(false, false);
        break;
    case LightRainThunder:
        makeRain(false, true);
        break;
    case HardRain:
        makeRain(true, false);
        break;
    case HardRainThunder:
        makeRain(true, true);
        break;
    case LightSnow:
        makeSnow(false, false);
        break;
    case LightSnowThunder:
        makeSnow(false, true);
        break;
    case HardSnow:
        makeSnow(true, false);
        break;
    case HardSnowThunder:
        makeSnow(true, true);
        break;
    case ThinFog:
        makeFog(false);
        break;
    case ThickFog:
        makeFog(true);
        break;
    case Sunny:
        makeSunny();
        break;
    case SandStorm:
        makeSandstorm();
        break;
    default:
        BL_LOG_WARN << "Unknown weather type: " << type;
        break;
    }
}

} // namespace map
} // namespace core
