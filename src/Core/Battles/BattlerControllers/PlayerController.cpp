#include <Core/Battles/BattlerControllers/PlayerController.hpp>

namespace core
{
namespace battle
{
const std::string& PlayerController::name() const {
    static const std::string n = "<PLAYER>";
    return n;
}

void PlayerController::startChooseAction() {
    // TODO - interface with ui
}

void PlayerController::startChoosePeoplemon() {
    // TODO - interface with ui
}

} // namespace battle
} // namespace core
