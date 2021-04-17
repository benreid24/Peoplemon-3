#include <BLIB/Media/Shapes/GradientCircle.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Maps/LightingSystem.hpp>
#include <Core/Properties.hpp>
#include <limits>

namespace core
{
namespace map
{
LightingSystem::LightingSystem()
: lightsField(*this)
, lightLevelField(*this, 255)
, sunlightField(*this, true) {}

LightingSystem::Handle LightingSystem::addLight(const Light& light, bool p) {
    Iterator it = lights.add(light);
    const Handle h =
        bl::util::Random::get<Handle>(static_cast<Handle>(1), std::numeric_limits<Handle>::max());
    handles.emplace(h, it);
    lightTree.add(light.position.getValue(), std::make_pair(h, it));

    if (p) { lightsField.getValue().push_back(light); }
    return h;
}

void LightingSystem::updateLight(Handle handle, const Light& light, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;

    if (p) {
        for (auto& l : lightsField.getValue()) {
            if (l.position.getValue() == it->second->position.getValue()) {
                l = light;
                break;
            }
        }
    }

    lightTree.updatePosition(it->second->position.getValue(), light.position.getValue());
    *it->second = light;
}

LightingSystem::Handle LightingSystem::getClosestLight(const sf::Vector2i& position) {
    auto set =
        lightTree.getInArea({{position.x - Properties::PixelsPerTile() * 2,
                              position.y - Properties::PixelsPerTile() * 2},
                             {Properties::PixelsPerTile() * 4, Properties::PixelsPerTile() * 4}});
    Handle closest      = None;
    unsigned long cdist = 0;
    for (auto pair : set) {
        const long dx            = pair.second->position.getValue().x - position.x;
        const long dy            = pair.second->position.getValue().y - position.y;
        const unsigned long dist = dx * dx + dy * dy;
        if (dist < cdist) {
            cdist   = dist;
            closest = pair.first;
        }
    }
    return closest;
}

void LightingSystem::removeLight(Handle handle, bool p) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;

    if (p) {
        for (unsigned int i = 0; i < lightsField.getValue().size(); ++i) {
            if (lightsField.getValue()[i].position.getValue() == it->second->position.getValue()) {
                lightsField.getValue().erase(lightsField.getValue().begin() + i);
                break;
            }
        }
    }

    lightTree.remove(it->second->position.getValue());
    lights.erase(it->second);
    handles.erase(it);
}

void LightingSystem::setAmbientLevel(std::uint8_t level) { lightLevelField = level; }

std::uint8_t LightingSystem::getAmbientLevel() const { return lightLevelField.getValue(); }

void LightingSystem::adjustForSunlight(bool a) { sunlightField = a; }

bool LightingSystem::adjustsForSunlight() const { return sunlightField.getValue(); }

void LightingSystem::activate(const sf::Vector2i& mapSize) {
    lights.clear();
    handles.clear();
    lightTree.clear();

    lightTree.setIndexedArea(
        {{0, 0},
         {mapSize.x * Properties::PixelsPerTile(), mapSize.y * Properties::PixelsPerTile()}});
    lightTree.setMaxLoad(5);

    for (const auto& light : lightsField.getValue()) { addLight(light, false); }

    renderSurface.create(Properties::LightingWidthTiles() * Properties::PixelsPerTile(),
                         Properties::LightingHeightTiles() * Properties::PixelsPerTile());
    sprite.setTexture(renderSurface.getTexture(), true);
    sprite.setScale(1.f, -1.f);
}

void LightingSystem::clear() {
    lightsField.getValue().clear();
    lightLevelField = 255;
    sunlightField   = true;
    lights.clear();
    handles.clear();
    lightTree.clear();
}

void LightingSystem::render(sf::RenderTarget& target) {
    const int w = std::ceil(target.getView().getSize().x / renderSurface.getSize().x);
    const int h = std::ceil(target.getView().getSize().y / renderSurface.getSize().y);
    const sf::Vector2f cornerF = target.getView().getCenter() - target.getView().getSize() / 2.f;
    const sf::Vector2i corner(std::floor(cornerF.x), std::floor(cornerF.y));
    const sf::Vector2i size = static_cast<sf::Vector2i>(renderSurface.getSize());

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            renderSection(target, {corner.x + size.x * x, corner.y + size.y * y});
        }
    }
}

void LightingSystem::renderSection(sf::RenderTarget& target, const sf::Vector2i& position) {
    const sf::Vector2f pos(position.x, position.y);
    auto lightSet =
        lightTree.getInArea({position, static_cast<sf::Vector2i>(renderSurface.getSize())});

    const std::uint8_t lightLevel = lightLevelField.getValue(); // TODO - account for time of day
    renderSurface.clear(sf::Color(0, 0, 0, lightLevel));

    bl::shapes::GradientCircle circle(100);
    circle.setCenterColor(sf::Color::Transparent);
    circle.setOuterColor(sf::Color(0, 0, 0, lightLevel));

    for (auto& pair : lightSet) {
        circle.setPosition(static_cast<sf::Vector2f>(pair.second->position.getValue() - position));
        renderSurface.draw(circle, sf::BlendNone);
    }

    target.draw(sprite);
}

} // namespace map
} // namespace core
