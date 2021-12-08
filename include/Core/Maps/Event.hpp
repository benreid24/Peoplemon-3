#ifndef CORE_MAPS_EVENT_HPP
#define CORE_MAPS_EVENT_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string>

namespace core
{
namespace map
{
/**
 * @brief Represents an event in a Map. A script that is run on a trigger within a given region
 *
 * @ingroup Maps
 *
 */
struct Event {
    /// What action triggers the event
    enum struct Trigger : std::uint8_t {
        /// The event triggers when the player steps into the zone
        OnEnter = 1,

        /// The event triggers when the player steps out of the zone
        OnExit = 2,

        /// The event triggers when the player either enters or exits the zone
        onEnterOrExit = 3,

        /// The event triggers repeatedly while the player is in the zone
        WhileIn = 4,

        /// The event triggers when the player interacts with the zone
        OnInteract = 5
    };

    Trigger trigger;
    sf::Vector2i position;
    sf::Vector2i areaSize;
    std::string script;

    /**
     * @brief Creates an empty event
     *
     */
    Event();

    /**
     * @brief Creates an event from the given parameters
     *
     * @param script The script to run when triggered
     * @param pos The position of the event trigger zone, in tiles
     * @param size The size of the event trigger zone, in tiles
     * @param trigger The trigger that starts the event
     */
    Event(const std::string& script, const sf::Vector2i& pos, const sf::Vector2i& size,
          Trigger trigger);
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct SerializableObject<core::map::Event> : public SerializableObjectBase {
    using E = core::map::Event;

    SerializableField<1, core::map::Event::Trigger, offsetof(E, trigger)> trigger;
    SerializableField<2, sf::Vector2i, offsetof(E, position)> position;
    SerializableField<3, sf::Vector2i, offsetof(E, areaSize)> areaSize;
    SerializableField<4, std::string, offsetof(E, script)> script;

    SerializableObject()
    : trigger(*this)
    , position(*this)
    , areaSize(*this)
    , script(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
