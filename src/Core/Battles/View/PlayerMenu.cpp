#include <Core/Battles/View/PlayerMenu.hpp>

#include <BLIB/Audio/AudioSystem.hpp>
#include <Core/Events/BagMenu.hpp>
#include <Core/Events/PeoplemonMenu.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
constexpr glm::vec2 MoveBoxPos(499.f, 463.f);
}

PlayerMenu::PlayerMenu(bl::engine::Engine& engine, bool canRun)
: engine(engine)
, canRun(canRun)
, state(State::Hidden)
, stateLoopGuard(false)
, currentPeoplemon(0)
, moves(nullptr) {}

void PlayerMenu::init(bl::rc::scene::CodeScene* scene) {
    using namespace bl::menu;

    actionMenu.create(engine,
                      engine.renderer().getObserver(),
                      bl::menu::ArrowSelector::create(12.f, sf::Color::Black));
    actionMenu.addToScene(scene);

    moveMenu.create(engine,
                    engine.renderer().getObserver(),
                    bl::menu::ArrowSelector::create(12.f, sf::Color::Black));
    moveMenu.addToScene(scene);

    fightItem = TextItem::create("Fight", Properties::MenuFont(), sf::Color::Black, 30);
    fightItem->getSignal(Item::Activated).willAlwaysCall(std::bind(&PlayerMenu::fightChosen, this));
    bagItem = TextItem::create("Bag", Properties::MenuFont(), sf::Color::Black, 30);
    bagItem->getSignal(Item::Activated).willAlwaysCall(std::bind(&PlayerMenu::itemChosen, this));
    switchItem = TextItem::create("Switch", Properties::MenuFont(), sf::Color::Black, 30);
    switchItem->getSignal(Item::Activated)
        .willAlwaysCall(std::bind(&PlayerMenu::switchChosen, this));
    runItem = TextItem::create("Run", Properties::MenuFont(), sf::Color::Black, 30);
    runItem->getSignal(Item::Activated).willAlwaysCall(std::bind(&PlayerMenu::runChosen, this));

    actionMenu.setRootItem(fightItem);
    actionMenu.addItem(bagItem, fightItem.get(), Item::Right);
    actionMenu.addItem(switchItem, fightItem.get(), Item::Bottom);
    if (canRun) {
        actionMenu.addItem(runItem, bagItem.get(), Item::Bottom);
        actionMenu.attachExisting(runItem.get(), switchItem.get(), Item::Right);
    }
    else {
        actionMenu.attachExisting(switchItem.get(), bagItem.get(), Item::Bottom, false);
        actionMenu.attachExisting(bagItem.get(), switchItem.get(), Item::Right, false);
    }
    actionMenu.setPosition({513.f, 476.f});
    actionMenu.setPadding({40.f, 17.f});

    moveMenu.setPosition({16.f, 477.f});
    moveMenu.setPadding({25.f, 20.f});

    moveTxtr = engine.renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/movebox.png"));
    moveBox.create(engine, moveTxtr);
    moveBox.getTransform().setPosition(MoveBoxPos);
    moveBox.addToScene(scene, bl::rc::UpdateSpeed::Static);

    movePwr.create(engine, Properties::MenuFont(), "", 25, sf::Color::Black);
    movePwr.getTransform().setPosition(77.f, 20.f);
    movePwr.setParent(moveBox);
    movePwr.addToScene(scene, bl::rc::UpdateSpeed::Static);

    moveAcc.create(engine, Properties::MenuFont(), "", 25, sf::Color::Black);
    moveAcc.getTransform().setPosition(77.f, 53.f);
    moveAcc.setParent(moveBox);
    moveAcc.addToScene(scene, bl::rc::UpdateSpeed::Static);

    movePP.create(engine, Properties::MenuFont(), "", 25, sf::Color::Black);
    movePP.getTransform().setPosition(77.f, 87.f);
    movePP.setParent(moveBox);
    movePP.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

void PlayerMenu::setPeoplemon(int i, const pplmn::BattlePeoplemon& ppl) {
    using namespace bl::menu;

    currentPeoplemon = i;
    moves            = ppl.base().knownMoves();
    for (int i = 0; i < 4; ++i) { moveItems[i].reset(); }

    for (int i = 0; i < 4; ++i) {
        if (moves[i].id == pplmn::MoveId::Unknown) break;

        moveItems[i] = TextItem::create(
            pplmn::Move::name(moves[i].id), Properties::MenuFont(), sf::Color::Black, 24);
        moveItems[i]
            ->getSignal(Item::Activated)
            .willAlwaysCall(std::bind(&PlayerMenu::moveChosen, this, i));
        moveItems[i]
            ->getSignal(Item::Selected)
            .willAlwaysCall(std::bind(&PlayerMenu::syncMove, this, i));
        if (i == 1) { moveItems[i]->overridePosition({240.f, 0.f}); }
    }

    moveMenu.setRootItem(moveItems[0]);
    if (moveItems[1]) {
        moveMenu.addItem(moveItems[1], moveItems[0].get(), Item::Right);
        if (moveItems[2]) {
            moveMenu.addItem(moveItems[2], moveItems[0].get(), Item::Bottom);
            if (moveItems[3]) {
                moveMenu.addItem(moveItems[3], moveItems[1].get(), Item::Bottom);
                moveMenu.attachExisting(moveItems[3].get(), moveItems[2].get(), Item::Right);
            }
            else {
                moveMenu.attachExisting(
                    moveItems[2].get(), moveItems[1].get(), Item::Bottom, false);
                moveMenu.attachExisting(moveItems[1].get(), moveItems[2].get(), Item::Right, false);
            }
        }
    }
}

void PlayerMenu::refresh() {
    switch (state) {
    case State::PickingItem:
        if (chosenItem != item::Id::None) { state = State::Hidden; }
        else if (stateLoopGuard) { stateLoopGuard = false; }
        else { state = State::PickingAction; }
        break;
    case State::PickingPeoplemon:
        if (chosenMoveOrPeoplemon != -1 && chosenMoveOrPeoplemon != currentPeoplemon) {
            state = State::Hidden;
        }
        else if (stateLoopGuard) { stateLoopGuard = false; }
        else {
            state                 = State::PickingAction;
            chosenMoveOrPeoplemon = -1;
        }
        break;
    default:
        break;
    }
}

void PlayerMenu::beginTurn() {
    state = State::PickingAction;
    menuDriver.drive(&actionMenu);
}

void PlayerMenu::choosePeoplemonMidTurn(bool fromFaint, bool fromRevive) {
    state                 = State::PickingPeoplemon;
    chosenMoveOrPeoplemon = -1;
    stateLoopGuard        = true;
    if (fromFaint) {
        bl::event::Dispatcher::dispatch<event::OpenPeoplemonMenu>(
            {event::OpenPeoplemonMenu::Context::BattleFaint,
             currentPeoplemon,
             &chosenMoveOrPeoplemon});
    }
    else if (fromRevive) {
        bl::event::Dispatcher::dispatch<event::OpenPeoplemonMenu>(
            {event::OpenPeoplemonMenu::Context::BattleReviveSwitch,
             currentPeoplemon,
             &chosenMoveOrPeoplemon});
    }
    else {
        bl::event::Dispatcher::dispatch<event::OpenPeoplemonMenu>(
            {event::OpenPeoplemonMenu::Context::BattleMustSwitch,
             currentPeoplemon,
             &chosenMoveOrPeoplemon});
    }
}

void PlayerMenu::chooseMoveToForget() {
    state = State::ChoosingMoveToForget;
    menuDriver.drive(&moveMenu);
}

bool PlayerMenu::ready() const { return state == State::Hidden; }

TurnAction PlayerMenu::selectedAction() const { return chosenAction; }

int PlayerMenu::selectedMove() const { return chosenMoveOrPeoplemon; }

item::Id PlayerMenu::selectedItem() const { return chosenItem; }

int PlayerMenu::selectedPeoplemon() const { return chosenMoveOrPeoplemon; }

void PlayerMenu::handleInput(input::EntityControl cmd, bool fromEvent) {
    menuDriver.sendControl(cmd, fromEvent);
    if (cmd == input::Control::Back && fromEvent) {
        if (state == State::PickingMove) {
            state = State::PickingAction;
            menuDriver.drive(&actionMenu);
            bl::audio::AudioSystem::playOrRestartSound(Properties::MenuBackSound());
        }
        else if (state == State::ChoosingMoveToForget) {
            // TODO - maybe confirm here that player wants to not learn
            moveChosen(-1);
        }
    }
}

void PlayerMenu::render(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (state == State::PickingAction) { actionMenu.draw(ctx); }
    else if (state == State::PickingMove || state == State::ChoosingMoveToForget) {
        moveMenu.draw(ctx);
        moveBox.draw(ctx);
        movePP.draw(ctx);
        movePwr.draw(ctx);
        moveAcc.draw(ctx);
    }
}

void PlayerMenu::fightChosen() {
    state        = State::PickingMove;
    chosenAction = TurnAction::Fight;

    bool useFlail = true;
    for (int i = 0; i < 4; ++i) {
        if (moves[i].id != pplmn::MoveId::Unknown && moves[i].curPP > 0) {
            useFlail = false;
            break;
        }
    }
    if (useFlail) {
        state                 = State::Hidden;
        chosenMoveOrPeoplemon = -1;
    }
    else {
        menuDriver.drive(&moveMenu);
        for (int i = 0; i < 4; ++i) {
            if (moveMenu.getSelectedItem() == moveItems[i].get()) {
                syncMove(i);
                break;
            }
        }
    }
}

void PlayerMenu::switchChosen() {
    state                 = State::PickingPeoplemon;
    chosenAction          = TurnAction::Switch;
    chosenMoveOrPeoplemon = -1;
    stateLoopGuard        = true;
    bl::event::Dispatcher::dispatch<event::OpenPeoplemonMenu>(
        {event::OpenPeoplemonMenu::Context::BattleSwitch,
         currentPeoplemon,
         &chosenMoveOrPeoplemon});
}

void PlayerMenu::itemChosen() {
    state          = State::PickingItem;
    chosenAction   = TurnAction::Item;
    stateLoopGuard = true;
    chosenItem     = item::Id::None;
    bl::event::Dispatcher::dispatch<event::OpenBagMenu>({event::OpenBagMenu::Context::BattleUse,
                                                         &chosenItem,
                                                         currentPeoplemon,
                                                         &chosenMoveOrPeoplemon});
}

void PlayerMenu::runChosen() {
    chosenAction = TurnAction::Run;
    state        = State::Hidden;
}

void PlayerMenu::moveChosen(int i) {
    chosenMoveOrPeoplemon = i;
    state                 = State::Hidden;
}

void PlayerMenu::syncMove(int i) {
    movePP.getSection().setString(std::to_string(moves[i].curPP) + "/" +
                                  std::to_string(moves[i].maxPP));
    movePwr.getSection().setString(std::to_string(pplmn::Move::damage(moves[i].id)));
    moveAcc.getSection().setString(std::to_string(pplmn::Move::accuracy(moves[i].id)));
}

} // namespace view
} // namespace battle
} // namespace core
