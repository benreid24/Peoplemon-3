#include <Core/Battles/BattleView.hpp>

#include <Core/Battles/BattleState.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView(BattleState& s)
: battleState(s)
, state(State::Done)
, localPeoplemon(view::PeoplemonAnimation::Player)
, opponentPeoplemon(view::PeoplemonAnimation::Opponent) {}

bool BattleView::actionsCompleted() const {
    return state == State::Done && commandQueue.empty() && statBoxes.synced() &&
           localPeoplemon.completed() && opponentPeoplemon.completed() &&
           moveAnimation.completed() && printer.messageDone();
}

void BattleView::processCommand(const Command& cmd) { commandQueue.emplace(cmd); }

void BattleView::update(float dt) {
    switch (state) {
    case State::Done:
        while (processQueue()) {}
        break;

    case State::WaitingBars:
        if (statBoxes.synced()) { state = State::Done; }
        break;

    case State::WaitingMove:
        if (moveAnimation.completed()) { state = State::Done; }
        break;

    case State::WaitingPeoplemon:
        if (opponentPeoplemon.completed() && localPeoplemon.completed()) { state = State::Done; }
        break;

    case State::WaitingMessage:
        if (printer.messageDone()) { state = State::Done; }
        break;

    default:
        BL_LOG_ERROR << "Unknown view state: " << state;
        state = State::Done;
        break;
    }

    statBoxes.update(dt);
    printer.update(dt);
    localPeoplemon.update(dt);
    opponentPeoplemon.update(dt);
    moveAnimation.update(dt);
}

void BattleView::render(sf::RenderTarget& target, float lag) const {
    target.clear();

    // TODO - render background

    localPeoplemon.render(target, lag);
    opponentPeoplemon.render(target, lag);
    moveAnimation.render(target, lag);
    if (battleState.currentStage() == BattleState::Stage::WaitingChoices &&
        !playerMenu.subActionSelected()) {
        playerMenu.render(target);
    }
    printer.render(target);
    statBoxes.render(target);
}

void BattleView::process(component::Command) {
    // TODO
}

bool BattleView::processQueue() {
    if (commandQueue.empty()) return false;

    const bool currentIsPlayer = &battleState.activeBattler() == &battleState.localPlayer();
    const Command& cmd         = commandQueue.front();

    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
        printer.setMessage(cmd.getMessage());
        state = State::WaitingMessage;
        break;

    case Command::Type::PlayAnimation: {
        const bool userIsPlayer =
            ((cmd.getAnimation().target == Animation::Target::User && currentIsPlayer) ||
             (cmd.getAnimation().target == Animation::Target::Other && !currentIsPlayer));

        switch (cmd.getAnimation().type) {
        case Animation::Type::ComeBack:
        case Animation::Type::SendOut:
        case Animation::Type::ShakeAndFlash:
        case Animation::Type::SlideDown: {
            auto& anim = userIsPlayer ? localPeoplemon : opponentPeoplemon;
            anim.triggerAnimation(cmd.getAnimation().type);
            state = State::WaitingPeoplemon;
        } break;

        case Animation::Type::UseMove:
            moveAnimation.playAnimation(userIsPlayer ? view::MoveAnimation::Player :
                                                       view::MoveAnimation::Opponent,
                                        cmd.getAnimation().move);
            state = State::WaitingMove;
            break;

        default:
            BL_LOG_ERROR << "Unknown animation type: " << cmd.getAnimation().type;
            commandQueue.pop();
            return true;
        }
    } break;

    case Command::Type::SyncState:
        statBoxes.sync();
        localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
        opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
        moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                                   battleState.enemy().activePeoplemon());
        state = State::WaitingBars;
        break;

    default:
        BL_LOG_ERROR << "Unknown command type: " << cmd.getType();
        commandQueue.pop();
        return true;
    }

    const bool wait = cmd.waitForView();
    if (!wait) { commandQueue.pop(); }
    return !wait;
}

} // namespace battle
} // namespace core
