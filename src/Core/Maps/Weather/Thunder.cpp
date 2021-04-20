#include "Thunder.hpp"

#include <BLIB/Util/Random.hpp>

namespace core
{
namespace map
{
namespace weather
{
Thunder::Thunder(bool e)
: enabled(e)
, timeSinceLastThunder(0.f) {
    if (e) {
        // TODO - load resources
    }
}

void Thunder::update(float dt) {
    if (enabled) {
        if (lightning.getFillColor().a > 0) {
            // TODO - update a
        }
        else {
            timeSinceLastThunder += dt;
            if (bl::util::Random::get<float>(0.f, 20.f) >= timeSinceLastThunder) {
                // TODO - make thunder
            }
        }
    }
}

void Thunder::render(sf::RenderTarget& target, float) const {
    if (enabled && lightning.getFillColor().a > 0) {
        lightning.setOrigin(target.getView().getSize() / 2.f);
        lightning.setPosition(target.getView().getCenter());
        lightning.setSize(target.getView().getSize());
        target.draw(lightning);
    }
}

} // namespace weather
} // namespace map
} // namespace core
