#ifndef CORE_SYSTEMS_RENDER_HPP
#define CORE_SYSTEMS_RENDER_HPP

#include <Core/Components/Renderable.hpp>
#include <Core/Maps/Map.hpp>

#include <BLIB/Entities.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Primary rendering system. Handles fade ins and outs, visual effects, and rendering
 *        everything, including the world and the entities within
 *
 * @ingroup Systems
 *
 */
class Render {
public:
    /**
     * @brief Construct a new Render system
     *
     * @param owner The primary systems object
     */
    Render(Systems& owner);

    /**
     * @brief Initializes the entity registry view
     *
     */
    void init();

    /**
     * @brief Updates all entity animations
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the world and all the entities
     *
     * @param target The target to render to
     * @param map The map to render
     * @param lag Residual time not accounted for in update yet
     */
    void render(sf::RenderTarget& target, const map::Map& map, float lag);

private:
    Systems& owner;
    bl::entity::Registry::View<component::Renderable>::Ptr entities;
};

} // namespace system
} // namespace core

#endif
