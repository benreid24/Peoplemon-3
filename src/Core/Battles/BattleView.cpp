#include <Core/Battles/BattleView.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView() {
    // TODO - init implementation details
}

bool BattleView::actionsCompleted() const {
    // TODO - add command queue
    return false;
}

void BattleView::queueMessage(const Message&) {
    // TODO - add command queue
}

void BattleView::playAnimation(const Animation&) {
    // TODO - add command queue
}

void BattleView::syncDisplay(const BattleState&) {
    // TODO - implement peoplemon animations and display boxes
}

void BattleView::update(float) {
    // TODO - update animations
}

void BattleView::render(sf::RenderTarget&, float) const {
    // TODO - render the view
}

} // namespace battle
} // namespace core
