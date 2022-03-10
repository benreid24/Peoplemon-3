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

void Trainers::init() {
    trainers = owner.engine()
                   .entities()
                   .getEntitiesWithComponents<component::Trainer,
                                              component::Position,
                                              component::Movable>();
    owner.engine().eventBus().subscribe(this);
}

void Trainers::update() {
    if (walkingTrainer == bl::entity::InvalidEntity) {
        // TODO - search vision for player
    }
    else {
        // TODO - approach player
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
        auto it = trainers->results().find(walkingTrainer);
        if (it != trainers->results().end()) {
            auto& t = *it->second.get<component::Trainer>();
            t.setDefeated();
            defeated.emplace(t.file());
        }
    }
    walkingTrainer = bl::entity::InvalidEntity;
}

} // namespace system
} // namespace core
