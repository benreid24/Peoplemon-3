#ifndef CORE_COMPONENTS_MOVABLE_HPP
#define CORE_COMPONENTS_MOVABLE_HPP

#include <BLIB/Entities/Component.hpp>
#include <SFML/System/Vector2.hpp>

namespace core
{
namespace component
{
struct Movable {
    /// Required to be used in the BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 2;

    /// The position the entity is moving to
    sf::Vector2i movingTo;

    /// The speed to interpolate at
    float movementSpeed;

    /// The higher speed to interpolate at if moving quickly
    float fastMovementSpeed;

    /// True if moving fast, false if moving at regular speed
    bool movingFast;
};

} // namespace component
} // namespace core

#endif
