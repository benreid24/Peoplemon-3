#ifndef CORE_SYSTEMS_TRAINERS_HPP
#define CORE_SYSTEMS_TRAINERS_HPP

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Battle.hpp>
#include <Core/Events/GameSave.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief This system manages trainers. It handles their vision and makes them walk up to the player
 *
 * @ingroup Systems
 *
 */
class Trainers
: bl::event::Listener<event::GameSaveInitializing,
                      bl::entity::event::ComponentAdded<component::Trainer>,
                      event::BattleCompleted> {
public:
    /**
     * @brief Construct a new Trainers system
     *
     * @param owner The main game systems
     */
    Trainers(Systems& owner);

    /**
     * @brief Constructs the ECS view of trainers
     *
     */
    void init();

    /**
     * @brief Performs per-frame trainer logic
     *
     */
    void update();

    /**
     * @brief Returns the trainer currently approaching the player
     *
     * @return bl::entity::Entity
     */
    bl::entity::Entity approachingTrainer() const;

private:
    Systems& owner;
    bl::entity::Registry::View<component::Trainer, component::Position, component::Movable>::Ptr
        trainers;
    bl::entity::Entity walkingTrainer;

    std::unordered_set<std::string> defeated;

    virtual void observe(const event::GameSaveInitializing& save) override;
    virtual void observe(const bl::entity::event::ComponentAdded<component::Trainer>& tc) override;
    virtual void observe(const event::BattleCompleted& event) override;
};

} // namespace system
} // namespace core

#endif