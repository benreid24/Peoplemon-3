#ifndef CORE_MAPS_LIGHTINGSYSTEM_HPP
#define CORE_MAPS_LIGHTINGSYSTEM_HPP

#include <BLIB/Containers/Grid.hpp>
#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Lighting/Scene2DLighting.hpp>
#include <BLIB/Serialization/Binary.hpp>
#include <Core/Events/TimeChange.hpp>
#include <Core/Events/Weather.hpp>
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
 */
class LightingSystem
: public bl::event::Listener<event::TimeChange, event::WeatherStarted, event::WeatherStopped> {
public:
    /// Handle representing a light in the map
    using Handle = std::uint16_t;

    /// Special handle indicating that no light is referenced
    static constexpr Handle None = 0;

    /**
     * @brief Initializes the lighting system with no lights
     */
    LightingSystem();

    /**
     * @brief Destroys the lighting system
     */
    virtual ~LightingSystem() = default;

    /**
     * @brief Adds a light to the map and to the persistent map file
     *
     * @param light The light to add
     */
    void addLight(const Light& light);

    /**
     * @brief Updates the light with the given handle to the new information
     *
     * @param handle The handle referencing the light to update
     * @param value The new light information
     */
    void updateLight(Handle handle, const Light& value);

    /**
     * @brief Returns a handle to the light closest to the given position
     *
     * @param position The position to search near
     * @return Handle A handle to the closest light. Do not store as handles may be invalidated
     */
    Handle getClosestLight(const sf::Vector2i& position);

    /**
     * @brief Returns the value of the light with the given handle. Invalid handles return a default
     *        value (a light with a negative position and 0 radius)
     *
     * @param handle The handle to get the light for
     * @return const Light& The value of the light with the given handle
     */
    const Light& getLight(Handle handle) const;

    /**
     * @brief Remove the given light from the system and optionally persist the removal
     *
     * @param light A handle to the light to remove
     * @param updateSave True to update the data saved to the map file (editor only)
     */
    void removeLight(Handle light);

    /**
     * @brief Returns whether lights are being shown or not based on how dark it is
     */
    bool lightsAreOn() const;

    /**
     * @brief The ambient light band. 0 is full darkness and 255 is full brightness
     *
     * @param lowLightLevel The min light level. This is reached at midnight
     * @param highLightLevel The max light level. This is reached at noon
     */
    void setAmbientLevel(std::uint8_t lowLightLevel, std::uint8_t highLightLevel);

    /**
     * @brief Returns the minimum ambient light level of this map
     *
     * @return std::uint8_t The minimum ambient light level
     */
    std::uint8_t getMinLightLevel() const;

    /**
     * @brief Returns the maximum ambient light level of this map
     *
     * @return std::uint8_t The maximum ambient light level
     */
    std::uint8_t getMaxLightLevel() const;

    /**
     * @brief Set whether or not the light level is adjusted based on time of day. If not adjusting
     *        for sunlight the higher light level is used for a constant ambient
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
     * @brief Adds all lights into the scene
     *
     * @param sceneLighting The lighting manager for the map scene
     */
    void activate(bl::rc::lgt::Scene2DLighting& sceneLighting);

    /**
     * @brief Subscribes the lighting system to time events for ambient light level
     *
     */
    void subscribe();

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
     * @brief Updates the lighting system
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Updates the light level based on the new current time
     *
     * @param timeChange The new current time
     */
    virtual void observe(const event::TimeChange& timeChange) override;

    /**
     * @brief Adjusts the ambient lighting based on the current weather
     *
     * @param event The new weather that has started
     */
    virtual void observe(const event::WeatherStarted& event) override;

    /**
     * @brief Resets the ambient lighting when weather stops
     *
     * @param event The weather that just stopped
     */
    virtual void observe(const event::WeatherStopped& event) override;

private:
    std::vector<Light> rawLights;
    std::uint8_t minLevel;
    std::uint8_t maxLevel;
    std::uint8_t sunlight;

    bl::rc::lgt::Scene2DLighting* sceneLighting;
    std::vector<bl::rc::lgt::Light2D> activeLights;
    bool lightsActive;

    float levelRange;
    float sunlightFactor;
    int weatherModifier;
    int targetWeatherModifier;
    float weatherResidual;

    std::uint8_t computeAmbient() const;
    void addLightToScene(const Light& light);
    void updateAmbientLighting();

    friend struct bl::serial::SerializableObject<LightingSystem>;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::LightingSystem> : public SerializableObjectBase {
    using LS = core::map::LightingSystem;

    SerializableField<1, LS, std::vector<core::map::Light>> lights;
    SerializableField<2, LS, std::uint8_t> low;
    SerializableField<3, LS, std::uint8_t> high;
    SerializableField<4, LS, std::uint8_t> sun;

    SerializableObject()
    : SerializableObjectBase("LightingSystem")
    , lights("lights", *this, &LS::rawLights, SerializableFieldBase::Required{})
    , low("low", *this, &LS::minLevel, SerializableFieldBase::Required{})
    , high("high", *this, &LS::maxLevel, SerializableFieldBase::Required{})
    , sun("sun", *this, &LS::sunlight, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
