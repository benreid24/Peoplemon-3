#include <Core/Systems/Trainers.hpp>

#include <BLIB/Audio/AudioSystem.hpp>
#include <Core/Files/GameSave.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
constexpr float HoldTime   = 0.75f;
constexpr float RevealRate = 60.f;

std::string trainerKey(const component::Trainer& t) { return t.file() + ":" + t.name(); }
} // namespace

Trainers::Trainers(Systems& o)
: owner(o)
, state(State::Searching)
, walkingTrainer(bl::ecs::InvalidEntity) {
    txtr = TextureManager::load(Properties::TrainerExclaimImage());
    exclaim.setTexture(*txtr, true);
    exclaimSound = bl::audio::AudioSystem::getOrLoadSound(Properties::TrainerExclaimSound());
}

void Trainers::init() { bl::event::Dispatcher::subscribe(this); }

void Trainers::update(float dt) {
    static const sf::Vector2i es(txtr->getSize());
    static const auto updateRect = [this](int h) {
        exclaim.setTextureRect({0, es.y - h, es.x, h});
    };

    switch (state) {
    case State::PoppingUp:
        height += dt * RevealRate;
        if (height >= txtr->getSize().y) {
            updateRect(txtr->getSize().y);
            state  = State::Holding;
            height = 0.f;
        }
        else { updateRect(height); }
        break;

    case State::Holding:
        height += dt;
        if (height >= HoldTime) {
            height = txtr->getSize().y;
            state  = State::Rising;
        }
        break;

    case State::Rising:
        height -= dt * RevealRate;
        if (height > 0.f) { updateRect(height); }
        else {
            updateRect(0);
            state = State::Walking;
        }
        break;

    case State::Walking:
        if (!trainerMove->moving()) {
            if (bl::tmap::Position::adjacent(*trainerPos, owner.player().position())) {
                if (!owner.interaction().interact(walkingTrainer)) {
                    BL_LOG_ERROR << "Trainer " << walkingTrainer
                                 << " was unable to interact with player, aborting";
                    cleanup();
                }
                else { state = State::Battling; }
            }
            else {
                if (!owner.movement().moveEntity(walkingTrainer, trainerPos->direction, false)) {
                    BL_LOG_ERROR << "Trainer " << walkingTrainer
                                 << " is unable to get to the player, aborting";
                    owner.controllable().setEntityLocked(owner.player().player(), false);
                    cleanup();
                }
            }
        }
        break;

    case State::Searching:
    case State::Battling:
    default:
        break;
    }
}

void Trainers::render(sf::RenderTarget& target) {
    switch (state) {
    case State::PoppingUp:
    case State::Holding:
    case State::Rising:
        target.draw(exclaim);
        break;
    default:
        break;
    }
}

bl::ecs::Entity Trainers::approachingTrainer() const { return walkingTrainer; }

bool Trainers::trainerDefeated(const component::Trainer& t) const {
    return defeated.find(trainerKey(t)) != defeated.end();
}

void Trainers::setDefeated(component::Trainer& t) {
    t.setDefeated();
    defeated.emplace(trainerKey(t));
}

void Trainers::observe(const event::GameSaveInitializing& save) {
    save.gameSave.trainers.defeated = &defeated;
}

void Trainers::observe(const bl::ecs::event::ComponentAdded<component::Trainer>& tc) {
    if (trainerDefeated(tc.component)) {
        component::Trainer& t = const_cast<component::Trainer&>(tc.component);
        t.setDefeated();
    }
}

void Trainers::observe(const event::BattleCompleted& b) {
    if (b.type == battle::Battle::Type::Trainer && b.result.localPlayerWon) {
        setDefeated(*trainerComponent);
    }
    cleanup();
}

void Trainers::observe(const event::EntityRotated& rot) {
    if (walkingTrainer == bl::ecs::InvalidEntity) checkTrainer(rot.entity);
}

void Trainers::observe(const event::EntityMoveFinished& moved) {
    if (walkingTrainer != bl::ecs::InvalidEntity) return;

    if (moved.entity == owner.player().player()) {
        for (const bl::ecs::Entity ent : owner.position().updateRangeEntities()) {
            checkTrainer(ent);
        }
    }
    else { checkTrainer(moved.entity); }
}

void Trainers::checkTrainer(bl::ecs::Entity ent) {
    component::Trainer* trainer = owner.engine().ecs().getComponent<component::Trainer>(ent);
    if (!trainer || trainer->defeated()) return;
    if (owner.flight().flying()) return;
    // TODO - respect player/trainer locks?
    const bl::tmap::Position* pos = owner.engine().ecs().getComponent<bl::tmap::Position>(ent);
    if (!pos) {
        BL_LOG_ERROR << "Encountered trainer " << ent << " with no position";
        return;
    }
    component::Movable* move = owner.engine().ecs().getComponent<component::Movable>(ent);
    if (!move) {
        BL_LOG_ERROR << "Encountered trainer " << ent << " with no movable";
        return;
    }

    const bl::ecs::Entity see = owner.position().search(*pos, pos->direction, trainer->range());
    if (see == owner.player().player()) {
        BL_LOG_INFO << "Trainer " << ent << "(" << trainer->name() << " | " << trainer->file()
                    << ") sees player";
        owner.ai().removeAi(ent);
        walkingTrainer   = ent;
        trainerComponent = trainer;
        trainerPos       = pos;
        trainerMove      = move;
        owner.controllable().setEntityLocked(owner.player().player(), true);

        bl::audio::AudioSystem::playSound(exclaimSound);
        state  = State::PoppingUp;
        height = 0.f;
        const sf::Vector2i s(txtr->getSize());
        exclaim.setTextureRect({0, s.y, s.x, 0});
        static const float xoff =
            static_cast<float>(Properties::PixelsPerTile()) * 0.5f - static_cast<float>(s.x) * 0.5f;
        /*exclaim.setPosition(trainerPos->positionPixels().x + xoff,
                            trainerPos->positionPixels().y -
                                static_cast<float>(Properties::PixelsPerTile()));*/
        // TODO - BLIB_UPGRADE - update trainer exclaim rendering
    }
}

void Trainers::cleanup() {
    state            = State::Searching;
    walkingTrainer   = bl::ecs::InvalidEntity;
    trainerComponent = nullptr;
    trainerMove      = nullptr;
}

void Trainers::resetDefeated() { defeated.clear(); }

} // namespace system
} // namespace core
