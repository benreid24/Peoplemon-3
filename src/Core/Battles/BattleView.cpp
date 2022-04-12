#include <Core/Battles/BattleView.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView(BattleState& s, bool canRun)
: battleState(s)
, state(State::Done)
, playerMenu(canRun)
, localPeoplemon(view::PeoplemonAnimation::Player)
, opponentPeoplemon(view::PeoplemonAnimation::Opponent) {
    bgndTxtr =
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/battleBgnd.png"))
            .data;
    background.setTexture(*bgndTxtr, true);
}

void BattleView::configureView(const sf::View& pv) {
    localPeoplemon.configureView(pv);
    opponentPeoplemon.configureView(pv);

    // first time init of view components
    statBoxes.setPlayer(&battleState.localPlayer().activePeoplemon());
    statBoxes.setOpponent(&battleState.enemy().activePeoplemon());
    statBoxes.sync();
    localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
    opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
    moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                               battleState.enemy().activePeoplemon());
}

bool BattleView::actionsCompleted() const { return state == State::Done; }

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
        if (printer.messageDone()) {
            state = State::Done;
            BL_LOG_INFO << "message done";
        }
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

    target.draw(background);
    moveAnimation.renderBackground(target, lag);
    localPeoplemon.render(target, lag);
    opponentPeoplemon.render(target, lag);
    moveAnimation.renderForeground(target, lag);
    if (battleState.currentStage() == BattleState::Stage::WaitingChoices && !playerMenu.ready()) {
        playerMenu.render(target);
    }
    printer.render(target);
    statBoxes.render(target);
}

void BattleView::process(component::Command cmd) {
    if ((battleState.currentStage() == BattleState::Stage::FaintSwitching ||
         battleState.currentStage() == BattleState::Stage::WaitingChoices) &&
        !playerMenu.ready()) {
        playerMenu.handleInput(cmd);
    }
    else {
        if (cmd == component::Command::Interact || cmd == component::Command::Back) {
            printer.finishPrint();
        }
    }
}

bool BattleView::processQueue() {
    if (commandQueue.empty()) return false;

    const bool currentIsPlayer = &battleState.activeBattler() == &battleState.localPlayer();
    const Command& cmd         = commandQueue.front();

    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
        printer.setMessage(battleState, cmd.getMessage());
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

        case Animation::Type::PlayerFirstSendout:
            localPeoplemon.triggerAnimation(Animation::Type::SendOut);
            state = State::WaitingPeoplemon;
            break;

        case Animation::Type::OpponentFirstSendout:
            opponentPeoplemon.triggerAnimation(Animation::Type::SendOut);
            state = State::WaitingPeoplemon;
            break;

        default:
            BL_LOG_ERROR << "Unknown animation type: " << cmd.getAnimation().type;
            commandQueue.pop();
            return true;
        }
    } break;

    case Command::Type::SyncStateNoSwitch:
        statBoxes.sync();
        state = State::WaitingBars;
        break;

    case Command::Type::SyncStatePlayerSwitch:
        statBoxes.setPlayer(&battleState.localPlayer().activePeoplemon());
        statBoxes.sync();
        localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
        moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                                   battleState.enemy().activePeoplemon());
        state = State::Done; // we do not wait for bars. anim starts in hidden state
        break;

    case Command::Type::SyncStateOpponentSwitch:
        statBoxes.setOpponent(&battleState.enemy().activePeoplemon());
        statBoxes.sync();
        opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
        moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                                   battleState.enemy().activePeoplemon());
        state = State::Done; // we do not wait for bars. anim starts in hidden state
        break;

    default:
        BL_LOG_ERROR << "Unknown command type: " << cmd.getType();
        commandQueue.pop();
        return true;
    }

    const bool wait = cmd.waitForView();
    commandQueue.pop();
    return !wait;
}

} // namespace battle
} // namespace core
