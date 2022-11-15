#ifndef CORE_COMPONENTS_MOVABLE_HPP
#define CORE_COMPONENTS_MOVABLE_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Events.hpp>
#include <Core/Components/Position.hpp>
#include <SFML/System/Vector2.hpp>

namespace core
{
namespace system
{
class Movement;
}
namespace component
{
class Renderable;

/**
 * @brief Adding this component to an entity will allow it to move
 *
 * @ingroup Components
 *
 */
class Movable {
public:
    /**
     * @brief Construct a new Movable component
     *
     * @param pos A handle to the position component of the owning entity
     * @param speed The speed to move at
     * @param fastSpeed The speed to move at when moving fast
     */
    Movable(Position& pos, float speed, float fastSpeed);

    /**
     * @brief Returns whether or not the entity is moving
     *
     */
    bool moving() const;

    /**
     * @brief Returns whether or not the entity is going fast
     *
     */
    bool goingFast() const;

    /**
     * @brief Starts the entity moving in the given direction. Only updates the interpolated
     *        position, not the tile position. This does not check collisions. Use the Movement
     *        system to ensure the move is valid
     *
     * @param dir The direction to move in
     * @param fast True to move fast, false to move slow
     * @param isLedgeHop Indicates whether we are hopping or moving regularly
     */
    void move(Direction dir, bool fast, bool isLedgeHop);

    /**
     * @brief Updates the interpolation of the entity if moving
     *
     * @param owner The owning entity of this component
     * @param engine The game engine
     * @param dt Time elapsed in seconds since last call to update
     */
    void update(bl::ecs::Entity owner, bl::engine::Engine& engine, float dt);

private:
    enum struct MoveState : std::uint8_t { Still, Moving, MovingFast, LedgeHopping };

    Position& position;
    float movementSpeed;
    float fastMovementSpeed;
    float interpRemaining;
    Direction moveDir;
    MoveState state;

    friend class system::Movement;
};

} // namespace component
} // namespace core

#endif
