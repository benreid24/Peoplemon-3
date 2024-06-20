#include <Core/Battles/BattleView.hpp>

#include <Core/Battles/BattleState.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView(bl::engine::Engine& engine, BattleState& s, bool canRun)
: engine(engine)
, battleState(s)
, playerMenu(engine, canRun)
, statBoxes(engine)
, printer(engine)
, localPeoplemon(engine, view::PeoplemonAnimation::Player)
, opponentPeoplemon(engine, view::PeoplemonAnimation::Opponent)
, moveAnimation(engine) {}

void BattleView::init(bl::rc::scene::CodeScene* scene) {
    bgndTxtr = engine.renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/battleBgnd.png"));
    background.create(engine, bgndTxtr);
    background.addToScene(scene, bl::rc::UpdateSpeed::Static);

    printer.init(scene);
    statBoxes.init(scene);
    moveAnimation.init(scene);
    playerMenu.init(scene);
    localPeoplemon.init(scene);
    opponentPeoplemon.init(scene);

    playerMenu.setPeoplemon(battleState.localPlayer().outNowIndex(),
                            battleState.localPlayer().activePeoplemon());
    localPeoplemon.setPeoplemon(battleState.localPlayer().activePeoplemon().base().id());
    opponentPeoplemon.setPeoplemon(battleState.enemy().activePeoplemon().base().id());
    moveAnimation.ensureLoaded(battleState.localPlayer().activePeoplemon(),
                               battleState.enemy().activePeoplemon());
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
    playerMenu.refresh();
}

void BattleView::render(bl::rc::scene::CodeScene::RenderContext& ctx) {
    background.draw(ctx);

    statBoxes.render(ctx);
    moveAnimation.renderBackground(ctx);
    localPeoplemon.render(ctx);
    opponentPeoplemon.render(ctx);
    moveAnimation.renderForeground(ctx);
    if ((battleState.currentStage() == BattleState::Stage::WaitingChoices ||
         battleState.currentStage() == BattleState::Stage::WaitingForgetMoveChoice) &&
        !playerMenu.ready()) {
        playerMenu.render(ctx);
    }
    printer.render(ctx);
}

bool BattleView::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                         bool eventTriggered) {
    if ((battleState.currentStage() == BattleState::Stage::WaitingForgetMoveChoice ||
         battleState.currentStage() == BattleState::Stage::WaitingChoices) &&
        !playerMenu.ready()) {
        playerMenu.handleInput(ctrl, eventTriggered);
    }
    else { printer.process(ctrl, eventTriggered); }
    return true;
}

} // namespace battle
} // namespace core
