#ifndef CORE_PLAYER_INPUT_LISTENER_HPP
#define CORE_PLAYER_INPUT_LISTENER_HPP

#include <Core/Components/Command.hpp>

namespace core
{
namespace player
{
class Input;

namespace input
{
/**
 * @brief Base class for any code that needs to handle player input
 *
 * @ingroup Player
 *
 */
class Listener {
public:
    /**
     * @brief Construct a new Listener object
     *
     */
    Listener();

    /**
     * @brief Copy construct a new Listener
     *
     */
    Listener(const Listener& copy);

    /**
     * @brief Move construct a new Listener
     *
     */
    Listener(Listener&& copy);

    /**
     * @brief Destroy the Listener and unsubscribes from the input if still subscribed
     *
     */
    virtual ~Listener();

    /**
     * @brief Override this method to receive input
     *
     * @param control The active player control
     */
    virtual void process(component::Command control) = 0;

private:
    Input* owner;

    friend class ::core::player::Input;
};

} // namespace input
} // namespace player
} // namespace core

#endif
