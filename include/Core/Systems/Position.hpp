#ifndef CORE_SYSTEMS_POSITION_HPP
#define CORE_SYSTEMS_POSITION_HPP

#include <Core/Components/Position.hpp>
#include <Core/Events/EntityMoved.hpp>
#include <Core/Events/Maps.hpp>

#include <BLIB/Containers/Grid.hpp>
#include <BLIB/Entities.hpp>
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
: public bl::event::Listener<bl::entity::event::EntityDestroyed, event::EntityMoved,
                             bl::entity::event::ComponentAdded<component::Position>,
                             bl::entity::event::ComponentRemoved<component::Position>,
                             event::MapSwitch> {
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
     * @return bl::entity::Entity The entity that was found, or bl::entity::InvalidEntity if none
     */
    bl::entity::Entity search(const component::Position& start, component::Direction dir,
                              unsigned int range);

    /**
     * @brief Returns an iterable set containing all the entities that should be updated
     *
     * @return const bl::container::Grid<bl::entity::Entity>::Range& Entities that should be updated
     */
    const std::vector<bl::entity::Entity>& updateRangeEntities() const;

private:
    Systems& owner;
    bl::entity::Registry::View<component::Position>::Ptr entities;
    std::vector<bl::container::Vector2D<bl::entity::Entity>> entityMap;
    std::vector<bl::entity::Entity> toUpdate;

    virtual void observe(const event::EntityMoved& event) override;

    virtual void observe(const bl::entity::event::EntityDestroyed& event) override;

    virtual void observe(
        const bl::entity::event::ComponentAdded<component::Position>& event) override;

    virtual void observe(
        const bl::entity::event::ComponentRemoved<component::Position>& event) override;

    virtual void observe(const event::MapSwitch& event) override;

    bl::entity::Entity& get(const component::Position& pos);
    const bl::entity::Entity& get(const component::Position& pos) const;
};

} // namespace system
} // namespace core

#endif
