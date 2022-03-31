#include <Core/Battles/View/MoveAnimation.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Peoplemon/Move.hpp>

namespace core
{
namespace battle
{
namespace view
{
MoveAnimation::MoveAnimation() {
    foreground.setIsCentered(false);
    foreground.setIsLoop(false);
    background.setIsCentered(false);
    background.setIsLoop(false);
}

void MoveAnimation::ensureLoaded(const pplmn::BattlePeoplemon& lp,
                                 const pplmn::BattlePeoplemon& op) {
    auto& loader = bl::engine::Resources::animations();

    for (int i = 0; i < 4; ++i) {
        if (lp.base().knownMoves()[i].id != pplmn::MoveId::Unknown) {
            loader.load(pplmn::Move::playerAnimationBackground(lp.base().knownMoves()[i].id));
            loader.load(pplmn::Move::playerAnimationForeground(lp.base().knownMoves()[i].id));
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (op.base().knownMoves()[i].id != pplmn::MoveId::Unknown) {
            loader.load(pplmn::Move::opponentAnimationBackground(op.base().knownMoves()[i].id));
            loader.load(pplmn::Move::opponentAnimationForeground(op.base().knownMoves()[i].id));
        }
    }
}

void MoveAnimation::playAnimation(User user, pplmn::MoveId move) {
    const auto fg = user == User::Player ? pplmn::Move::playerAnimationForeground :
                                           pplmn::Move::opponentAnimationForeground;
    const auto bg = user == User::Player ? pplmn::Move::playerAnimationBackground :
                                           pplmn::Move::opponentAnimationBackground;
    fgSrc         = bl::engine::Resources::animations().load(fg(move)).data;
    bgSrc         = bl::engine::Resources::animations().load(bg(move)).data;
    foreground.setData(*fgSrc);
    background.setData(*bgSrc);
    foreground.play(true);
    background.play(true);
}

bool MoveAnimation::completed() const { return background.finished() && foreground.finished(); }

void MoveAnimation::update(float dt) {
    background.update(dt);
    foreground.update(dt);
}

void MoveAnimation::renderBackground(sf::RenderTarget& target, float lag) const {
    bl::gfx::Animation& anim = const_cast<bl::gfx::Animation&>(background);
    if (!anim.finished()) anim.render(target, lag);
}

void MoveAnimation::renderForeground(sf::RenderTarget& target, float lag) const {
    bl::gfx::Animation& anim = const_cast<bl::gfx::Animation&>(foreground);
    if (!anim.finished()) anim.render(target, lag);
}

} // namespace view
} // namespace battle
} // namespace core