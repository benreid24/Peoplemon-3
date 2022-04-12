#include <Core/Battles/BattlerControllers/RemoteController.hpp>

namespace core
{
namespace battle
{
const std::string& RemoteController::name() const {
    static const std::string n = "<NETWORK>";
    return n;
}

void RemoteController::startChooseAction() {
    // TODO - get from network
}

void RemoteController::startChoosePeoplemon() {
    // TODO - get from network
}

} // namespace battle
} // namespace core
