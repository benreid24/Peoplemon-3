#include <Game/Menus/StoreItemRow.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace menu
{
namespace
{
constexpr unsigned int FontSize = 16;
constexpr float Margin          = 4.f;
constexpr float TopMargin       = Margin * 0.5f;
constexpr float Width           = 352.f;
} // namespace

StoreItemRow::Ptr StoreItemRow::create(int qty, core::item::Id item, int price) {
    return Ptr{new StoreItemRow(qty, item, price)};
}

StoreItemRow::Ptr StoreItemRow::create(core::item::Id item, int price) {
    return Ptr{new StoreItemRow(-1, item, price)};
}

StoreItemRow::StoreItemRow(int qty, core::item::Id item, int price)
: item(item) {
    const sf::Font& font = core::Properties::MenuFont();
    qtyText.setFont(font);
    nameText.setFont(font);
    priceText.setFont(font);
    qtyText.setCharacterSize(FontSize);
    nameText.setCharacterSize(FontSize);
    priceText.setCharacterSize(FontSize);
    qtyText.setFillColor(sf::Color::Black);
    nameText.setFillColor(sf::Color::Black);
    priceText.setFillColor(sf::Color::Black);

    if (qty > 0) { updateQty(qty); }
    else {
        qtyText.setString("");
    }
    const float LeftMargin = qtyText.getGlobalBounds().width;
    nameText.setString(core::item::Item::getName(item));
    priceText.setString("$" + std::to_string(price));

    qtyText.setPosition({Margin, TopMargin});
    nameText.setPosition({Margin * 2.f + LeftMargin, TopMargin});
    priceText.setPosition({Width - Margin - priceText.getGlobalBounds().width, TopMargin});
}

void StoreItemRow::updateQty(int qty) { qtyText.setString(std::to_string(std::min(qty, 999))); }

sf::Vector2f StoreItemRow::getSize() const { return {Width, Height}; }

core::item::Id StoreItemRow::getItem() const { return item; }

void StoreItemRow::render(sf::RenderTarget& target, sf::RenderStates states,
                          const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(qtyText, states);
    target.draw(nameText, states);
    target.draw(priceText, states);
}

} // namespace menu
} // namespace game
