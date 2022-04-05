#include <Core/Battles/View/PlayerMenu.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
namespace view
{
PlayerMenu::PlayerMenu(bool canRun)
: state(State::Hidden)
, actionMenu(bl::menu::ArrowSelector::create(12.f, sf::Color::Black))
, moveMenu(bl::menu::ArrowSelector::create(12.f, sf::Color::Black)) {
    using namespace bl::menu;

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
    actionMenu.setPosition({513.f, 476.f});
    actionMenu.setPadding({12.f, 12.f});

    for (int i = 0; i < 4; ++i) {
        moves[i]     = pplmn::MoveId::Unknown;
        moveItems[i] = TextItem::create("<UNSET>", Properties::MenuFont(), sf::Color::Black, 30);
        moveItems[i]
            ->getSignal(Item::Activated)
            .willAlwaysCall(std::bind(&PlayerMenu::moveChosen, this, i));
    }

    // TODO - init move stat stuff
}

void PlayerMenu::setPeoplemon(const pplmn::BattlePeoplemon&) {
    // TODO - sync moves to menu
}

void PlayerMenu::beginTurn() {
    state = State::PickingAction;
    menuDriver.drive(&actionMenu);
}

void PlayerMenu::chooseFaintReplacement() {
    state = State::PickingPeoplemon;
    // TODO - open peoplemon menu
}

bool PlayerMenu::ready() const { return state == State::Hidden; }

TurnAction PlayerMenu::selectedAction() const { return chosenAction; }

int PlayerMenu::selectedMove() const { return chosenMoveOrPeoplemon; }

item::Id PlayerMenu::selectedItem() const { return chosenItem; }

int PlayerMenu::selectedPeoplemon() const { return chosenMoveOrPeoplemon; }

void PlayerMenu::handleInput(component::Command cmd) {
    menuDriver.process(cmd);
    if (state == State::PickingMove && cmd == component::Command::Back) {
        state = State::PickingAction;
    }
}

void PlayerMenu::render(sf::RenderTarget& target) const {
    if (state == State::PickingAction) { actionMenu.render(target); }
    else if (state == State::PickingMove) {
        moveMenu.render(target);
        target.draw(moveBox);
        target.draw(movePP);
        target.draw(movePwr);
        target.draw(moveAcc);
    }
}

void PlayerMenu::fightChosen() {
    state        = State::PickingMove;
    chosenAction = TurnAction::Fight;
}

void PlayerMenu::switchChosen() {
    state        = State::PickingPeoplemon;
    chosenAction = TurnAction::Switch;
    // TODO - open menu
}

void PlayerMenu::itemChosen() {
    state        = State::PickingItem;
    chosenAction = TurnAction::Item;
    // TODO - open menu
}

void PlayerMenu::runChosen() {
    chosenAction = TurnAction::Run;
    state        = State::Hidden;
}

void PlayerMenu::moveChosen(int i) {
    chosenMoveOrPeoplemon = i;
    state                 = State::Hidden;
}

} // namespace view
} // namespace battle
} // namespace core
