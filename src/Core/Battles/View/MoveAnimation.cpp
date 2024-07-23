#include <Core/Battles/View/MoveAnimation.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
std::string getMoveForeground(MoveAnimation::User user, pplmn::MoveId mid) {
    return user == MoveAnimation::User::Player ? pplmn::Move::playerAnimationForeground(mid) :
                                                 pplmn::Move::opponentAnimationForeground(mid);
}

std::string getMoveBackground(MoveAnimation::User user, pplmn::MoveId mid) {
    return user == MoveAnimation::User::Player ? pplmn::Move::playerAnimationBackground(mid) :
                                                 pplmn::Move::opponentAnimationBackground(mid);
}
} // namespace

MoveAnimation::MoveAnimation(bl::engine::Engine& engine)
: engine(engine)
, scene(nullptr)
, playing(nullptr) {}

void MoveAnimation::init(bl::rc::scene::CodeScene* s) { scene = s; }

void MoveAnimation::ensureLoaded(const pplmn::BattlePeoplemon& lp,
                                 const pplmn::BattlePeoplemon& op) {
    for (int i = 0; i < 4; ++i) {
        const auto mid = lp.base().knownMoves()[i].id;
        if (mid != pplmn::MoveId::Unknown && mid != playerAnims[i].move) {
            playerAnims[i].init(engine, scene, User::Player, mid);
        }
    }

    for (int i = 0; i < 4; ++i) {
        const auto mid = op.base().knownMoves()[i].id;
        if (mid != pplmn::MoveId::Unknown && mid != opponentAnims->move) {
            opponentAnims[i].init(engine, scene, User::Opponent, mid);
        }
    }
}

void MoveAnimation::playAnimation(User user, pplmn::MoveId move) {
    playing        = nullptr;
    auto& toSearch = user == User::Player ? playerAnims : opponentAnims;
    for (unsigned int i = 0; i < 4; ++i) {
        if (toSearch[i].move == move && toSearch[i].valid) {
            playing = &toSearch[i];
            break;
        }
    }
    if (!playing) {
        if (extraMove.init(engine, scene, user, move)) { playing = &extraMove; }
        else {
            BL_LOG_ERROR << "Missing animation for move " << move;
            return;
        }
    }

    playing->play();
}

bool MoveAnimation::completed() const { return !playing || playing->finished(); }

void MoveAnimation::renderBackground(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (playing) {
        if (playing->background.getPlayer().playing()) { playing->background.draw(ctx); }
    }
}

void MoveAnimation::renderForeground(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (playing) {
        if (playing->foreground.getPlayer().playing()) { playing->foreground.draw(ctx); }
    }
}

MoveAnimation::Anim::Anim()
: move(pplmn::MoveId::Unknown)
, valid(false) {}

bool MoveAnimation::Anim::init(bl::engine::Engine& engine, bl::rc::scene::CodeScene* scene,
                               User user, pplmn::MoveId mid) {
    move = mid;

    auto bgSrc = AnimationManager::load(getMoveBackground(user, mid));
    auto fgSrc = AnimationManager::load(getMoveForeground(user, mid));
    if (bgSrc->frameCount() == 0 || fgSrc->frameCount() == 0) {
        valid = false;
        return false;
    }

    background.createWithUniquePlayer(engine, bgSrc);
    foreground.createWithUniquePlayer(engine, fgSrc);
    background.addToScene(scene, bl::rc::UpdateSpeed::Static);
    foreground.addToScene(scene, bl::rc::UpdateSpeed::Static);

    valid = true;
    return true;
}

void MoveAnimation::Anim::play() {
    if (valid) {
        background.getPlayer().play(true);
        foreground.getPlayer().play(true);
    }
}

bool MoveAnimation::Anim::finished() const {
    return !valid || (!background.getPlayer().playing() && !foreground.getPlayer().playing());
}

} // namespace view
} // namespace battle
} // namespace core
