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
     * @brief Updates contained animations
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

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
    };

    struct StaticSprite : public Base {
        bl::resource::Resource<sf::Texture>::Ref texture;
        sf::Sprite sprite;

        virtual ~StaticSprite() = default;
        virtual void update(
            float, const bl::entity::Registry::ComponentHandle<component::Position>&) override {}
        virtual void render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) override;
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
    };

    bl::entity::Registry::ComponentHandle<component::Position> position;
    std::shared_ptr<Base> data;

    Renderable(const bl::entity::Registry::ComponentHandle<component::Position>& pos);
};

} // namespace component
} // namespace core

#endif
