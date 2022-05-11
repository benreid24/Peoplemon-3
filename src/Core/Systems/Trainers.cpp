#include <Core/Systems/Trainers.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Media/Audio/AudioSystem.hpp>
#include <Core/Files/GameSave.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
constexpr float HoldTime   = 0.75f;
constexpr float RevealRate = 60.f;
constexpr float HideRate   = RevealRate * 2.f;
} // namespace

Trainers::Trainers(Systems& o)
: owner(o)
, state(State::Searching)
, walkingTrainer(bl::entity::InvalidEntity) {
    txtr = bl::engine::Resources::textures().load(Properties::TrainerExclaimImage()).data;
    exclaim.setTexture(*txtr, true);
}

void Trainers::init() { owner.engine().eventBus().subscribe(this); }

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
        else {
            updateRect(height);
        }
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
            if (component::Position::adjacent(*trainerPos, owner.player().position())) {
                if (!owner.interaction().interact(walkingTrainer)) {
                    BL_LOG_ERROR << "Trainer " << walkingTrainer
                                 << " was unable to interact with player, aborting";
                    cleanup();
                }
                else {
                    state = State::Battling;
                }
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

bl::entity::Entity Trainers::approachingTrainer() const { return walkingTrainer; }

void Trainers::observe(const event::GameSaveInitializing& save) {
    save.gameSave.trainers.defeated = &defeated;
}

void Trainers::observe(const bl::entity::event::ComponentAdded<component::Trainer>& tc) {
    if (defeated.find(tc.component.file()) != defeated.end()) {
        component::Trainer& t = const_cast<component::Trainer&>(tc.component);
        t.setDefeated();
    }
}

void Trainers::observe(const event::BattleCompleted& b) {
    if (b.playerWon) {
        trainerComponent->setDefeated();
        defeated.emplace(trainerComponent->file());
    }
    cleanup();
}

void Trainers::observe(const event::EntityRotated& rot) {
    if (walkingTrainer == bl::entity::InvalidEntity) checkTrainer(rot.entity);
}

void Trainers::observe(const event::EntityMoveFinished& moved) {
    if (walkingTrainer != bl::entity::InvalidEntity) return;

    if (moved.entity == owner.player().player()) {
        for (const bl::entity::Entity ent : owner.position().updateRangeEntities()) {
            checkTrainer(ent);
        }
    }
    else {
        checkTrainer(moved.entity);
    }
}

void Trainers::checkTrainer(bl::entity::Entity ent) {
    component::Trainer* trainer = owner.engine().entities().getComponent<component::Trainer>(ent);
    if (!trainer || trainer->defeated()) return;
    const component::Position* pos =
        owner.engine().entities().getComponent<component::Position>(ent);
    if (!pos) {
        BL_LOG_ERROR << "Encountered trainer " << ent << " with no position";
        return;
    }
    component::Movable* move = owner.engine().entities().getComponent<component::Movable>(ent);
    if (!move) {
        BL_LOG_ERROR << "Encountered trainer " << ent << " with no movable";
        return;
    }

    const bl::entity::Entity see = owner.position().search(*pos, pos->direction, trainer->range());
    if (see == owner.player().player()) {
        BL_LOG_INFO << "Trainer " << ent << "(" << trainer->name() << " | " << trainer->file()
                    << ") sees player";
        owner.ai().removeAi(ent);
        walkingTrainer   = ent;
        trainerComponent = trainer;
        trainerPos       = pos;
        trainerMove      = move;
        owner.controllable().setEntityLocked(owner.player().player(), true);

        auto sfx = bl::audio::AudioSystem::getOrLoadSound(Properties::TrainerExclaimSound());
        bl::audio::AudioSystem::playSound(sfx);
        state  = State::PoppingUp;
        height = 0.f;
        const sf::Vector2i s(txtr->getSize());
        exclaim.setTextureRect({0, s.y, s.x, 0});
        static const float xoff =
            static_cast<float>(Properties::PixelsPerTile()) * 0.5f - static_cast<float>(s.x) * 0.5f;
        exclaim.setPosition(trainerPos->positionPixels().x + xoff,
                            trainerPos->positionPixels().y -
                                static_cast<float>(Properties::PixelsPerTile()));
    }
}

void Trainers::cleanup() {
    state            = State::Searching;
    walkingTrainer   = bl::entity::InvalidEntity;
    trainerComponent = nullptr;
    trainerMove      = nullptr;
}

} // namespace system
} // namespace core
