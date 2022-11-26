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
namespace
{
constexpr float LanternRadius           = 32.f * 6.f;
constexpr float MaxVariance             = 19.f;
constexpr float MaxVarianceHoldtime     = 0.22f;
constexpr float VarianceMinConvergeRate = MaxVariance / 0.25f;
constexpr float VarianceMaxConvergeRate = MaxVariance / 0.15f;
} // namespace

using Serializer = bl::serial::json::Serializer<Player>;

Player::Player(Systems& owner)
: owner(owner)
, lantern(map::LightingSystem::None) {}

bool Player::spawnPlayer(const component::Position& pos) {
    playerId = owner.engine().ecs().createEntity();
    BL_LOG_INFO << "New player id: " << playerId;

    _position = owner.engine().ecs().addComponent<component::Position>(playerId, pos);
    owner.engine().ecs().addComponent<component::Collision>(playerId, {});

    movable = owner.engine().ecs().addComponent<component::Movable>(
        playerId,
        {*_position, Properties::CharacterMoveSpeed(), Properties::FastCharacterMoveSpeed()});

    owner.engine().ecs().addComponent<component::Controllable>(playerId, {owner, playerId});

    if (!makePlayerControlled(playerId)) { return false; }

    if (!owner.engine().ecs().addComponent<component::Renderable>(
            playerId,
            component::Renderable::fromFastMoveAnims(
                *_position, *movable, Properties::PlayerAnimations(data.sex)))) {
        BL_LOG_ERROR << "Failed to add renderble component to player";
        return false;
    }

    return true;
}

bl::ecs::Entity Player::player() const { return playerId; }

const component::Position& Player::position() const { return *_position; }

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

bool Player::makePlayerControlled(bl::ecs::Entity entity) {
    component::Controllable* controllable =
        owner.engine().ecs().getComponent<component::Controllable>(entity);
    if (!controllable) {
        BL_LOG_ERROR << "Failed to get controllable component handle for " << entity;
        return false;
    }

    component::PlayerControlled* p = owner.engine().ecs().addComponent<component::PlayerControlled>(
        entity, {owner, *controllable});
    p->start();

    return true;
}

void Player::removePlayerControlled(bl::ecs::Entity e) {
    owner.engine().ecs().removeComponent<component::PlayerControlled>(e);
}

void Player::init() { bl::event::Dispatcher::subscribe(this); }

void Player::whiteout() {
    data.healPeoplemon();
    owner.world().whiteout(data.whiteoutMap, data.whiteoutSpawn);
}

void Player::update(float dt) {
    if (lantern != map::LightingSystem::None) { updateLantern(dt); }
}

void Player::showLantern() {
    if (lantern == map::LightingSystem::None) {
        lanternVariance       = 0.f;
        lanternTargetVariance = 0.f;
        lantern               = owner.world().activeMap().lightingSystem().addLight(makeLight());
        startLanternVarianceHold();
    }
}

void Player::updateLantern(float dt) {
    auto& lighting = owner.world().activeMap().lightingSystem();

    if (lighting.lightsAreOn()) {
        if (lanternVariance != lanternTargetVariance) {
            if (lanternVariance < lanternTargetVariance) {
                lanternVariance += varianceConvergeRate * dt;
                if (lanternVariance >= lanternTargetVariance) { startLanternVarianceHold(); }
            }
            else {
                lanternVariance -= varianceConvergeRate * dt;
                if (lanternVariance <= lanternTargetVariance) { startLanternVarianceHold(); }
            }
        }
        else {
            varianceSwitchTime -= dt;
            if (varianceSwitchTime <= 0.f) { startLanternVarianceChange(); }
        }

        lighting.updateLight(lantern, makeLight());
    }
    else {
        lighting.removeLight(lantern);
        lantern = map::LightingSystem::None;
    }
}

void Player::startLanternVarianceHold() {
    lanternVariance    = lanternTargetVariance;
    varianceSwitchTime = bl::util::Random::get<float>(0.1f, MaxVarianceHoldtime);
}

void Player::startLanternVarianceChange() {
    lanternTargetVariance = bl::util::Random::get<float>(-MaxVariance, MaxVariance);
    varianceConvergeRate =
        bl::util::Random::get<float>(VarianceMinConvergeRate, VarianceMaxConvergeRate);
}

map::Light Player::makeLight() const {
    sf::Vector2i pos(position().positionPixels());
    pos.x += Properties::PixelsPerTile() / 2;
    return map::Light(static_cast<std::uint16_t>(LanternRadius + lanternVariance), pos);
}

player::State& Player::state() { return data; }

const player::State& Player::state() const { return data; }

void Player::observe(const event::GameSaveInitializing& save) {
    save.gameSave.player.inventory              = &data.bag;
    save.gameSave.player.monei                  = &data.monei;
    save.gameSave.player.peoplemon              = &data.peoplemon;
    save.gameSave.player.playerName             = &data.name;
    save.gameSave.player.sex                    = &data.sex;
    save.gameSave.player.whiteoutMap            = &data.whiteoutMap;
    save.gameSave.player.whiteoutSpawn          = &data.whiteoutSpawn;
    save.gameSave.player.repelSteps             = &data.repelSteps;
    save.gameSave.player.storage                = &data.storage.boxes;
    save.gameSave.player.visitedTowns           = &data.visitedTowns;
    save.gameSave.player.seenPeoplemon          = &data.peopledex.seenCounts;
    save.gameSave.player.firstSightingLocations = &data.peopledex.firstSawLocations;
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
