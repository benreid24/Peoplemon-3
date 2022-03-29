#include <Core/Battles/View/PeoplemonAnimation.hpp>

namespace core
{
namespace battle
{
namespace view
{
PeoplemonAnimation::PeoplemonAnimation(Position pos)
: position(pos) {
    // TODO - init
}

void PeoplemonAnimation::setPeoplemon(pplmn::Id) {
    // TODO - load and set
}

void PeoplemonAnimation::triggerAnimation(Animation::Type) {
    // TODO
}

bool PeoplemonAnimation::completed() const {
    // TODO
    return false;
}

void PeoplemonAnimation::update(float) {
    // TODO
}

void PeoplemonAnimation::render(sf::RenderTarget&, float) const {
    // TODO
}

} // namespace view
} // namespace battle
} // namespace core