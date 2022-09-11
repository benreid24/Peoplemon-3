#include <Core/Battles/BattlerControllers/AIController.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Battles/Battler.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace battle
{
AIController::AIController(pplmn::Id wildId)
: _name(pplmn::Peoplemon::name(wildId)) {}

AIController::AIController(const std::string& tn, const std::vector<item::Id>& items)
: _name(tn)
, items(items) {}

const std::string& AIController::name() const { return _name; }

void AIController::refresh() {}

void AIController::startChooseAction() {
    // TODO - battle ai
    int i = 0;
    do {
        i = bl::util::Random::get<int>(0, 3);
    } while (owner->activePeoplemon().base().knownMoves()[i].id == pplmn::MoveId::Unknown);
    chooseMove(i);
}

void AIController::startChoosePeoplemon(bool, bool reviveOnly) {
    // TODO - battle ai
    for (unsigned int i = 0; i < owner->peoplemon().size(); ++i) {
        if (owner->peoplemon()[i].base().currentHp() > 0 && !reviveOnly) {
            choosePeoplemon(i);
            return;
        }
        else if (owner->peoplemon()[i].base().currentHp() == 0 && reviveOnly) {
            choosePeoplemon(i);
            return;
        }
    }
    choosePeoplemon(-1);
}

void AIController::startChooseToContinue() { chooseGiveUp(false); }

bool AIController::removeItem(item::Id item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == item) {
            items.erase(it);
            return true;
        }
    }
    return false;
}

bool AIController::isHost() const { return false; }

} // namespace battle
} // namespace core
