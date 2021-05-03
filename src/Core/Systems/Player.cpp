#include <Core/Systems/Player.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Player::Player(Systems& owner)
: owner(owner)
, gender(player::Gender::Boy) {}

bool Player::spawnPlayer(const component::Position& pos) {
    playerId = owner.engine().entities().createEntity();
    BL_LOG_INFO << "New player id: " << playerId;

    if (!owner.engine().entities().addComponent<component::Position>(playerId, pos)) {
        BL_LOG_ERROR << "Failed to add position to player";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Collision>(playerId, {})) {
        BL_LOG_ERROR << "Failed to add collision to player";
        return false;
    }

    position = owner.engine().entities().getComponentHandle<component::Position>(playerId);
    if (!position.hasValue()) {
        BL_LOG_ERROR << "Failed to get position handle for player";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Movable>(
            playerId,
            {position, Properties::CharacterMoveSpeed(), Properties::FastCharacterMoveSpeed()})) {
        BL_LOG_ERROR << "Failed to add movable component to player";
        return false;
    }

    movable = owner.engine().entities().getComponentHandle<component::Movable>(playerId);
    if (!movable.hasValue()) {
        BL_LOG_ERROR << "Failed to get movable handle for player";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Renderable>(
            playerId,
            component::Renderable::fromFastMoveAnims(
                position, movable, Properties::PlayerAnimations(gender)))) {
        BL_LOG_ERROR << "Failed to add renderble component to player";
        return false;
    }

    return true;
}

bl::entity::Entity Player::player() const { return playerId; }

} // namespace system
} // namespace core
