#include <Core/Maps/LightingSystem.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Media/Shapes/GradientCircle.hpp>
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
constexpr std::size_t GameBufferExtra   = 4;
constexpr std::size_t EditorBufferExtra = 64;
} // namespace

LightingSystem::LightingSystem()
: minLevel(175)
, maxLevel(255)
, sunlight(1)
, activeLights(Storage::GrowthPolicy::ExpandBuffer, 0) // no alocation
, lightGrid({}, 1.f, 1.f)                              // no allocation
, nextHandle(1)
, sunlightFactor(1.f)
, weatherModifier(0)
, targetWeatherModifier(0)
, weatherResidual(0.f) {}

LightingSystem::Handle LightingSystem::addLight(const Light& light, bool p) {
    const Handle h         = nextHandle++;
    const bool needRecache = activeLights.size() == activeLights.capacity();
    auto it                = activeLights.emplace(light, h);
    if (needRecache) { rebuildCache(); }
    else {
        lightGrid.add(sf::Vector2f(light.position), &(*it));
        handles.emplace(h, it);
    }

    if (p) { rawLights.push_back(light); }
    return h;
}

void LightingSystem::updateLight(Handle handle, const Light& light, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;
    lightGrid.move(
        sf::Vector2f(it->second->light.position), sf::Vector2f(light.position), &(*it->second));
    it->second->light = light;

    if (p) {
        for (auto& l : rawLights) {
            if (l.position == light.position) {
                l = light;
                break;
            }
        }
    }
}

LightingSystem::Handle LightingSystem::getClosestLight(const sf::Vector2i& position) {
    Handle closest      = None;
    unsigned long cdist = 10000000;

    const auto visitor = [&closest, &cdist, &position](ActiveLight* light) {
        const unsigned long dist = bl::math::magnitudeSquared(light->light.position - position);
        const unsigned int r     = light->light.radius;
        if (dist < cdist && dist < r * r) {
            cdist   = dist;
            closest = light->handle;
        }
    };

    lightGrid.forAllInCellAndNeighbors(sf::Vector2f(position), visitor);
    return closest;
}

const Light& LightingSystem::getLight(Handle h) const {
    static const Light empty(0, {-1, -1});
    const auto it = handles.find(h);
    return it != handles.end() ? it->second->light : empty;
}

void LightingSystem::removeLight(Handle handle, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;

    if (p) {
        for (auto lit = rawLights.begin(); lit != rawLights.end(); ++lit) {
            if (lit->position == it->second->light.position) {
                rawLights.erase(lit);
                break;
            }
        }
    }

    lightGrid.remove(sf::Vector2f(it->second->light.position), &(*it->second));
    handles.erase(it);
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

void LightingSystem::legacyResize(const sf::Vector2i& mapSize) {
    handles.clear();
    lightGrid.setSize({0.f,
                       0.f,
                       static_cast<float>(mapSize.x * Properties::PixelsPerTile()),
                       static_cast<float>(mapSize.y * Properties::PixelsPerTile())},
                      Properties::WindowWidth(),
                      Properties::WindowHeight());
}

void LightingSystem::activate(const sf::Vector2i& mapSize) {
    legacyResize(mapSize);

    activeLights.reserve(rawLights.size() +
                         (Properties::InEditor() ? EditorBufferExtra : GameBufferExtra));
    for (const auto& light : rawLights) { addLight(light, false); }

    renderSurface.create(Properties::LightingWidthTiles() * Properties::PixelsPerTile(),
                         Properties::LightingHeightTiles() * Properties::PixelsPerTile());
    sprite.setTexture(renderSurface.getTexture(), true);
    sprite.setScale(1.f, -1.f);
    levelRange = maxLevel - minLevel;
}

void LightingSystem::subscribe() { bl::event::Dispatcher::subscribe(this); }

void LightingSystem::unsubscribe() { bl::event::Dispatcher::unsubscribe(this); }

void LightingSystem::clear() {
    rawLights.clear();
    maxLevel   = 255;
    minLevel   = 75;
    levelRange = maxLevel - minLevel;
    sunlight   = 1;
    activeLights.clear();
    handles.clear();
    lightGrid.clear();
}

void LightingSystem::update(float dt) {
    if (weatherModifier != targetWeatherModifier) {
        weatherResidual += AdjustSpeed * dt;
        const float p = std::floor(weatherResidual);
        if (p > 0.f) {
            weatherResidual -= p;
            if (targetWeatherModifier > weatherModifier) {
                weatherModifier += static_cast<int>(p);
                if (weatherModifier > targetWeatherModifier) {
                    weatherModifier = targetWeatherModifier;
                }
            }
            else {
                weatherModifier -= static_cast<int>(p);
                if (weatherModifier < targetWeatherModifier) {
                    weatherModifier = targetWeatherModifier;
                }
            }
        }
    }
    else { weatherResidual = 0.f; }
}

void LightingSystem::render(sf::RenderTarget& target) {
    const sf::Vector2f corner(target.getView().getCenter() - target.getView().getSize() * 0.5f);
    const sf::Vector2f& size = target.getView().getSize();

    const std::uint8_t ambient = computeAmbient();

    sf::View view = target.getView();
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    renderSurface.setView(view);
    renderSurface.clear(sf::Color(0, 0, 0, ambient));

    if (ambient > 80) {
        const float extraSpace =
            static_cast<float>(Properties::ExtraRenderTiles() * Properties::PixelsPerTile());
        const sf::BlendMode blendMode(
            sf::BlendMode::One, sf::BlendMode::One, sf::BlendMode::ReverseSubtract);
        bl::shapes::GradientCircle circle(100);
        circle.setCenterColor(sf::Color(0, 0, 0, ambient));
        circle.setOuterColor(sf::Color(0, 0, 0, 0));

        const auto visitor = [this, &circle, &blendMode](ActiveLight* light) {
            circle.setPosition(static_cast<sf::Vector2f>(light->light.position));
            circle.setRadius(light->light.radius);
            renderSurface.draw(circle, blendMode);
        };

        lightGrid.forAllInRegion({corner.x - extraSpace,
                                  corner.y - extraSpace,
                                  size.x + extraSpace * 2.f,
                                  size.y + extraSpace * 2.f},
                                 visitor);
    }

    sprite.setPosition(corner.x, corner.y + size.y);
    sprite.setScale(size.x / static_cast<float>(renderSurface.getSize().x),
                    -size.y / static_cast<float>(renderSurface.getSize().y));
    target.draw(sprite);
}

std::uint8_t LightingSystem::computeAmbient() const {
    const float preambient = 255.f - (static_cast<float>(minLevel) + levelRange * sunlightFactor);
    return std::min(std::max(0, static_cast<int>(preambient) + weatherModifier), 255);
}

void LightingSystem::observe(const event::TimeChange& now) {
    if (adjustsForSunlight()) {
        const float x  = now.newTime.hour * 60 + now.newTime.minute;
        const float n  = 0.7;
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

void LightingSystem::rebuildCache() {
    BL_LOG_WARN << "Light allocation was too small and reallocation had to be performed";

    handles.clear();
    lightGrid.clear();
    for (auto it = activeLights.begin(); it != activeLights.end(); ++it) {
        handles.emplace(it->handle, it);
        lightGrid.add(sf::Vector2f(it->light.position), &(*it));
    }
}

} // namespace map
} // namespace core
