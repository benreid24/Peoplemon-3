#ifndef CORE_SYSTEMS_RENDER_HPP
#define CORE_SYSTEMS_RENDER_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <Core/Components/Renderable.hpp>
#include <Core/Events/EntityMoved.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Basic system that synchronizes animation states based on movement state events
 *
 * @ingroup Systems
 */
class Render
: public bl::engine::System
, public bl::event::Listener<event::EntityMoved, event::EntityMoveFinished, event::EntityRotated> {
public:
    /**
     * @brief Creates the render system
     *
     * @param owner The game systems
     */
    Render(Systems& owner);

    /**
     * @brief Destroys the render system
     */
    virtual ~Render() = default;

    /**
     * @brief Adds or updates the shadow of the given entity
     *
     * @param entity The entity to add the shadow to
     * @param distance How far below the renderable to render the shadow
     * @param radius The radius of the shadow in pixels
     */
    void updateShadow(bl::ecs::Entity entity, float distance, float radius);

    /**
     * @brief Removes the shadow from the given entity
     *
     * @param entity The entity to remove the shadow from
     */
    void removeShadow(bl::ecs::Entity entity);

    /**
     * @brief Returns the engine observer that the main game is rendering to
     */
    bl::rc::RenderTarget& getMainRenderTarget() { return *mainRenderTarget; }

    /**
     * @brief Sets the main render target. Used by the editor
     *
     * @param target The render target the world gets rendered to
     */
    void setMainRenderTarget(bl::rc::RenderTarget& target);

private:
    Systems& owner;
    bl::rc::RenderTarget* mainRenderTarget;
    bl::ecs::ComponentPool<component::Renderable>& pool;
    bl::ecs::ComponentPool<bl::com::Transform2D>& transformPool;
    std::vector<std::pair<bl::ecs::Entity, bl::tmap::Direction>> stopRm;
    std::vector<std::pair<bl::ecs::Entity, bl::tmap::Direction>> stopAdd;
    std::vector<bl::ecs::Entity> shadowsToRemove;

    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;
    virtual void init(bl::engine::Engine& engine) override;
    virtual void notifyFrameStart() override;
    virtual void observe(const event::EntityMoved& event) override;
    virtual void observe(const event::EntityMoveFinished& event) override;
    virtual void observe(const event::EntityRotated& event) override;
};

} // namespace system
} // namespace core

#endif
