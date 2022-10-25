#include <Core/Systems/Render.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Render::Render(Systems& owner)
: owner(owner) {}

void Render::init() {
    entities = owner.engine().entities().getEntitiesWithComponents<component::Renderable>();
}

void Render::update(float dt) {
    for (const bl::entity::Entity e : owner.position().updateRangeEntities()) {
        auto it = entities->results().find(e);
        if (it != entities->results().end()) {
            it->second.get<component::Renderable>()->update(dt);
        }
    }
}

void Render::render(sf::RenderTarget& target, const map::Map& map, float lag) {
    const sf::View oldView = target.getView();

    sf::View view = oldView;
    owner.cameras().configureView(map, view);
    target.setView(view);

    map.render(target,
               lag,
               std::bind(&Render::renderEntities,
                         this,
                         std::ref<sf::RenderTarget>(target),
                         lag,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4));
    ;
    owner.hud().render(target, lag);
    owner.trainers().render(target);

    target.setView(oldView);
}

void Render::renderEntities(sf::RenderTarget& target, float lag, std::uint8_t level,
                            unsigned int row, unsigned int minX, unsigned int maxX) {
    for (unsigned int x = minX; x < maxX; ++x) {
        const bl::entity::Entity e = this->owner.position().getEntity(
            {level, sf::Vector2i(x, row), component::Direction::Up});
        if (e == bl::entity::InvalidEntity) continue;
        auto it = this->entities->results().find(e);
        if (it != this->entities->results().end()) {
            it->second.get<component::Renderable>()->render(target, lag);
        }
    }
}

} // namespace system
} // namespace core
