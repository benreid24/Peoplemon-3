#include <Core/Battles/BattlerControllers/AIController.hpp>

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
    chooseMove(0);
}

void AIController::startChoosePeoplemon() {
    // TODO - battle ai
    for (unsigned int i = 0; i < owner->peoplemon().size(); ++i) {
        if (owner->peoplemon()[i].base().currentHp() > 0) {
            choosePeoplemon(i);
            break;
        }
    }
    choosePeoplemon(-1);
}

} // namespace battle
} // namespace core
