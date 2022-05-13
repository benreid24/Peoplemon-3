#ifndef CORE_COMPONENTS_RENDERABLE_HPP
#define CORE_COMPONENTS_RENDERABLE_HPP

#include <BLIB/Entities/Registry.hpp>
#include <BLIB/Media/Graphics/Animation.hpp>
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
 *
 */
class Renderable {
public:
    /// Required to be used in the BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 4;

    /**
     * @brief Creates a renderable component for a static sprite
     *
     * @param pos The position component of the owner
     * @param path The path to the sprite
     * @return Renderable A usable component
     */
    static Renderable fromSprite(
        const bl::entity::Registry::ComponentHandle<component::Position>& pos,
        const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations
     *
     * @param pos The position component of the owner
     * @param movable The movable component of the owner
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable fromMoveAnims(
        const bl::entity::Registry::ComponentHandle<component::Position>& pos,
        const bl::entity::Registry::ComponentHandle<component::Movable>& movable,
        const std::string& path);

    /**
     * @brief Creates a renderable component for movement animations with running
     *
     * @param pos The position component of the owner
     * @param movable The movable component of the owner
     * @param path The path to the movement animations
     * @return Renderable A usable component
     */
    static Renderable fromFastMoveAnims(
        const bl::entity::Registry::ComponentHandle<component::Position>& pos,
        const bl::entity::Registry::ComponentHandle<component::Movable>& move,
        const std::string& path);

    /**
     * @brief Creates a renderable component from a single animation
     *
     * @param pos The position component to render at
     * @param path The path of the animation
     * @return Renderable The created component
     */
    static Renderable fromAnimation(
        const bl::entity::Registry::ComponentHandle<component::Position>& pos,
        const std::string& path);

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

private:
    struct Base {
        virtual ~Base() = default;

        virtual void update(
            float dt, const bl::entity::Registry::ComponentHandle<component::Position>& pos) = 0;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos)    = 0;
        virtual float length() const                                                         = 0;
        virtual void trigger(bool loop)                                                      = 0;
    };

    struct StaticSprite : public Base {
        bl::resource::Resource<sf::Texture>::Ref texture;
        sf::Sprite sprite;

        virtual ~StaticSprite() = default;
        virtual void update(
            float, const bl::entity::Registry::ComponentHandle<component::Position>&) override {}
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
    };

    struct MoveAnims : public Base {
        bl::resource::Resource<bl::gfx::AnimationData>::Ref data[4];
        bl::gfx::Animation anim;
        bl::entity::Registry::ComponentHandle<component::Movable> movable;

        MoveAnims(const bl::entity::Registry::ComponentHandle<component::Movable>& movable);
        virtual ~MoveAnims() = default;
        virtual void update(
            float dt,
            const bl::entity::Registry::ComponentHandle<component::Position>& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
    };

    struct FastMoveAnims : public Base {
        bl::resource::Resource<bl::gfx::AnimationData>::Ref walk[4];
        bl::resource::Resource<bl::gfx::AnimationData>::Ref run[4];
        bl::gfx::Animation anim;
        bl::entity::Registry::ComponentHandle<component::Movable> movable;

        FastMoveAnims(const bl::entity::Registry::ComponentHandle<component::Movable>& movable);
        virtual ~FastMoveAnims() = default;
        virtual void update(
            float dt,
            const bl::entity::Registry::ComponentHandle<component::Position>& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
    };

    struct OneAnimation : public Base {
        bl::resource::Resource<bl::gfx::AnimationData>::Ref src;
        bl::gfx::Animation anim;
        sf::Vector2f offset;

        OneAnimation(const std::string& path);
        virtual ~OneAnimation() = default;
        virtual void update(
            float dt,
            const bl::entity::Registry::ComponentHandle<component::Position>& pos) override;
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
        virtual float length() const override;
        virtual void trigger(bool loop) override;
    };

    bl::entity::Registry::ComponentHandle<component::Position> position;
    std::variant<StaticSprite, MoveAnims, FastMoveAnims, OneAnimation> data;

    Base* cur();
    Base* cur() const;

    Renderable(const bl::entity::Registry::ComponentHandle<component::Position>& pos);
};

} // namespace component
} // namespace core

#endif
