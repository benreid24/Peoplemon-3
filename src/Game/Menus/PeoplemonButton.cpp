#include <Game/Menus/PeoplemonButton.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace menu
{
PeoplemonButton::Ptr PeoplemonButton::create(const core::pplmn::OwnedPeoplemon& ppl) {
    return Ptr(new PeoplemonButton(ppl));
}

PeoplemonButton::PeoplemonButton(const core::pplmn::OwnedPeoplemon& ppl)
: color(sf::Color::White) {
    txtr = bl::engine::Resources::textures()
               .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                  "Peoplemon/button.png"))
               .data;
    image.setTexture(*txtr, true);

    name.setFont(core::Properties::MenuFont());
    name.setCharacterSize(22);
    name.setStyle(sf::Text::Bold);
    name.setFillColor(sf::Color(60, 225, 200));
    name.setString(ppl.name());
    name.setPosition(135.f, 18.f);

    const unsigned int curHp = ppl.currentHp();
    const unsigned int maxHp = ppl.currentStats().hp;
    const float hpPercent    = static_cast<float>(curHp) / static_cast<float>(maxHp);
    const sf::Color hpColor  = core::Properties::HPBarColor(curHp, maxHp);

    hpBar.setPosition(164.f, 70.f);
    hpBar.setSize({hpPercent * 178.f, 12.f});
    hpBar.setFillColor(hpColor);

    hpText.setFont(core::Properties::MenuFont());
    hpText.setCharacterSize(18);
    hpText.setString(std::to_string(curHp) + " / " + std::to_string(maxHp));
    hpText.setFillColor(hpColor);
    hpText.setPosition(342.f - hpText.getGlobalBounds().width - 8.f, 83.f);

    item.setFont(core::Properties::MenuFont());
    item.setCharacterSize(16);
    item.setFillColor(sf::Color(30, 230, 160));
    item.setString(ppl.holdItem() != core::item::Id::None ?
                       core::item::Item::getName(ppl.holdItem()) :
                       "No hold item");
    item.setPosition(85.f, 136.f);

    level.setFont(core::Properties::MenuFont());
    level.setString(std::to_string(ppl.currentLevel()));
    level.setPosition(304.f, 16.f);
    level.setFillColor(sf::Color(50, 220, 250));
    level.setCharacterSize(16);
    level.setStyle(sf::Text::Bold);

    faceTxtr = bl::engine::Resources::textures()
                   .load(core::pplmn::Peoplemon::thumbnailImage(ppl.id()))
                   .data;
    face.setTexture(*faceTxtr, true);
    sf::Rect<unsigned int> bounds(100000, 100000, 0, 0);
    const sf::Image img(faceTxtr->copyToImage());
    for (unsigned int x = 0; x < img.getSize().x; ++x) {
        for (unsigned int y = 0; y < img.getSize().y; ++y) {
            if (img.getPixel(x, y) != sf::Color::Transparent) {
                if (x < bounds.left) bounds.left = x;
                if (x > bounds.width) bounds.width = x;
                if (y < bounds.top) bounds.top = y;
                if (y > bounds.height) bounds.height = y;
            }
        }
    }
    const sf::Vector2f size(bounds.width - bounds.left, bounds.height - bounds.top);
    const sf::Vector2f dscale(95.f / size.x, 95.f / size.y);
    const float scale = std::min(dscale.x, dscale.y);
    face.setOrigin(face.getGlobalBounds().width * 0.5f, face.getGlobalBounds().height * 0.5f);
    face.setScale(scale, scale);
    face.setPosition(71.f, 76.f);

    getSignal(bl::menu::Item::Selected).willAlwaysCall([this]() {
        image.setColor(sf::Color(170, 190, 30));
    });
    getSignal(bl::menu::Item::Deselected).willAlwaysCall([this]() { image.setColor(color); });
}

void PeoplemonButton::setHighlightColor(const sf::Color& c) {
    color = c;
    if (!isSelected) { image.setColor(color); }
}

sf::Vector2f PeoplemonButton::getSize() const {
    return {image.getGlobalBounds().width, image.getGlobalBounds().height};
}

void PeoplemonButton::render(sf::RenderTarget& target, sf::RenderStates states,
                             const sf::Vector2f& pos) const {
    states.transform.translate(pos);
    target.draw(hpBar, states);
    target.draw(image, states);
    target.draw(face, states);
    target.draw(name, states);
    target.draw(level, states);
    target.draw(item, states);
    target.draw(hpText, states);
}

} // namespace menu
} // namespace game
