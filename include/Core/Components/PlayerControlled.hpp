#ifndef CORE_COMPONENTS_PLAYERCONTROLLED_HPP
#define CORE_COMPONENTS_PLAYERCONTROLLED_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Controllable.hpp>
#include <Core/Player/Input.hpp>
#include <Core/Player/Input/Listener.hpp>

namespace core
{
namespace component
{
/**
 * @brief Add this component to an entity to make it controlled by player input
 *
 * @ingroup Components
 *
 */
class PlayerControlled : public player::input::Listener {
public:
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 6;

    /**
     * @brief Construct a new Player Controlled component and immediately start taking player input.
     *        Whatever was previously receiving player input is cut off
     *
     * @param controllable The controllable component to interface with
     * @param input The player input object to listen to
     */
    PlayerControlled(
        const bl::entity::Registry::ComponentHandle<component::Controllable>& controllable,
        player::Input& input);

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
     * @param input The input to forward
     */
    virtual void process(Command input) override;

private:
    player::Input& input;
    bl::entity::Registry::ComponentHandle<component::Controllable> controllable;
    bool started;
};

} // namespace component
} // namespace core

#endif
