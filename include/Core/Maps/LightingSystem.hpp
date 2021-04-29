#ifndef CORE_MAPS_LIGHTINGSYSTEM_HPP
#define CORE_MAPS_LIGHTINGSYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Containers/Grid.hpp>
#include <BLIB/Containers/QuadTree.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Files/Binary.hpp>
#include <Core/Events/TimeChange.hpp>
#include <Core/Maps/Light.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief System for handling lighting in Maps. Manages all the lights and performs rendering. Also
 *        handles the loading and saving of lights to map files
 *
 * @ingroup Maps
 *
 */
class LightingSystem
: public bl::file::binary::SerializableObject
, public bl::event::Listener<event::TimeChange> {
public:
    /// Handle representing a light in the map
    using Handle = std::uint16_t;

    /// Special handle indicating that no light is referenced
    static constexpr Handle None = 0;

    /**
     * @brief Initializes the lighting system with no lights
     *
     */
    LightingSystem();

    /**
     * @brief Adds a light to the map and optionally to the persistent map file
     *
     * @param light The light to add
     * @param updateSave True to update the data saved to the map file (editor only)
     * @return Handle The handle referencing the newly created light
     */
    Handle addLight(const Light& light, bool updateSave = false);

    /**
     * @brief Updates the light with the given handle to the new information
     *
     * @param handle The handle referencing the light to update
     * @param value The new light information
     * @param updateSave True to update the data saved to the map file (editor only)
     */
    void updateLight(Handle handle, const Light& value, bool updateSave = false);

    /**
     * @brief Returns a handle to the light closest to the given position
     *
     * @param position The position to search near
     * @return Handle A handle to the closest light. May be None based on QuadTree partitions
     */
    Handle getClosestLight(const sf::Vector2i& position);

    /**
     * @brief Remove the given light from the system and optionally persist the removal
     *
     * @param light A handle to the light to remove
     * @param updateSave True to update the data saved to the map file (editor only)
     */
    void removeLight(Handle light, bool updateSave = false);

    /**
     * @brief Set the ambient light level of the map. Areas not illuminated will be lit to a max of
     *        this light level. If the map is in sunlight this is the highest brightness during
     *        daytime. Night will get darker from this level. Default is 255
     *
     * @param lightLevel The ambient light level. 0 is full darkness, 255 is full brightness
     */
    void setAmbientLevel(std::uint8_t lightLevel);

    /**
     * @brief Returns the ambient light level of this map
     *
     * @return std::uint8_t The ambient light level
     */
    std::uint8_t getAmbientLevel() const;

    /**
     * @brief Set whether or not the light level is adjusted based on time of day
     *
     * @param adjust True to adjust based on time of day, false to always use ambient level
     */
    void adjustForSunlight(bool adjust);

    /**
     * @brief Returns whether or not this map is adjusted for time of day
     *
     * @return True is adjusting light level based on time, false if constant light level
     */
    bool adjustsForSunlight() const;

    /**
     * @brief Prepares the lighting system to take lights from the LegacyMapLoader. Clears all
     *        lights
     *
     * @param mapSize The map size to prepare for
     */
    void legacyResize(const sf::Vector2i& mapSize);

    /**
     * @brief This must be called after a map is loaded and before any rendering is attempted.
     *        Loaded data is synced into runtime structures
     *
     * @param eventBus The engine event bus
     * @param mapSize The size of the map in tiles. Used for quadtree partitioning
     */
    void activate(const sf::Vector2i& mapSize);

    /**
     * @brief Subscribes the lighting system to time events for ambient light level
     *
     */
    void subscribe(bl::event::Dispatcher& eventBus);

    /**
     * @brief Unsubscribes the lighting system from the event dispatcher
     *
     */
    void unsubscribe();

    /**
     * @brief Clears all lights from the map, including the persisted light data
     *
     */
    void clear();

    /**
     * @brief Renders the current lighting to the given target. This should be called at the end of
     *        the rendering process to ensure that all layers and entities are lit properly
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target);

    /**
     * @brief Updates the light level based on the new current time
     *
     * @param timeChange The new current time
     */
    virtual void observe(const event::TimeChange& timeChange) override;

private:
    bl::file::binary::SerializableField<1, std::vector<Light>> lightsField;
    bl::file::binary::SerializableField<2, std::uint8_t> lightLevelField;
    bl::file::binary::SerializableField<3, bool> sunlightField;

    bl::event::ClassGuard<event::TimeChange> eventGuard;

    using Storage = bl::container::Grid<std::pair<Handle, Light>>;
    std::unordered_map<Handle, Storage::Payload::Ptr> handles;
    Storage lights;

    sf::RenderTexture renderSurface;
    sf::Sprite sprite;
    std::uint8_t lightLevel;
};

} // namespace map
} // namespace core

#endif
