#ifndef CORE_SYSTEMS_POSITION_HPP
#define CORE_SYSTEMS_POSITION_HPP

#include <Core/Components/Position.hpp>
#include <Core/Events/EntityMoved.hpp>
#include <Core/Events/Maps.hpp>

#include <BLIB/Containers/Grid.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Events.hpp>
#include <unordered_map>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief The primary entity positioning system. Handles determining which entities get updated as
 *        well as tracking where entities are for collisions
 *
 * @ingroup Systems
 *
 */
class Position
: public bl::event::Listener<
      event::EntityMoved, bl::ecs::event::ComponentAdded<component::Position>,
      bl::ecs::event::ComponentRemoved<component::Position>, event::MapSwitch> {
public:
    /**
     * @brief Construct the Position system
     *
     * @param owner The primary Systems object
     */
    Position(Systems& owner);

    /**
     * @brief Destroy the Position system
     *
     */
    virtual ~Position() = default;

    /**
     * @brief Creates the view on the entity registry
     *
     */
    void init();

    /**
     * @brief Update the group of entities that updates should be performed on based on the current
     *        camera position. This should be called before any other systems
     *
     */
    void update();

    /**
     * @brief Returns the entity at the given position or InvalidEntity if not found
     *
     * @param pos The position to check
     * @return bl::ecs::Entity The entity at the given position or bl::ecs::InvalidEntity
     */
    bl::ecs::Entity getEntity(const component::Position& pos) const;

    /**
     * @brief Returns whether or not a tile is currently occupied
     *
     * @param position The tile position to check
     * @return True if no entity is on the tile, false if an entity is currently there
     */
    bool spaceFree(const component::Position& position) const;

    /**
     * @brief Searches for an entity from the starting position in the given direction for the given
     *        number of spaces. Note that start position should be the position of a searching
     *        entity and is not included in the search results
     *
     * @param start The position to start at. Is not searched
     * @param range Number of spaces to search. Must be greater than 1
     * @return bl::ecs::Entity The entity that was found, or bl::ecs::InvalidEntity if none
     */
    bl::ecs::Entity search(const component::Position& start, component::Direction dir,
                           unsigned int range);

    /**
     * @brief Returns an iterable set containing all the entities that should be updated
     *
     * @return const bl::container::Grid<bl::ecs::Entity>::Range& Entities that should be updated
     */
    const std::vector<bl::ecs::Entity>& updateRangeEntities() const;

    /**
     * @brief Called by the editor when a level is added
     *
     */
    void editorPushLevel();

    /**
     * @brief Called by the editor when a level is removed
     *
     */
    void editorPopLevel();

private:
    Systems& owner;
    std::vector<bl::container::Vector2D<bl::ecs::Entity>> entityMap;
    std::vector<bl::ecs::Entity> toUpdate;

    virtual void observe(const event::EntityMoved& event) override;
    virtual void observe(const bl::ecs::event::ComponentAdded<component::Position>& event) override;
    virtual void observe(
        const bl::ecs::event::ComponentRemoved<component::Position>& event) override;
    virtual void observe(const event::MapSwitch& event) override;

    bl::ecs::Entity& get(const component::Position& pos);
};

} // namespace system
} // namespace core

#endif
