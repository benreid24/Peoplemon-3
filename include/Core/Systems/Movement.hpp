#ifndef CORE_SYSTEMS_MOVEMENT_HPP
#define CORE_SYSTEMS_MOVEMENT_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Position.hpp>

namespace core
{
namespace system
{
class Systems;

class Movement {
public:
    Movement(Systems& owner);

    bool moveEntity(bl::entity::Entity entity, core::component::Direction direction);

    void update(float dt);

private:
    Systems& owner;
};

} // namespace system
} // namespace core

#endif
