#include <Core/Battles/BattlerControllers/AIController.hpp>

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
}

void AIController::startChoosePeoplemon() {
    // TODO - battle ai
}

} // namespace battle
} // namespace core
