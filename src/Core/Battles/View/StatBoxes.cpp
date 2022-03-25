#include <Core/Battles/View/StatBoxes.hpp>

namespace core
{
namespace battle
{
namespace view
{
StatBoxes::StatBoxes()
: localPlayer(nullptr)
, opponent(nullptr) {
    // TODO - load graphics and init all
}

void StatBoxes::setOpponent(pplmn::BattlePeoplemon* ppl) {
    opponent = ppl;
    sync();
}

void StatBoxes::setPlayer(pplmn::BattlePeoplemon* ppl) {
    localPlayer = ppl;
    sync();
}

void StatBoxes::sync() {
    // TODO - sync
}

void StatBoxes::update(float) {
    // TODO - update bar sizes and colors
}

bool StatBoxes::synced() const {
    // TODO
    return false;
}

void StatBoxes::render(sf::RenderTarget& target) const {
    target.draw(opHpBar);
    target.draw(opBox);
    target.draw(opName);
    target.draw(opLevel);
    target.draw(opAil);

    target.draw(lpHpBar);
    target.draw(lpXpBar);
    target.draw(lpBox);
    target.draw(lpName);
    target.draw(lpHp);
    target.draw(lpLevel);
    target.draw(lpAil);
}

} // namespace view
} // namespace battle
} // namespace core