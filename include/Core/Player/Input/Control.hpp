#ifndef CORE_PLAYER_INPUT_CONTROL_HPP
#define CORE_PLAYER_INPUT_CONTROL_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Components/Control.hpp>
#include <SFML/Window.hpp>
#include <string>

namespace core
{
namespace player
{
/// Collection of classes for processing player input
namespace input
{
/**
 * @brief Basic struct encapsulating a player input control
 *
 * @ingroup Player
 *
 */
struct Control {
    /**
     * @brief Parses the control from the given string
     *
     * @param str
     * @return Control
     */
    static Control fromString(const std::string& str);

    /**
     * @brief Creates a control from the gien event
     *
     * @param event The event to create the control from
     * @return Control The produced control. May be Invalid
     */
    static Control fromEvent(const sf::Event& event);

    /**
     * @brief Creates an empty invalid control
     *
     */
    Control();

    /**
     * @brief Creates a control for the given key
     *
     * @param key The key to be active on
     */
    Control(sf::Keyboard::Key key);

    /**
     * @brief Creates a control for the given mouse button
     *
     * @param button The mouse button to be active on
     */
    Control(sf::Mouse::Button button);

    /**
     * @brief Returns the string representation of the control
     *
     */
    const std::string& toString() const;

    /**
     * @brief Returns whether or not the given event effects this control
     *
     * @param event The event to check
     * @return True if the event is for this control, false if not
     */
    bool matches(const sf::Event& event) const;

    /**
     * @brief Returns whether or not the given event activates this control
     *
     * @param event The event to process
     * @return True if this control was activated, false if not
     */
    bool activated(const sf::Event& event) const;

    /**
     * @brief Returns whether or not the given event deactivates this control
     *
     * @param event The event to process
     * @return True if the control is deactivated by the event
     */
    bool deactivated(const sf::Event& event) const;

    union {
        /// The key of the control if a key
        sf::Keyboard::Key key;

        /// The button of the control if a button
        sf::Mouse::Button button;
    };

    /**
     * @brief The type of control (keyboard or mouse)
     *
     */
    enum Type : std::uint8_t {
        /// The control is invalid or empty
        Invalid = 0,

        /// The control is on the keyboard
        Key = 1,

        /// The control is on the mouse
        Mouse = 2
    } type;
};
} // namespace input
} // namespace player
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
using core::player::input::Control;

template<>
struct Serializer<Control, false> {
    static bool serialize(File& output, const Control& c) {
        return Serializer<std::string>::serialize(output, c.toString());
    }

    static bool deserialize(File& input, Control& c) {
        std::string s;
        if (!Serializer<std::string>::deserialize(input, s)) return false;
        c = Control::fromString(s);
        return c.type != Control::Invalid;
    }

    static std::uint32_t size(const Control& c) {
        return Serializer<std::string>::size(c.toString());
    }
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
