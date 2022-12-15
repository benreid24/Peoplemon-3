#include <Core/Battles/View/MoveAnimation.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace battle
{
namespace view
{
MoveAnimation::MoveAnimation() {
    foreground.setIsLoop(false);
    background.setIsLoop(false);
    background.setPosition({0.f, 0.f});
    foreground.setPosition({0.f, 0.f});
}

void MoveAnimation::ensureLoaded(const pplmn::BattlePeoplemon& lp,
                                 const pplmn::BattlePeoplemon& op) {
    for (int i = 0; i < 4; ++i) {
        if (lp.base().knownMoves()[i].id != pplmn::MoveId::Unknown) {
            AnimationManager::load(
                pplmn::Move::playerAnimationBackground(lp.base().knownMoves()[i].id));
            AnimationManager::load(
                pplmn::Move::playerAnimationForeground(lp.base().knownMoves()[i].id));
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (op.base().knownMoves()[i].id != pplmn::MoveId::Unknown) {
            AnimationManager::load(
                pplmn::Move::opponentAnimationBackground(op.base().knownMoves()[i].id));
            AnimationManager::load(
                pplmn::Move::opponentAnimationForeground(op.base().knownMoves()[i].id));
        }
    }
}

void MoveAnimation::playAnimation(User user, pplmn::MoveId move) {
    const auto fg = user == User::Player ? pplmn::Move::playerAnimationForeground :
                                           pplmn::Move::opponentAnimationForeground;
    const auto bg = user == User::Player ? pplmn::Move::playerAnimationBackground :
                                           pplmn::Move::opponentAnimationBackground;
    fgSrc         = AnimationManager::load(fg(move));
    bgSrc         = AnimationManager::load(bg(move));
    if (fgSrc && bgSrc) {
        foreground.setData(*fgSrc);
        background.setData(*bgSrc);
        foreground.play(true);
        background.play(true);
    }
    else {
        BL_LOG_ERROR << "Missing animation for move " << move;
        foreground.stop();
        background.stop();
    }
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
