#include <Core/Battles/BattleView.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
BattleView::BattleView()
: localPeoplemon(view::PeoplemonAnimation::Player)
, opponentPeoplemon(view::PeoplemonAnimation::Opponent) {
    // TODO - init implementation details

    temp.setFont(Properties::MenuFont());
    temp.setCharacterSize(32);
    temp.setFillColor(sf::Color::Green);
    temp.setPosition({300.f, 300.f});
    temp.setString("Battle here");
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

void BattleView::render(sf::RenderTarget& target, float) const {
    // TODO - render the view

    target.clear();
    target.draw(temp);
}

} // namespace battle
} // namespace core
