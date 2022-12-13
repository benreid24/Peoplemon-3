#include <Game/Menus/PeopledexRow.hpp>

#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace menu
{
namespace
{
std::string toString(core::pplmn::Id ppl) {
    std::string r = std::to_string(static_cast<int>(ppl));
    while (r.size() < 3) { r.insert(0, "0"); }
    return "No" + r;
}
} // namespace

PeopledexRow::Ptr PeopledexRow::create(core::pplmn::Id ppl, const core::player::Peopledex& dex) {
    return Ptr{new PeopledexRow(ppl, dex)};
}

PeopledexRow::PeopledexRow(core::pplmn::Id ppl, const core::player::Peopledex& dex) {
    const auto intel  = dex.getIntelLevel(ppl);
    const bool caught = dex.getCaught(ppl) > 0;

    const std::string& ImgPath = core::Properties::MenuImagePath();
    auto& jp                   = bl::util::FileUtil::joinPath;

    bgndTxtr       = TextureManager::load(jp(ImgPath, "Peopledex/item.png"));
    activeBgndTxtr = TextureManager::load(jp(ImgPath, "Peopledex/itemActive.png"));
    background.setTexture(*bgndTxtr, true);
    const float mid = static_cast<float>(bgndTxtr->getSize().y) * 0.5f;

    ballTxtr = TextureManager::load(jp(core::Properties::ImagePath(), "item.png"));
    if (caught) {
        ball.setTexture(*ballTxtr, true);
        ball.setOrigin(sf::Vector2f(ballTxtr->getSize()) * 0.5f);
    }
    ball.setPosition({26.f, mid});
    const float bw = static_cast<float>(ballTxtr->getSize().x) * 0.5f;

    id.setFont(core::Properties::MenuFont());
    id.setFillColor(sf::Color::Black);
    id.setCharacterSize(14);
    id.setString(toString(ppl));
    id.setPosition(bw + ball.getPosition().x, mid);
    id.setOrigin(0.f, id.getGlobalBounds().height * 0.5f + id.getLocalBounds().top * 0.5f);

    name.setFont(core::Properties::MenuFont());
    name.setFillColor(sf::Color::Black);
    name.setCharacterSize(24);
    name.setString(intel != core::player::Peopledex::NoIntel ? core::pplmn::Peoplemon::name(ppl) :
                                                               "???");
    name.setOrigin(0.f, name.getGlobalBounds().height * 0.5f + name.getLocalBounds().top * 0.5f);
    name.setPosition(id.getPosition().x + 65.f, mid);

    getSignal(Item::Selected).willAlwaysCall(std::bind(&PeopledexRow::makeActive, this));
    getSignal(Item::Deselected).willAlwaysCall(std::bind(&PeopledexRow::makeInactive, this));
}

sf::Vector2f PeopledexRow::getSize() const { return sf::Vector2f(bgndTxtr->getSize()); }

void PeopledexRow::render(sf::RenderTarget& target, sf::RenderStates states,
                          const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(background, states);
    target.draw(ball, states);
    target.draw(id, states);
    target.draw(name, states);
}

void PeopledexRow::makeActive() { background.setTexture(*activeBgndTxtr, true); }

void PeopledexRow::makeInactive() { background.setTexture(*bgndTxtr, true); }

} // namespace menu
} // namespace game
