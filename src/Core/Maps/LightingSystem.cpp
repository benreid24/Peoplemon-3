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
LightingSystem::LightingSystem()
: lightsField(*this)
, lightLevelField(*this, 0)
, sunlightField(*this, true)
, lightLevel(255)
, eventGuard(this)
, lights(320, 320, 800, 600) {}

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
    unsigned long cdist = 0;
    for (const auto& light : set) {
        const long dx            = light.get().second.position.getValue().x - position.x;
        const long dy            = light.get().second.position.getValue().y - position.y;
        const unsigned long dist = dx * dx + dy * dy;
        if (dist < cdist) {
            cdist   = dist;
            closest = light.get().first;
        }
    }
    return closest;
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

void LightingSystem::setAmbientLevel(std::uint8_t level) {
    lightLevelField = level;
    if (level != 255) lightLevel = level;
}

std::uint8_t LightingSystem::getAmbientLevel() const { return lightLevelField.getValue(); }

void LightingSystem::adjustForSunlight(bool a) { sunlightField = a; }

bool LightingSystem::adjustsForSunlight() const { return sunlightField.getValue(); }

void LightingSystem::legacyResize(const sf::Vector2i& mapSize) {
    handles.clear();
    lightsField.getValue().clear();
    lights.setSize(mapSize.x * Properties::PixelsPerTile(),
                   mapSize.y * Properties::PixelsPerTile(),
                   Properties::WindowWidth(),
                   Properties::WindowHeight());
}

void LightingSystem::activate(bl::event::Dispatcher& bus, const sf::Vector2i& mapSize) {
    eventGuard.subscribe(bus);
    legacyResize(mapSize);

    if (lightLevel == 255) lightLevel = lightLevelField.getValue();
    for (const auto& light : lightsField.getValue()) { addLight(light, false); }

    renderSurface.create(Properties::LightingWidthTiles() * Properties::PixelsPerTile(),
                         Properties::LightingHeightTiles() * Properties::PixelsPerTile());
    sprite.setTexture(renderSurface.getTexture(), true);
    sprite.setScale(1.f, -1.f);
}

void LightingSystem::clear() {
    lightsField.getValue().clear();
    lightLevelField = 0;
    lightLevel      = 255;
    sunlightField   = true;
    lights.clear();
    handles.clear();
}

void LightingSystem::render(sf::RenderTarget& target) {
    const sf::Vector2f corner(target.getView().getCenter() - target.getView().getSize() / 2.f);
    const sf::Vector2f& size = target.getView().getSize();

    auto lightSet =
        lights.getArea(corner.x - Properties::ExtraRenderTiles() * Properties::PixelsPerTile(),
                       corner.x - Properties::ExtraRenderTiles() * Properties::PixelsPerTile(),
                       size.x + Properties::ExtraRenderTiles() * Properties::PixelsPerTile(),
                       size.x + Properties::ExtraRenderTiles() * Properties::PixelsPerTile());

    renderSurface.setView(target.getView());
    renderSurface.clear(sf::Color(0, 0, 0, lightLevel));

    bl::shapes::GradientCircle circle(100);
    circle.setCenterColor(sf::Color::Transparent);
    circle.setOuterColor(sf::Color(0, 0, 0, lightLevel));

    for (auto& light : lightSet) {
        circle.setPosition(static_cast<sf::Vector2f>(light.get().second.position.getValue()));
        circle.setRadius(light.get().second.radius.getValue());
        renderSurface.draw(circle, sf::BlendNone);
    }

    sprite.setPosition(corner.x, target.getView().getCenter().y + size.y / 2.f);
    sprite.setScale(size.x / static_cast<float>(renderSurface.getSize().x),
                    -size.y / static_cast<float>(renderSurface.getSize().y));
    target.draw(sprite);
}

void LightingSystem::observe(const event::TimeChange& now) {
    if (lightLevelField.getValue() == 255) {
        const float x = now.newTime.hour * 60 + now.newTime.minute;
        const float n = 0.7;
        lightLevel    = std::max(0.,
                              (90 * cos(3.1415926 * x / 720) + 90) *
                                  ((1 - n) * (720 - x) * (720 - x) / 518400 + n));
        // TODO - add weather light modifier
    }
}

} // namespace map
} // namespace core
