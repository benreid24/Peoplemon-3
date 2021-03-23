#ifndef CORE_MAPS_LIGHTINGSYSTEM_HPP
#define CORE_MAPS_LIGHTINGSYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Containers/QuadTree.hpp>
#include <BLIB/Files/Binary.hpp>
#include <Core/Maps/Light.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

namespace core
{
namespace map
{
class LightingSystem : public bl::file::binary::SerializableObject {
public:
    using Handle                 = std::uint16_t;
    static constexpr Handle None = 0;

    LightingSystem();

    Handle addLight(const Light& light, bool updateSave = false);

    void updateLight(Handle handle, const Light& value, bool updateSave = false);

    Handle getClosestLight(const sf::Vector2i& position);

    void removeLight(Handle light, bool updateSave = false);

    void setAmbientLevel(std::uint8_t lightLevel);

    std::uint8_t getAmbientLevel() const;

    void adjustForSunlight(bool adjust);

    bool adjustsForSunlight() const;

    // TODO - adjust ambient based on time of day

    void activate(const sf::Vector2i& mapSize); // sync with loaded fields

    void clear();

    void render(sf::RenderTarget& target);

private:
    bl::file::binary::SerializableField<1, std::vector<Light>> lightsField;
    bl::file::binary::SerializableField<2, std::uint8_t> lightLevelField;
    bl::file::binary::SerializableField<3, bool> sunlightField;

    using Iterator = bl::container::DynamicObjectPool<Light>::Iterator;
    bl::container::DynamicObjectPool<Light> lights;
    std::unordered_map<Handle, Iterator> handles;
    bl::container::QuadTree<sf::Vector2i, std::pair<Handle, Iterator>> lightTree;

    sf::RenderTexture renderSurface;
    sf::Sprite sprite;

    void renderSection(sf::RenderTarget& target, const sf::Vector2i& position);
};

} // namespace map
} // namespace core

#endif
