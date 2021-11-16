#include <Core/Maps/LightingSystem.hpp>

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
constexpr float AdjustSpeed = 30.f;
}

LightingSystem::LightingSystem()
: lightsField(*this)
, lowLevelField(*this, 175)
, highLevelField(*this, 255)
, sunlightField(*this, true)
, eventGuard(this)
, lights(320, 320, 800, 600)
, minLevel(lowLevelField.getValue())
, maxLevel(highLevelField.getValue())
, sunlightFactor(1.f)
, weatherModifier(0)
, targetWeatherModifier(0)
, weatherResidual(0.f) {}

LightingSystem::Handle LightingSystem::addLight(const Light& light, bool p) {
    const Handle h =
        bl::util::Random::get<Handle>(static_cast<Handle>(1), std::numeric_limits<Handle>::max());
    auto ptr = lights.add(
        light.position.getValue().x, light.position.getValue().y, std::make_pair(h, light));
    handles.emplace(h, ptr);

    if (p) { lightsField.getValue().push_back(light); }
    return h;
}

void LightingSystem::updateLight(Handle handle, const Light& light, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;
    it->second->move(light.position.getValue().x, light.position.getValue().y);
    it->second->get().second = light;

    if (p) {
        for (auto& l : lightsField.getValue()) {
            if (l.position.getValue() == it->second->get().second.position.getValue()) {
                l = light;
                break;
            }
        }
    }
}

LightingSystem::Handle LightingSystem::getClosestLight(const sf::Vector2i& position) {
    auto set            = lights.getCellAndNeighbors(position.x, position.y);
    Handle closest      = None;
    unsigned long cdist = 10000000;
    for (const auto& light : set) {
        const long dx            = light.get().second.position.getValue().x - position.x;
        const long dy            = light.get().second.position.getValue().y - position.y;
        const unsigned int r     = light.get().second.radius.getValue();
        const unsigned long dist = dx * dx + dy * dy;
        if (dist < cdist && dist < r * r) {
            cdist   = dist;
            closest = light.get().first;
        }
    }
    return closest;
}

const Light& LightingSystem::getLight(Handle h) const {
    static const Light empty(0, {-1, -1});
    const auto it = handles.find(h);
    return it != handles.end() ? it->second->get().second : empty;
}

void LightingSystem::removeLight(Handle handle, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;

    if (p) {
        for (unsigned int i = 0; i < lightsField.getValue().size(); ++i) {
            if (lightsField.getValue()[i].position.getValue() ==
                it->second->get().second.position.getValue()) {
                lightsField.getValue().erase(lightsField.getValue().begin() + i);
                break;
            }
        }
    }

    it->second->remove();
    handles.erase(it);
}

void LightingSystem::setAmbientLevel(std::uint8_t lowLightLevel, std::uint8_t highLightLevel) {
    minLevel   = std::min(lowLightLevel, highLightLevel);
    maxLevel   = std::max(lowLightLevel, highLightLevel);
    levelRange = maxLevel - minLevel;
}

std::uint8_t LightingSystem::getMinLightLevel() const { return lowLevelField.getValue(); }

std::uint8_t LightingSystem::getMaxLightLevel() const { return highLevelField.getValue(); }

void LightingSystem::adjustForSunlight(bool a) { sunlightField = a; }

bool LightingSystem::adjustsForSunlight() const { return sunlightField.getValue(); }

void LightingSystem::legacyResize(const sf::Vector2i& mapSize) {
    handles.clear();
    lights.setSize(mapSize.x * Properties::PixelsPerTile(),
                   mapSize.y * Properties::PixelsPerTile(),
                   Properties::WindowWidth(),
                   Properties::WindowHeight());
}

void LightingSystem::activate(const sf::Vector2i& mapSize) {
    legacyResize(mapSize);

    for (const auto& light : lightsField.getValue()) { addLight(light, false); }

    renderSurface.create(Properties::LightingWidthTiles() * Properties::PixelsPerTile(),
                         Properties::LightingHeightTiles() * Properties::PixelsPerTile());
    sprite.setTexture(renderSurface.getTexture(), true);
    sprite.setScale(1.f, -1.f);
}

void LightingSystem::subscribe(bl::event::Dispatcher& bus) { eventGuard.subscribe(bus); }

void LightingSystem::unsubscribe() { eventGuard.unsubscribe(); }

void LightingSystem::clear() {
    lightsField.getValue().clear();
    highLevelField = 255;
    lowLevelField  = 75;
    levelRange     = maxLevel - minLevel;
    sunlightField  = true;
    lights.clear();
    handles.clear();
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
    else {
        weatherResidual = 0.f;
    }
}

void LightingSystem::render(sf::RenderTarget& target) {
    const sf::Vector2f corner(target.getView().getCenter() - target.getView().getSize() * 0.5f);
    const sf::Vector2f& size = target.getView().getSize();

    const float preambient = 255.f - (static_cast<float>(minLevel) + levelRange * sunlightFactor);
    const std::uint8_t ambient =
        std::min(std::max(0, static_cast<int>(preambient) + weatherModifier), 255);
    auto lightSet =
        lights.getArea(corner.x - Properties::ExtraRenderTiles() * Properties::PixelsPerTile(),
                       corner.y - Properties::ExtraRenderTiles() * Properties::PixelsPerTile(),
                       size.x + Properties::ExtraRenderTiles() * Properties::PixelsPerTile() * 2,
                       size.y + Properties::ExtraRenderTiles() * Properties::PixelsPerTile() * 2);

    sf::View view = target.getView();
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    renderSurface.setView(view);
    renderSurface.clear(sf::Color(0, 0, 0, ambient));

    bl::shapes::GradientCircle circle(100);
    circle.setCenterColor(sf::Color::Transparent);
    circle.setOuterColor(sf::Color(0, 0, 0, ambient));

    if (ambient > 80) {
        for (auto& light : lightSet) {
            circle.setPosition(static_cast<sf::Vector2f>(light.get().second.position.getValue()));
            circle.setRadius(light.get().second.radius.getValue());
            renderSurface.draw(circle, sf::BlendNone);
        }
    }

    sprite.setPosition(corner.x, corner.y + size.y);
    sprite.setScale(size.x / static_cast<float>(renderSurface.getSize().x),
                    -size.y / static_cast<float>(renderSurface.getSize().y));
    target.draw(sprite);
}

void LightingSystem::observe(const event::TimeChange& now) {
    if (sunlightField.getValue()) {
        const float x  = now.newTime.hour * 60 + now.newTime.minute;
        const float n  = 0.7;
        sunlightFactor = 1.f - (0.5 * std::cos(3.1415926 * x / 720) + 0.5) *
                                   ((1 - n) * (720 - x) * (720 - x) / 518400 + n);
    }
    else {
        sunlightFactor = 1.f;
    }
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
