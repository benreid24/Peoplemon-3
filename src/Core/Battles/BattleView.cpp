#include <Core/Battles/BattleView.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView(BattleState& s, bool canRun, bl::event::Dispatcher& eb)
: battleState(s)
, playerMenu(canRun, eb)
, localPeoplemon(view::PeoplemonAnimation::Player)
, opponentPeoplemon(view::PeoplemonAnimation::Opponent)
, inited(false) {
    bgndTxtr =
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/battleBgnd.png"))
            .data;
    background.setTexture(*bgndTxtr, true);
}

void BattleView::configureView(const sf::View& pv) {
    // first time init of view components
    if (!inited) {
        inited = true;
        localPeoplemon.configureView(pv);
        opponentPeoplemon.configureView(pv);

        playerMenu.setPeoplemon(battleState.localPlayer().outNowIndex(),
                                battleState.localPlayer().activePeoplemon());
        localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
        opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
        moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                                   battleState.enemy().activePeoplemon());
    }
}

bool BattleView::actionsCompleted() const {
    return statBoxes.synced() && moveAnimation.completed() && localPeoplemon.completed() &&
           opponentPeoplemon.completed() && printer.messageDone() && playerMenu.ready();
}

void BattleView::hideText() { printer.hide(); }

void BattleView::processCommand(const Command& cmd) {
    using Animation = cmd::Animation;

    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
        printer.setMessage(battleState, cmd.getMessage());
        break;

    case Command::Type::PlayAnimation: {
        const bool userIsPlayer =
            cmd.getAnimation().forHostBattler() == battleState.localPlayer().isHost();

        switch (cmd.getAnimation().getType()) {
        case Animation::Type::UseMove:
            moveAnimation.playAnimation(userIsPlayer ? view::MoveAnimation::Player :
                                                       view::MoveAnimation::Opponent,
                                        cmd.getAnimation().getMove());
            break;

        case Animation::Type::PlayerFirstSendout:
            localPeoplemon.triggerAnimation(Animation::Type::SendOut);
            break;

        case Animation::Type::OpponentFirstSendout:
            opponentPeoplemon.triggerAnimation(Animation::Type::SendOut);
            break;

        default: {
            auto& anim = userIsPlayer ? localPeoplemon : opponentPeoplemon;
            anim.triggerAnimation(cmd.getAnimation());
        } break;
        }
    } break;

    case Command::Type::SyncStateNoSwitch:
        statBoxes.sync();
        break;

    case Command::Type::SyncStateSwitch: {
        const bool isPlayer = cmd.forHost() == battleState.localPlayer().isHost();

        if (isPlayer) {
            statBoxes.setPlayer(&battleState.localPlayer().activePeoplemon());
            playerMenu.setPeoplemon(battleState.localPlayer().outNowIndex(),
                                    battleState.localPlayer().activePeoplemon());
            localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
        }
        else {
            statBoxes.setOpponent(&battleState.enemy().activePeoplemon());
            opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
        }
        moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                                   battleState.enemy().activePeoplemon());
    } break;

    default:
        BL_LOG_ERROR << "Unknown command type: " << cmd.getType();
        break;
    }
}

view::PlayerMenu& BattleView::menu() { return playerMenu; }

bool BattleView::playerChoseForgetMove() const { return printer.choseToForget(); }

bool BattleView::playerChoseToSetName() const { return printer.choseToSetName(); }

const std::string& BattleView::chosenNickname() const { return printer.chosenNickname(); }

void BattleView::update(float dt) {
    statBoxes.update(dt);
    printer.update(dt);
    localPeoplemon.update(dt);
    opponentPeoplemon.update(dt);
    moveAnimation.update(dt);
    playerMenu.refresh();
}

void BattleView::render(sf::RenderTarget& target, float lag) const {
    target.clear();

    target.draw(background);
    statBoxes.render(target);
    moveAnimation.renderBackground(target, lag);
    localPeoplemon.render(target, lag);
    opponentPeoplemon.render(target, lag);
    moveAnimation.renderForeground(target, lag);
    if ((battleState.currentStage() == BattleState::Stage::WaitingChoices ||
         battleState.currentStage() == BattleState::Stage::WaitingForgetMoveChoice) &&
        !playerMenu.ready()) {
        playerMenu.render(target);
    }
    printer.render(target);
}

bool BattleView::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                         bool eventTriggered) {
    if ((battleState.currentStage() == BattleState::Stage::WaitingForgetMoveChoice ||
         battleState.currentStage() == BattleState::Stage::WaitingChoices) &&
        !playerMenu.ready()) {
        playerMenu.handleInput(ctrl, eventTriggered);
    }
    else {
        printer.process(ctrl, eventTriggered);
    }
    return true;
}

} // namespace battle
} // namespace core
