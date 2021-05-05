#ifndef CORE_PLAYER_INPUT_HPP
#define CORE_PLAYER_INPUT_HPP

#include <BLIB/Events.hpp>
#include <Core/Components/Direction.hpp>
#include <Core/Player/Input/Control.hpp>
#include <Core/Player/Input/Controls.hpp>
#include <Core/Player/Input/Listener.hpp>

namespace core
{
namespace player
{
/**
 * @brief Class for processing player input and dispatching it to the appropriate listeners
 *
 * @ingroup Player
 *
 */
class Input : public bl::event::Listener<sf::Event> {
public:
    /**
     * @brief Construct a new Input object with default controls
     *
     */
    Input();

    /**
     * @brief Adds a new input listener. Only the latest listener receives input
     *
     * @param listener The listener to make active
     */
    void addListener(input::Listener& listener);

    /**
     * @brief Removes the given listener. Does not have to be the active listener
     *
     * @param listener The input listener to remove
     */
    void removeListener(input::Listener& listener);

    /**
     * @brief Replaces the old listener with the new, keeping it at the same position in the queue
     *
     * @param oldListener The listener to replace
     * @param newListener The listener to take its place
     */
    void replaceListener(const input::Listener& oldListener, input::Listener& newListener);

    /**
     * @brief Fires events for controls that are held
     *
     */
    void update();

private:
    input::Control controls[input::Controls::COUNT];
    std::vector<input::Listener*> listeners;

    bool moving;
    bool running;
    component::Direction moveDir;

    virtual void observe(const sf::Event& event) override;
    void dispatch(component::Command control);
};

} // namespace player
} // namespace core

#endif
