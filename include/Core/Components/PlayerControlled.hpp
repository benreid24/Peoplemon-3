#ifndef CORE_COMPONENTS_PLAYERCONTROLLED_HPP
#define CORE_COMPONENTS_PLAYERCONTROLLED_HPP

#include <Core/Components/Controllable.hpp>
#include <Core/Input/Control.hpp>

namespace core
{
namespace system
{
class Systems;
}

namespace component
{
/**
 * @brief Add this component to an entity to make it controlled by player input
 *
 * @ingroup Components
 *
 */
class PlayerControlled : public bl::input::Listener {
public:
    /**
     * @brief Construct a new Player Controlled component and immediately start taking player input.
     *        Whatever was previously receiving player input is cut off
     *
     * @param systems The primary systems object
     * @param controllable The controllable component to interface with
     */
    PlayerControlled(system::Systems& systems, Controllable& controllable);

    /**
     * @brief Destroy the Player Controlled component
     *
     */
    virtual ~PlayerControlled() = default;

    /**
     * @brief Activate this listener and receive player input. Does not have effect if already
     *        activated but another input listener has taken precedence
     *
     */
    void start();

    /**
     * @brief Stops listening to player input
     *
     */
    void stop();

    /**
     * @brief Forwards the player input to the underlying entity
     *
     * @param ctrl The input to forward
     */
    virtual bool observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                         bool) override;

private:
    system::Systems& systems;
    component::Controllable& controllable;
    bool started;
};

} // namespace component
} // namespace core

#endif
