#include <Core/Maps/Weather.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Events/Weather.hpp>
#include <Core/Systems/Systems.hpp>

#include "Weather/Base.hpp"
#include "Weather/Fog.hpp"
#include "Weather/Rain.hpp"
#include "Weather/Sandstorm.hpp"
#include "Weather/Snow.hpp"
#include "Weather/Sunny.hpp"

namespace core
{
namespace map
{
namespace
{
const std::string Types[] = {"None",
                             "AllRandom",
                             "LightRain",
                             "LightRainThunder",
                             "HardRain",
                             "HardRainThunder",
                             "LightSnow",
                             "LightSnowThunder",
                             "HardSnow",
                             "HardSnowThunder",
                             "ThinFog",
                             "ThickFog",
                             "Sunny",
                             "Sandstorm",
                             "WaterRandom",
                             "SnowRandom",
                             "DesertRandom"};

} // namespace

Weather::Weather()
: type(None)
, state(Continuous)
, stateTime(-1.f)
, engine(nullptr) {}

Weather::~Weather() { weather.reset(); }

void Weather::activate(bl::engine::Engine& e, Map& m) {
    engine = &e;
    owner  = &m;
}

void Weather::set(Type t, bool im) {
    if (t != type) {
        BL_LOG_INFO << "Set weather to " << Types[t];
        type  = t;
        state = Stopping;
        if (weather) {
            weather->stop();
            if (im) weather.release();
        }
    }
}

Weather::Type Weather::getType() const { return type; }

void Weather::update(float dt) {
    if (weather) weather->update(dt);

    switch (state) {
    case WaitingWeather:
        stateTime -= dt;
        if (stateTime <= 0.f) {
            BL_LOG_INFO << "Stopping current weather";
            stateTime = -1.f;
            weather->stop();
            state = Stopping;
        }
        break;

    case WaitingStopped:
        stateTime -= dt;
        if (stateTime <= 0.f) {
            BL_LOG_INFO << "Starting new weather";
            stateTime = bl::util::Random::get<float>(300.f, 600.f);
            state     = WaitingWeather;
            makeWeather();
        }
        break;

    case Stopping:
        if (!weather || weather->stopped()) {
            if (weather) {
                BL_LOG_INFO << "Weather stopped";
                bl::event::Dispatcher::dispatch<event::WeatherStopped>({weather->type()});
                weather.release();
            }

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

void Weather::makeWeather() {
    const auto makeRain = [this](bool hard, bool thunder) {
        BL_LOG_INFO << "Created rain";
        this->weather.reset(new weather::Rain(hard, thunder));
    };
    const auto makeSnow = [this](bool hard, bool thunder) {
        BL_LOG_INFO << "Created snow";
        this->weather.reset(new weather::Snow(hard, thunder));
    };
    const auto makeFog = [this](bool thick) {
        BL_LOG_INFO << "Created fog";
        this->weather.reset(new weather::Fog(thick));
    };
    const auto makeSunny = [this]() {
        BL_LOG_INFO << "Created sunny";
        this->weather.reset(new weather::Sunny());
    };
    const auto makeSandstorm = [this]() {
        BL_LOG_INFO << "Created sandstorm";
        this->weather.reset(new weather::Sandstorm());
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

    if (weather) {
        weather->start(*engine, *owner);
        bl::event::Dispatcher::dispatch<event::WeatherStarted>({weather->type()});
    }
}

} // namespace map
} // namespace core
