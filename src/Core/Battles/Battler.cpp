#include <Core/Battles/Battle.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Peoplemon/Move.hpp>

namespace core
{
namespace battle
{
Battler::Battler(BattleState& state)
: state(state)
, currentPeoplemon(0) {}

void Battler::init(std::vector<core::pplmn::BattlePeoplemon>&& t,
                   std::unique_ptr<BattlerController>&& c) {
    team = t;
    controller.swap(c);
    controller->setOwner(*this);
}

void Battler::notifyTurnBegin() { substate.notifyTurnBegin(); }

void Battler::notifyTurnEnd() { substate.notifyTurnEnd(chosenAction(), activePeoplemon()); }

bool Battler::actionSelected() const {
    if (substate.chargingMove >= 0) return true;
    if (substate.encoreTurnsLeft > 0) return true;
    return controller->actionSelected();
}

void Battler::pickAction() {
    if (substate.chargingMove >= 0) return;
    if (substate.encoreTurnsLeft > 0) return;
    controller->pickAction();
}

void Battler::pickPeoplemon(bool ff, bool ro) { controller->pickPeoplemon(ff, ro); }

TurnAction Battler::chosenAction() const {
    if (substate.chargingMove >= 0) { return TurnAction::Fight; }
    if (substate.encoreTurnsLeft > 0) { return TurnAction::Fight; }
    return controller->chosenAction();
}

int Battler::chosenMove() const {
    if (substate.chargingMove >= 0) return substate.chargingMove;
    if (substate.encoreMove >= 0) return substate.encoreMove;
    return controller->chosenMove();
}

core::item::Id Battler::chosenItem() const { return controller->chosenItem(); }

std::uint8_t Battler::chosenPeoplemon() const { return controller->chosenPeoplemon(); }

std::vector<core::pplmn::BattlePeoplemon>& Battler::peoplemon() { return team; }

core::pplmn::BattlePeoplemon& Battler::activePeoplemon() { return team[currentPeoplemon]; }

void Battler::setActivePeoplemon(unsigned int i) {
    currentPeoplemon = i;
    substate.notifySwitch();
}

unsigned int Battler::outNowIndex() const { return currentPeoplemon; }

const std::string& Battler::name() const { return controller->name(); }

unsigned int Battler::getPriority() const {
    switch (chosenAction()) {
    case TurnAction::Fight:
        return pplmn::Move::priority(team[currentPeoplemon].base().knownMoves()[chosenMove()].id);
    case TurnAction::Item:
    case TurnAction::Run:
    case TurnAction::Switch:
        return 1000;
    default:
        BL_LOG_ERROR << "Unknown turn action: " << chosenAction();
        return 0;
    }
}

bool Battler::canFight() const {
    for (const auto& ppl : team) {
        if (ppl.base().currentHp() > 0) return true;
    }
    return false;
}

bool Battler::canSwitch() const {
    for (const auto& ppl : team) {
        if (ppl.base().currentHp() > 0 && &ppl != &team[currentPeoplemon]) return true;
    }
    return false;
}

void Battler::refresh() { controller->refresh(); }

BattlerSubstate& Battler::getSubstate() { return substate; }

unsigned int Battler::selectRandomPeoplemon() const {
    if (!canSwitch()) return currentPeoplemon;

    unsigned int r = 0;
    do {
        r = bl::util::Random::get<unsigned int>(0, team.size());
    } while (team[r].base().currentHp() == 0 || r == currentPeoplemon);
    return r;
}

} // namespace battle
} // namespace core
