#ifndef CORE_SYSTEMS_TRAINERS_HPP
#define CORE_SYSTEMS_TRAINERS_HPP

#include <BLIB/Audio/AudioSystem.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Tilemap/Position.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Battle.hpp>
#include <Core/Events/EntityMoved.hpp>
#include <Core/Events/GameSave.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief This system manages trainers. It handles their vision and makes them walk up to the player
 *
 * @ingroup Systems
 */
class Trainers
: public bl::engine::System
, bl::event::Listener<event::GameSaveInitializing,
                      bl::ecs::event::ComponentAdded<component::Trainer>, event::BattleCompleted,
                      event::EntityMoveFinished, event::EntityRotated> {
public:
    /**
     * @brief Construct a new Trainers system
     *
     * @param owner The main game systems
     */
    Trainers(Systems& owner);

    /**
     * @brief Destroys the system
     */
    virtual ~Trainers() = default;

    /**
     * @brief Returns the trainer currently approaching the player
     *
     * @return bl::ecs::Entity
     */
    bl::ecs::Entity approachingTrainer() const;

    /**
     * @brief Resets the list of defeated trainers
     */
    void resetDefeated();

    /**
     * @brief Checks whether or not the given trainer has been defeated
     *
     * @param trainer The trainer to test
     * @return True if defeated, false if not
     */
    bool trainerDefeated(const component::Trainer& trainer) const;

    /**
     * @brief Sets the given trainer as having been defeated and updates the game save data
     *
     * @param trainer The trainer to mark defeated
     */
    void setDefeated(component::Trainer& trainer);

private:
    enum struct State { Searching, PoppingUp, Holding, Rising, Walking, Battling };

    Systems& owner;
    bl::rc::res::TextureRef exclaimTxtr;
    bl::gfx::Sprite exclaim;
    bl::audio::AudioSystem::Handle exclaimSound;
    float height;

    State state;
    bl::ecs::Entity walkingTrainer;
    component::Trainer* trainerComponent;
    const bl::tmap::Position* trainerPos;
    component::Movable* trainerMove;

    std::unordered_set<std::string> defeated;

    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;
    virtual void init(bl::engine::Engine&) override;

    virtual void observe(const event::GameSaveInitializing& save) override;
    virtual void observe(const bl::ecs::event::ComponentAdded<component::Trainer>& tc) override;
    virtual void observe(const event::BattleCompleted& event) override;
    virtual void observe(const event::EntityMoveFinished& moved) override;
    virtual void observe(const event::EntityRotated& rotated) override;

    void checkTrainer(bl::ecs::Entity ent);
    void cleanup();
};

} // namespace system
} // namespace core

#endif
