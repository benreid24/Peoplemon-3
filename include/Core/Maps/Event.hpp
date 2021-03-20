#ifndef CORE_MAPS_EVENT_HPP
#define CORE_MAPS_EVENT_HPP

#include <BLIB/Files/Binary.hpp>
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
struct Event : public bl::file::binary::SerializableObject {
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

    bl::file::binary::SerializableField<1, Trigger> trigger;
    bl::file::binary::SerializableField<2, sf::Vector2i> position;
    bl::file::binary::SerializableField<3, sf::Vector2i> size;
    bl::file::binary::SerializableField<4, std::string> script;

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

    /**
     * @brief Copy constructs the event
     *
     * @param copy The event to copy from
     */
    Event(const Event& copy);

    /**
     * @brief Copies from the given event
     *
     * @param copy The event to copy
     * @return Event& A reference to this event
     */
    Event& operator=(const Event& copy);
};

} // namespace map
} // namespace core

#endif
