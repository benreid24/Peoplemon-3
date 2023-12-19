#include <Core/Systems/Render.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Render::Render(Systems& owner)
: owner(owner) {}

void Render::update(float dt) {
    for (const bl::ecs::Entity e : owner.position().updateRangeEntities()) {
        component::Renderable* r = owner.engine().ecs().getComponent<component::Renderable>(e);
        if (r != nullptr) { r->update(dt); }
    }
}

void Render::render(sf::RenderTarget& target, const map::Map& map, float lag) {
    owner.trainers().render(target);
}

void Render::renderEntities(sf::RenderTarget& target, float lag, std::uint8_t level,
                            unsigned int row, unsigned int minX, unsigned int maxX) {
    for (unsigned int x = minX; x < maxX; ++x) {
        const bl::ecs::Entity e = this->owner.position().getEntity(
            {level, sf::Vector2i(x, row), component::Direction::Up});
        if (e == bl::ecs::InvalidEntity) continue;
        component::Renderable* r = owner.engine().ecs().getComponent<component::Renderable>(e);
        if (r != nullptr) { r->render(target, lag); }
    }
}

} // namespace system
} // namespace core
