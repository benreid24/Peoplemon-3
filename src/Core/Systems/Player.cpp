#include <Core/Systems/Player.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Components/PlayerControlled.hpp>
#include <Core/Files/GameSave.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
using Serializer = bl::serial::json::Serializer<Player>;

Player::Player(Systems& owner)
: owner(owner) {}

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
                _position, movable, Properties::PlayerAnimations(data.sex)))) {
        BL_LOG_ERROR << "Failed to add renderble component to player";
        return false;
    }

    return true;
}

bl::entity::Entity Player::player() const { return playerId; }

const component::Position& Player::position() const { return _position.get(); }

player::Input& Player::inputSystem() { return input; }

void Player::newGame(const std::string& n, player::Gender g) {
    data.name = n;
    data.sex  = g;
    data.bag.clear();
    data.monei = 0;
    data.peoplemon.clear();

#ifdef PEOPLEMON_DEBUG
    data.monei = 100000;
    data.peoplemon.emplace_back(pplmn::Id::AnnaA, 30);
    data.peoplemon.back().learnMove(pplmn::MoveId::Awkwardness, 0);
    data.peoplemon.back().learnMove(pplmn::MoveId::Confuse, 1);
    data.peoplemon.back().learnMove(pplmn::MoveId::MedicalAttention, 2);
    data.peoplemon.back().learnMove(pplmn::MoveId::Oblivious, 3);
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

void Player::whiteout() {
    for (auto& ppl : data.peoplemon) { ppl.heal(); }
    if (!owner.world().whiteout(data.whiteoutMap, data.whiteoutSpawn)) {
        BL_LOG_CRITICAL << "Failed to whiteout";
        owner.engine().flags().set(bl::engine::Flags::Terminate);
    }
}

void Player::update() { input.update(); }

player::State& Player::state() { return data; }

const player::State& Player::state() const { return data; }

void Player::observe(const event::GameSaveInitializing& save) {
    save.gameSave.player.inventory     = &data.bag;
    save.gameSave.player.monei         = &data.monei;
    save.gameSave.player.peoplemon     = &data.peoplemon;
    save.gameSave.player.playerName    = &data.name;
    save.gameSave.player.sex           = &data.sex;
    save.gameSave.player.whiteoutMap   = &data.whiteoutMap;
    save.gameSave.player.whiteoutSpawn = &data.whiteoutSpawn;
    save.gameSave.player.repelSteps    = &data.repelSteps;
    save.gameSave.player.storage       = &data.storage.boxes;
    save.gameSave.player.visitedTowns  = &data.visitedTowns;
}

void Player::observe(const event::EntityMoveFinished& ent) {
    if (ent.entity == playerId) {
        if (data.repelSteps > 0) {
            --data.repelSteps;
            if (data.repelSteps == 0) { owner.hud().displayMessage("Repel wore off"); }
        }

        // MrExtra ability
        for (auto& ppl : data.peoplemon) {
            if (ppl.holdItem() != item::Id::None) continue;

            if (ppl.ability() == pplmn::SpecialAbility::MrExtra) {
                if (bl::util::Random::get<int>(0, 1000) < 5) {
                    item::Id pickedUp;
                    do {
                        pickedUp = item::Item::validIds()[bl::util::Random::get<unsigned int>(
                            0, item::Item::validIds().size() - 1)];
                    } while (item::Item::getCategory(pickedUp) == item::Category::Key);

                    ppl.holdItem() = pickedUp;
                    owner.hud().displayMessage(ppl.name() + " picked up a " +
                                               item::Item::getName(pickedUp) +
                                               " they found on the ground.");
                }
            }
        }
    }
}

} // namespace system
} // namespace core
