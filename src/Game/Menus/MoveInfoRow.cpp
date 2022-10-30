#include <Game/Menus/MoveInfoRow.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace menu
{
bl::menu::Item::Ptr MoveInfoRow::create(core::pplmn::MoveId move) {
    return Ptr(new MoveInfoRow(move));
}

MoveInfoRow::MoveInfoRow(core::pplmn::MoveId move) {
    const auto& joinPath      = bl::util::FileUtil::joinPath;
    const std::string ImgPath = joinPath(core::Properties::MenuImagePath(), "PplInfo");
    auto& txtrs               = bl::engine::Resources::textures();

    bgndTxtr       = txtrs.load(joinPath(ImgPath, "move.png")).data;
    activeBgndTxtr = txtrs.load(joinPath(ImgPath, "moveActive.png")).data;
    background.setTexture(*bgndTxtr, true);

    name.setFont(core::Properties::MenuFont());
    name.setCharacterSize(26);
    name.setFillColor(sf::Color(30, 50, 65));
    name.setString(core::pplmn::Move::name(move));
    name.setPosition(getSize() * 0.5f);
    name.setOrigin(name.getGlobalBounds().width * 0.5f, name.getGlobalBounds().height * 0.5f);

    getSignal(Selected).willAlwaysCall(std::bind(&MoveInfoRow::makeActive, this));
    getSignal(Deselected).willAlwaysCall(std::bind(&MoveInfoRow::makeInactive, this));
}

sf::Vector2f MoveInfoRow::getSize() const { return sf::Vector2f(bgndTxtr->getSize()); }

void MoveInfoRow::render(sf::RenderTarget& target, sf::RenderStates states,
                         const sf::Vector2f& pos) const {
    states.transform.translate(pos);
    target.draw(background, states);
    target.draw(name, states);
}

void MoveInfoRow::makeActive() { background.setTexture(*activeBgndTxtr, true); }

void MoveInfoRow::makeInactive() { background.setTexture(*bgndTxtr, true); }

} // namespace menu
} // namespace game
