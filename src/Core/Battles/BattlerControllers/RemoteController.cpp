#include <Core/Battles/BattlerControllers/RemoteController.hpp>

namespace core
{
namespace battle
{
const std::string& RemoteController::name() const {
    static const std::string n = "<NETWORK>";
    return n;
}

void RemoteController::refresh() {}

void RemoteController::startChooseAction() {
    // TODO - get from network
}

void RemoteController::startChoosePeoplemon(bool, bool) {
    // TODO - get from network
}

void RemoteController::startChooseToContinue() { chooseGiveUp(false); }

bool RemoteController::removeItem(item::Id) {
    // TODO - send command over network
    return false;
}

bool RemoteController::isHost() const { return false; }

} // namespace battle
} // namespace core
