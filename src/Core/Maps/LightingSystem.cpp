#include <Core/Maps/LightingSystem.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Render/Primitives/Color.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <cmath>
#include <limits>

namespace core
{
namespace map
{
namespace
{
constexpr float AdjustSpeed             = 30.f;
constexpr std::size_t GameBufferExtra   = 0;
constexpr std::size_t EditorBufferExtra = 64;
} // namespace

LightingSystem::LightingSystem()
: minLevel(175)
, maxLevel(255)
, sunlight(1)
, lightsActive(false)
, sunlightFactor(1.f)
, weatherModifier(0)
, targetWeatherModifier(0)
, weatherResidual(0.f) {}

void LightingSystem::addLight(const Light& light) {
    rawLights.push_back(light);
    addLightToScene(light);
}

void LightingSystem::addLightToScene(const Light& light) {
    if (sceneLighting) {
        activeLights.emplace_back(sceneLighting->addLight(
            glm::vec2(static_cast<float>(light.position.x), static_cast<float>(light.position.y)),
            static_cast<float>(light.radius),
            glm::vec3(static_cast<float>(light.color.x) / 255.f,
                      static_cast<float>(light.color.y) / 255.f,
                      static_cast<float>(light.color.z) / 255.f)));
    }
}

void LightingSystem::updateLight(Handle handle, const Light& light) {
    rawLights[handle] = light;

    if (sceneLighting) {
        bl::rc::lgt::Light2D& l = activeLights[handle];
        l.setPosition(
            glm::vec2(static_cast<float>(light.position.x), static_cast<float>(light.position.y)));
        l.setRadius(static_cast<float>(light.radius));
        l.setColor(glm::vec3(static_cast<float>(light.color.x) / 255.f,
                             static_cast<float>(light.color.y) / 255.f,
                             static_cast<float>(light.color.z) / 255.f));
    }
}

LightingSystem::Handle LightingSystem::getClosestLight(const sf::Vector2i& position) {
    Handle closest      = None;
    unsigned long cdist = 10000000;

    for (unsigned int i = 0; i < rawLights.size(); ++i) {
        const unsigned long dist = bl::math::magnitudeSquared(rawLights[i].position - position);
        const unsigned int r     = rawLights[i].radius;
        if (dist < cdist && dist < r * r) {
            cdist   = dist;
            closest = i;
        }
    }

    return closest;
}

const Light& LightingSystem::getLight(Handle h) const { return rawLights[h]; }

void LightingSystem::removeLight(Handle handle) {
    rawLights.erase(rawLights.begin() + handle);

    if (sceneLighting) {
        activeLights[handle].removeFromScene();
        activeLights.erase(activeLights.begin() + handle);
    }
}

bool LightingSystem::lightsAreOn() const { return computeAmbient() > 80; }

void LightingSystem::setAmbientLevel(std::uint8_t lowLightLevel, std::uint8_t highLightLevel) {
    minLevel   = std::min(lowLightLevel, highLightLevel);
    maxLevel   = std::max(lowLightLevel, highLightLevel);
    levelRange = maxLevel - minLevel;
}

std::uint8_t LightingSystem::getMinLightLevel() const { return minLevel; }

std::uint8_t LightingSystem::getMaxLightLevel() const { return maxLevel; }

void LightingSystem::adjustForSunlight(bool a) { sunlight = a ? 1 : 0; }

bool LightingSystem::adjustsForSunlight() const { return sunlight != 0; }

void LightingSystem::activate(bl::rc::lgt::Scene2DLighting& lighting) {
    sceneLighting = &lighting;
    levelRange    = maxLevel - minLevel;

    activeLights.reserve(rawLights.size() +
                         (Properties::InEditor() ? EditorBufferExtra : GameBufferExtra));
    for (const auto& light : rawLights) { addLightToScene(light); }
    for (auto& handle : activeLights) { handle.setColor(glm::vec3(0.f)); }
    lightsActive = false;

    updateAmbientLighting();
}

void LightingSystem::subscribe() { bl::event::Dispatcher::subscribe(this); }

void LightingSystem::unsubscribe() { bl::event::Dispatcher::unsubscribe(this); }

void LightingSystem::clear() {
    rawLights.clear();
    maxLevel   = 255;
    minLevel   = 75;
    levelRange = maxLevel - minLevel;
    sunlight   = 1;
    for (auto& light : activeLights) { light.removeFromScene(); }
    activeLights.clear();
}

void LightingSystem::update(float dt) {
    if (weatherModifier != targetWeatherModifier) {
        weatherResidual += AdjustSpeed * dt;
        const float p = std::floor(weatherResidual);
        if (p >= 1.f) {
            weatherResidual -= p;
            if (targetWeatherModifier > weatherModifier) {
                weatherModifier += static_cast<int>(p);
                if (weatherModifier > targetWeatherModifier) {
                    weatherModifier = targetWeatherModifier;
                    weatherResidual = 0.f;
                }
            }
            else {
                weatherModifier -= static_cast<int>(p);
                if (weatherModifier < targetWeatherModifier) {
                    weatherModifier = targetWeatherModifier;
                    weatherResidual = 0.f;
                }
            }
        }
    }

    if (lightsAreOn() && !lightsActive) {
        lightsActive = true;
        Light* light = &rawLights.front();

        for (auto& handle : activeLights) {
            handle.setColor(glm::vec3(static_cast<float>(light->color.x) / 255.f,
                                      static_cast<float>(light->color.y) / 255.f,
                                      static_cast<float>(light->color.z) / 255.f));
            light += 1;
        }
    }
    else if (!lightsAreOn() && lightsActive) {
        lightsActive = false;
        for (auto& handle : activeLights) { handle.setColor(glm::vec3(0.f)); }
    }

    updateAmbientLighting();
}

void LightingSystem::updateAmbientLighting() {
    if (sceneLighting) {
        const std::uint8_t ambient = computeAmbient();
        const float a              = static_cast<float>(255 - ambient) / 255.f;
        sceneLighting->setAmbientLight(glm::vec3(a));
    }
}

std::uint8_t LightingSystem::computeAmbient() const {
    const float preambient = 255.f - (static_cast<float>(minLevel) + levelRange * sunlightFactor);
    return std::min(std::max(0, static_cast<int>(preambient) + weatherModifier), 255);
}

void LightingSystem::observe(const event::TimeChange& now) {
    if (adjustsForSunlight()) {
        const float x  = now.newTime.hour * 60 + now.newTime.minute;
        const float n  = 0.7f;
        sunlightFactor = 1.f - (0.5 * std::cos(3.1415926 * x / 720) + 0.5) *
                                   ((1 - n) * (720 - x) * (720 - x) / 518400 + n);
    }
    else { sunlightFactor = 1.f; }
}

void LightingSystem::observe(const event::WeatherStarted& event) {
    switch (event.type) {
    case Weather::LightRain:
    case Weather::LightRainThunder:
        targetWeatherModifier = Properties::LightRainLightModifier();
        break;

    case Weather::HardRain:
    case Weather::HardRainThunder:
        targetWeatherModifier = Properties::HardRainLightModifier();
        break;

    case Weather::LightSnow:
    case Weather::LightSnowThunder:
        targetWeatherModifier = Properties::LightSnowLightModifier();
        break;

    case Weather::HardSnow:
    case Weather::HardSnowThunder:
        targetWeatherModifier = Properties::HardSnowLightModifier();
        break;

    case Weather::ThinFog:
        targetWeatherModifier = Properties::ThinFogLightModifier();
        break;

    case Weather::ThickFog:
        targetWeatherModifier = Properties::ThickFogLightModifier();
        break;

    case Weather::SandStorm:
        targetWeatherModifier = Properties::SandstormLightModifier();
        break;

    case Weather::Sunny:
        targetWeatherModifier = Properties::SunnyLightModifier();
        break;

    default:
        targetWeatherModifier = 0;
        break;
    }
}

void LightingSystem::observe(const event::WeatherStopped&) { targetWeatherModifier = 0; }

} // namespace map
} // namespace core
