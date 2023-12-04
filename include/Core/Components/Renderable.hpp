#ifndef CORE_COMPONENTS_RENDERABLE_HPP
#define CORE_COMPONENTS_RENDERABLE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Graphics/Animation2D.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Position.hpp>

namespace core
{
namespace component
{
/**
 * @brief Adding this component to an entity will allow it to be rendered
 *
 * @ingroup Components
 */
class Renderable { // TODO - BLIB_UPGRADE - update entity rendering and remove this
public:
    /**
     * @brief Creates a renderable component for a static sprite
     *
     * @param pos The position component of the owner
     * @param path The path to the sprite
     * @return Renderable A usable component
     */
    static Renderable fromSprite(const Position& pos, const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations
     *
     * @param pos The position component of the owner
     * @param movable The movable component of the owner
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable fromMoveAnims(const Position& pos, component::Movable& movable,
                                    const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations with running
     *
     * @param pos The position component of the owner
     * @param movable The movable component of the owner
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable fromFastMoveAnims(const Position& pos, component::Movable& move,
                                        const std::string& path);

    /**
     * @brief Creates a renderable component from a single animation
     *
     * @param pos The position component to render at
     * @param path The path of the animation
     * @return Renderable The created component
     */
    static Renderable fromAnimation(const Position& pos, const std::string& path);

    /**
     * @brief Updates contained animations
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Returns the length of the contained animation, or 0.f if no animation
     *
     */
    float animLength() const;

    /**
     * @brief Triggers the current animation if any
     *
     * @param loop True to loop, false to play once
     *
     */
    void triggerAnim(bool loop);

    /**
     * @brief Renders the entity to the given target
     *
     * @param target The target to render to
     * @param lag Time elapsed in seconds not accounted for in update
     */
    void render(sf::RenderTarget& target, float lag) const;

    /**
     * @brief Sets the angle to render the entity at
     *
     * @param angle Rotation in degrees
     */
    void setAngle(float angle);

    /**
     * @brief Adds or updates the shadow of this renderable
     *
     * @param distance How far below the renderable to render the shadow
     * @param radius The radius of the shadow in pixels
     */
    void updateShadow(float distance, float radius);

    /**
     * @brief Removes the shadow from this renderable if present
     *
     */
    void removeShadow();

private:
    struct Base {
        virtual ~Base() = default;

        virtual void update(float dt, const Position& pos)                                = 0;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) = 0;
        virtual float length() const                                                      = 0;
        virtual void trigger(bool loop)                                                   = 0;
        virtual void setAngle(float angle)                                                = 0;
    };

    struct StaticSprite : public Base {
        bl::resource::Ref<sf::Texture> texture;
        sf::Sprite sprite;

        virtual ~StaticSprite() = default;
        virtual void update(float, const Position&) override {}
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
        virtual void setAngle(float angle) override;
    };

    struct MoveAnims : public Base {
        bl::resource::Ref<bl::gfx::a2d::AnimationData> data[4];
        // bl::gfx::Animation anim;
        component::Movable& movable;

        MoveAnims(component::Movable& movable);
        virtual ~MoveAnims() = default;
        virtual void update(float dt, const Position& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
        virtual void setAngle(float angle) override;
    };

    struct FastMoveAnims : public Base {
        bl::resource::Ref<bl::gfx::a2d::AnimationData> walk[4];
        bl::resource::Ref<bl::gfx::a2d::AnimationData> run[4];
        // bl::gfx::Animation anim;
        component::Movable& movable;

        FastMoveAnims(component::Movable& movable);
        virtual ~FastMoveAnims() = default;
        virtual void update(float dt, const Position& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
        virtual void setAngle(float angle) override;
    };

    struct OneAnimation : public Base {
        bl::resource::Ref<bl::gfx::a2d::AnimationData> src;
        // bl::gfx::Animation anim;
        sf::Vector2f offset;

        OneAnimation(const std::string& path);
        virtual ~OneAnimation() = default;
        virtual void update(float dt, const Position& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
        virtual void setAngle(float angle) override;
    };

    const component::Position& position;
    std::variant<StaticSprite, MoveAnims, FastMoveAnims, OneAnimation> data;
    sf::CircleShape shadow;
    float shadowHeight;

    Base* cur();
    Base* cur() const;

    Renderable(const Position& pos);
};

} // namespace component
} // namespace core

#endif
