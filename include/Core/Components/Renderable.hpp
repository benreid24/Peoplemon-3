#ifndef CORE_COMPONENTS_RENDERABLE_HPP
#define CORE_COMPONENTS_RENDERABLE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Graphics/Animation2D.hpp>
#include <BLIB/Graphics/Slideshow.hpp>
#include <BLIB/Graphics/Sprite.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Tilemap/Position.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Resources/RunWalkAnimations.hpp>
#include <Core/Resources/WalkAnimations.hpp>

namespace core
{
namespace system
{
class Render;
}

namespace component
{
/**
 * @brief Adding this component to an entity will allow it to be rendered
 *
 * @ingroup Components
 */
class Renderable {
public:
    /**
     * @brief Does nothing
     */
    Renderable();

    /**
     * @brief Creates a renderable component for a static sprite
     *
     * @param engine The game engine instance
     * @param entity The entity to construct the component on
     * @param scene The scene to add to
     * @param path The path to the sprite
     * @return Renderable A usable component
     */
    static Renderable& createFromSprite(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                        bl::rc::Scene* scene, const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations
     *
     * @param engine The game engine instance
     * @param entity The entity to construct the component on
     * @param scene The scene to add to
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable& createFromMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                           bl::rc::Scene* scene, const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations with running
     *
     * @param engine The game engine instance
     * @param entity The entity to construct the component on
     * @param scene The scene to add to
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable& createFromFastMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                               bl::rc::Scene* scene, const std::string& path);

    /**
     * @brief Creates a renderable component from a single animation
     *
     * @param engine The game engine instance
     @param scene The scene to add to
     * @param entity The entity to construct the component on
     * @param path The path of the animation
     * @return Renderable The created component
     */
    static Renderable& createFromAnimation(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                           bl::rc::Scene* scene, const std::string& path);

    /**
     * @brief Returns the length of the contained animation, or 0.f if no animation
     */
    float animLength() const;

    /**
     * @brief Triggers the current animation if any
     *
     * @param loop True to loop, false to play once
     */
    void triggerAnim(bool loop);

    /**
     * @brief Call when the entity starts or stops moving or changes direction
     *
     * @param dir The direction the entity is facing
     * @param moving Whether or not the entity is moving
     * @param running Whether or not the entity is running
     */
    void notifyMoveState(bl::tmap::Direction dir, bool moving, bool running);

    /**
     * @brief Sets the angle to render the entity at
     *
     * @param angle Rotation in degrees
     */
    void setAngle(float angle);

    /**
     * @brief Returns the render transform
     */
    bl::com::Transform2D& getTransform() { return *transform; }

    /**
     * @brief Set whether the entity is hidden or not
     *
     * @param hide True to hide, false to render
     */
    void setHidden(bool hide);

private:
    enum SourceType { Walk, Run, SingleAnim, Sprite } srcType;
    union {
        res::WalkAnimations* walkSrc;
        res::RunWalkAnimations* runSrc;
        bl::gfx::a2d::AnimationData* animSrc;
    };
    bl::rc::rcom::DrawableBase* drawable;
    bl::com::Transform2D* transform;
    bl::com::Animation2DPlayer* player;
    bl::ecs::Entity shadow;
    bool isMoving;

    friend class system::Render;
};

} // namespace component
} // namespace core

#endif
