#include <Core/Systems/Player.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Components/PlayerControlled.hpp>
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

    if (!owner.engine().entities().addComponent<component::Controllable>(playerId,
                                                                         {owner, playerId})) {
        BL_LOG_ERROR << "Failed to add controllable component to player";
        return false;
    }

    if (!makePlayerControlled(playerId)) { return false; }

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

player::Input& Player::inputSystem() { return input; }

bool Player::makePlayerControlled(bl::entity::Entity entity) {
    auto controllable =
        owner.engine().entities().getComponentHandle<component::Controllable>(entity);
    if (!controllable.hasValue()) {
        BL_LOG_ERROR << "Failed to get controllable component handle for " << entity;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::PlayerControlled>(
            entity, {owner, controllable})) {
        BL_LOG_ERROR << "Failed to add player controlled component to " << entity;
        return false;
    }

    component::PlayerControlled* p =
        owner.engine().entities().getComponent<component::PlayerControlled>(entity);
    if (p) { p->start(); }
    else {
        BL_LOG_ERROR << "Failed to start player controlled component for entity: " << entity;
    }

    return true;
}

void Player::removePlayerControlled(bl::entity::Entity e) {
    owner.engine().entities().removeComponent<component::PlayerControlled>(e);
}

void Player::init() { owner.engine().eventBus().subscribe(&input); }

void Player::update() { input.update(); }

} // namespace system
} // namespace core
