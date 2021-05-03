#ifndef CORE_SYSTEMS_PLAYER_HPP
#define CORE_SYSTEMS_PLAYER_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Player/Gender.hpp>

namespace core
{
namespace system
{
class Systems;

class Player {
public:
    Player(Systems& owner);

    bool spawnPlayer(const component::Position& position);

    bl::entity::Entity player() const;

private:
    Systems& owner;
    bl::entity::Entity playerId;
    bl::entity::Registry::ComponentHandle<component::Position> position;
    bl::entity::Registry::ComponentHandle<component::Movable> movable;

    player::Gender gender;
    // TODO - other stuff like inventory, peoplemon, etc
};

} // namespace system
} // namespace core

#endif
