#ifndef CORE_COMPONENTS_COLLISION_HPP
#define CORE_COMPONENTS_COLLISION_HPP

#include <BLIB/Entities.hpp>

namespace core
{
namespace component
{
/**
 * @brief Empty component to indicate that an entity cannot be moved through
 *
 * @ingroup Components
 *
 */
struct Collision {
    static constexpr bl::entity::Component::IdType ComponentId = 3;
};

} // namespace component
} // namespace core

#endif
