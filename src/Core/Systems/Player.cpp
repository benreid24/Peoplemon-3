#include <Core/Systems/Player.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Components/PlayerControlled.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

#include <Core/Files/GameSave.hpp>

namespace core
{
namespace system
{
using Serializer = bl::serial::json::Serializer<Player>;

Player::Player(Systems& owner)
: owner(owner)
, gender(player::Gender::Boy)
, monei(0) {}

bool Player::spawnPlayer(const component::Position& pos) {
    playerId = owner.engine().entities().createEntity();
    BL_LOG_INFO << "New player id: " << playerId;

    if (!owner.engine().entities().addComponent<component::Position>(playerId, pos)) {
        BL_LOG_ERROR << "Failed to add _position to player";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Collision>(playerId, {})) {
        BL_LOG_ERROR << "Failed to add collision to player";
        return false;
    }

    _position = owner.engine().entities().getComponentHandle<component::Position>(playerId);
    if (!_position.hasValue()) {
        BL_LOG_ERROR << "Failed to get _position handle for player";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Movable>(
            playerId,
            {_position, Properties::CharacterMoveSpeed(), Properties::FastCharacterMoveSpeed()})) {
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
                _position, movable, Properties::PlayerAnimations(gender)))) {
        BL_LOG_ERROR << "Failed to add renderble component to player";
        return false;
    }

    return true;
}

bl::entity::Entity Player::player() const { return playerId; }

const component::Position& Player::position() const { return _position.get(); }

player::Input& Player::inputSystem() { return input; }

const std::string& Player::name() const { return playerName; }

std::vector<pplmn::OwnedPeoplemon>& Player::team() { return peoplemon; }

const std::vector<pplmn::OwnedPeoplemon>& Player::team() const { return peoplemon; }

player::Bag& Player::bag() { return inventory; }

const player::Bag& Player::bag() const { return inventory; }

long Player::money() const { return monei; }

long& Player::money() { return monei; }

void Player::newGame(const std::string& n, player::Gender g) {
    playerName = n;
    gender     = g;
    inventory.clear();
    monei = 0;
    peoplemon.clear();

#ifdef PEOPLEMON_DEBUG
    peoplemon.emplace_back(pplmn::Id::AnnaA, 30);
    peoplemon.back().learnMove(pplmn::MoveId::Awkwardness, 0);
    peoplemon.back().learnMove(pplmn::MoveId::Confuse, 1);
    peoplemon.back().learnMove(pplmn::MoveId::MedicalAttention, 2);
    peoplemon.back().learnMove(pplmn::MoveId::Oblivious, 3);
#endif
}

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

void Player::init() {
    owner.engine().eventBus().subscribe(&input);
    owner.engine().eventBus().subscribe(this);
}

void Player::update() { input.update(); }

void Player::observe(const event::GameSaving& save) {
    Serializer::serializeInto(save.saveData, "player", *this);
}

void Player::observe(const event::GameLoading& load) {
    if (!Serializer::deserializeFrom(load.saveData, "player", *this)) {
        load.failMessage = "Failed to load player data from save";
    }
}

} // namespace system
} // namespace core
