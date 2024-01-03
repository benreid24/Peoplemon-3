#ifndef CORE_SYSTEMS_AI_HPP
#define CORE_SYSTEMS_AI_HPP

#include <BLIB/ECS.hpp>
#include <Core/Components/FixedPathBehavior.hpp>
#include <Core/Components/PathFinder.hpp>
#include <Core/Components/SpinBehavior.hpp>
#include <Core/Components/StandingBehavior.hpp>
#include <Core/Components/WanderBehavior.hpp>
#include <Core/Files/Behavior.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Manages NPC's and trainers and their behaviors
 *
 * @ingroup Systems
 *
 */
class AI {
public:
    /**
     * @brief Construct a new AI system
     *
     * @param owner The primary systems object
     */
    AI(Systems& owner);

    /**
     * @brief Creates the entity views
     *
     */
    void init();

    /**
     * @brief Updates all AI
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Helper function for adding the given behavior to the given entity
     *
     * @param entity The entity to add the behavior to
     * @param behavior The behavior to add
     * @return True if the behavior was able to be added, false otherwise
     */
    bool addBehavior(bl::ecs::Entity entity, const file::Behavior& behavior);

    /**
     * @brief Makes the given entity stand in place. Replaces any existing behavior on the entity
     *
     * @param entity The entity to stand in place
     * @param dir The direction to face
     * @return True if the behvaior was able to be added, false otherwise
     */
    bool makeStanding(bl::ecs::Entity entity, bl::tmap::Direction dir);

    /**
     * @brief Makes the given entity spin in place. Replaces any existing behavior
     *
     * @param entity The entity to spin
     * @param dir The direction to spin in
     * @return True if the behavior was able to be added, false otherwise
     */
    bool makeSpinning(bl::ecs::Entity entity, file::Behavior::Spinning::Direction dir);

    /**
     * @brief Make the entity follow a fixed path. Replacces existing behavior
     *
     * @param entity The entity to go on the path
     * @param path The path to follow
     * @return True if the behavior was added, false otherwise
     */
    bool makeFollowPath(bl::ecs::Entity entity, const file::Behavior::Path& path);

    /**
     * @brief Makes the given entity wander around within the given radius. Replaces any existing
     *        behavior the entity may have
     *
     * @param entity The entity to wander around
     * @param radius The radius to stay within, in tiles
     * @return True if the behavior was able to be added, false otherwise
     */
    bool makeWander(bl::ecs::Entity entity, unsigned int radius);

    /**
     * @brief Uses a pathfinder to navigate the entity to the given position and end up facing the
     *        given direction. The existing behavior is locked and left in the locked state when the
     *        path is completed
     *
     * @param entity The entity to navigate
     * @param dest The position to navigate to
     * @return True if the controller was able to be added and the path is valid, false otherwise
     */
    bool moveToPosition(bl::ecs::Entity entity, const bl::tmap::Position& dest);

    /**
     * @brief Removes any ai behavior from the given entity. Does not affect path finding
     *
     * @param ent The entity to remove behavior from
     */
    void removeAi(bl::ecs::Entity ent);

private:
    // type helpers
    using StandingTypes =
        bl::ecs::Require<component::StandingBehavior, bl::tmap::Position, component::Controllable>;
    using SpinTypes =
        bl::ecs::Require<component::SpinBehavior, bl::tmap::Position, component::Controllable>;
    using FixedPathTypes = bl::ecs::Require<component::FixedPathBehavior, bl::tmap::Position,
                                            component::Controllable>;
    using WanderTypes =
        bl::ecs::Require<component::WanderBehavior, bl::tmap::Position, component::Controllable>;

    using StandingView  = bl::ecs::View<StandingTypes>*;
    using StandingRow   = bl::ecs::ComponentSet<StandingTypes>;
    using SpinView      = bl::ecs::View<SpinTypes>*;
    using SpinRow       = bl::ecs::ComponentSet<SpinTypes>;
    using FixedPathView = bl::ecs::View<FixedPathTypes>*;
    using FixedPathRow  = bl::ecs::ComponentSet<FixedPathTypes>;
    using WanderView    = bl::ecs::View<WanderTypes>*;
    using WanderRow     = bl::ecs::ComponentSet<WanderTypes>;

    Systems& owner;
    StandingView standing;
    SpinView spinning;
    FixedPathView paths;
    WanderView wandering;

    bool findPath(bl::ecs::Entity ent, component::PathFinder& path);
};

} // namespace system
} // namespace core

#endif
