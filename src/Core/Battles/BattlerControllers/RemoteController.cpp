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

void RemoteController::removeItem(item::Id) {
    // TODO - send command over network
}

} // namespace battle
} // namespace core
