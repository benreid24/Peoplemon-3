#include <Core/Battles/View/PlayerMenu.hpp>

namespace core
{
namespace battle
{
namespace view
{
PlayerMenu::PlayerMenu()
: actionMenu(bl::menu::ArrowSelector::create(12.f, sf::Color::Black))
, moveMenu(bl::menu::ArrowSelector::create(12.f, sf::Color::Black)) {
    // TODO
}

void PlayerMenu::setPeoplemon(const pplmn::BattlePeoplemon&) {
    // TODO
}

void PlayerMenu::beginTurn() {
    // TODO
}

void PlayerMenu::chooseFaintReplacement() {
    // TODO
}

bool PlayerMenu::turnActionSelected() const {
    // TODO
    return false;
}

TurnAction PlayerMenu::selectedAction() const {
    // TODO
    return TurnAction::Run;
}

bool PlayerMenu::subActionSelected() const {
    // TODO
    return false;
}

int PlayerMenu::selectedMove() const {
    // TODO
    return -1;
}

item::Id PlayerMenu::selectedItem() const {
    // TODO
    return item::Id::Unknown;
}

int PlayerMenu::selectedPeoplemon() const {
    // TODO
    return -1;
}

void PlayerMenu::handleInput(component::Command) {
    // TODO
}

void PlayerMenu::render(sf::RenderTarget&) const {
    // TODO
}

} // namespace view
} // namespace battle
} // namespace core