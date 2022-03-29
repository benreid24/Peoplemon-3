#include <Core/Battles/View/MoveAnimation.hpp>

#include <Core/Peoplemon/Move.hpp>

namespace core
{
namespace battle
{
namespace view
{
MoveAnimation::MoveAnimation() {
    // anything here?
}

void MoveAnimation::ensureLoaded(const pplmn::BattlePeoplemon&, const pplmn::BattlePeoplemon&) {
    // TODO - add animation files to move db or gen from id
}

void MoveAnimation::playAnimation(User, pplmn::MoveId) {
    // TODO
}

bool MoveAnimation::completed() const { return playing.finished(); }

void MoveAnimation::update(float dt) { playing.update(dt); }

void MoveAnimation::render(sf::RenderTarget& target, float lag) const {
    bl::gfx::Animation& anim = const_cast<bl::gfx::Animation&>(playing);
    if (!anim.finished()) anim.render(target, lag);
}

} // namespace view
} // namespace battle
} // namespace core