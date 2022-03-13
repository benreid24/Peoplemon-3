#include <Core/Systems/Trainers.hpp>

#include <Core/Files/GameSave.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Trainers::Trainers(Systems& o)
: owner(o)
, walkingTrainer(bl::entity::InvalidEntity) {}

void Trainers::init() { owner.engine().eventBus().subscribe(this); }

void Trainers::update() {
    if (walkingTrainer != bl::entity::InvalidEntity) {
        if (!trainerMove->moving()) {
            if (component::Position::adjacent(*trainerPos, owner.player().position())) {
                if (!owner.interaction().interact(walkingTrainer)) {
                    BL_LOG_ERROR << "Trainer " << walkingTrainer
                                 << " was unable to interact with player, aborting";
                    cleanup();
                }
                else {
                    // prevent interact spam
                    walkingTrainer = bl::entity::InvalidEntity;
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
    BL_LOG_INFO << "entity rotated";
    if (walkingTrainer == bl::entity::InvalidEntity) checkTrainer(rot.entity);
}

void Trainers::observe(const event::EntityMoveFinished& moved) {
    BL_LOG_INFO << "Entity moved";
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
    if (!trainer) return;
    BL_LOG_INFO << "Checking trainer: " << trainer->name();
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
        // TODO - play sound and add visual indicator to trainer
    }
}

void Trainers::cleanup() {
    walkingTrainer   = bl::entity::InvalidEntity;
    trainerComponent = nullptr;
    trainerMove      = nullptr;
}

} // namespace system
} // namespace core
