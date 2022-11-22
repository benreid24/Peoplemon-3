#include <Game/Menus/BagItemButton.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace menu
{
BagItemButton::Ptr BagItemButton::create(const core::player::Bag::Item& item) {
    return Ptr(new BagItemButton(item));
}

BagItemButton::BagItemButton(const core::player::Bag::Item& item) {
    txtr =
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "Bag/button.png"))
            .data;
    background.setTexture(*txtr, true);

    label.setFillColor(sf::Color::Black);
    label.setFont(core::Properties::MenuFont());
    label.setCharacterSize(22);

    qty.setFillColor(sf::Color(30, 140, 230));
    qty.setFont(core::Properties::MenuFont());
    qty.setCharacterSize(18);

    update(item);
    getSignal(Item::Selected).willAlwaysCall(std::bind(&BagItemButton::onSelect, this));
    getSignal(Item::Deselected).willAlwaysCall(std::bind(&BagItemButton::onDeselect, this));
}

void BagItemButton::update(const core::player::Bag::Item& i) {
    item = i;
    if (i.id == core::item::Id::None) {
        label.setString("Exit");
        qty.setString("");
        label.setPosition(
            background.getGlobalBounds().width * 0.5f - label.getGlobalBounds().width * 0.5f,
            background.getGlobalBounds().height * 0.5f - label.getGlobalBounds().height * 0.5f -
                label.getLocalBounds().top * 0.5f);
    }
    else {
        label.setString(core::item::Item::getName(item.id));
        qty.setString("Qty: " + std::to_string(item.qty));
        label.setPosition(8.f,
                          background.getGlobalBounds().height * 0.5f -
                              label.getGlobalBounds().height * 0.5f);
        qty.setPosition(background.getGlobalBounds().width * 0.75f,
                        background.getGlobalBounds().height * 0.5f -
                            qty.getGlobalBounds().height * 0.5f - qty.getLocalBounds().top * 0.5f);
    }

    label.setCharacterSize(22);
    while (label.getGlobalBounds().width > background.getGlobalBounds().width * 0.75f - 8.f) {
        label.setCharacterSize(label.getCharacterSize() - 1);
        if (label.getCharacterSize() <= 8) {
            BL_LOG_WARN << "Item name too long to fit: " << label.getString().toAnsiString();
            break;
        }
    }
}

const core::player::Bag::Item& BagItemButton::getItem() const { return item; }

sf::Vector2f BagItemButton::getSize() const {
    return {background.getGlobalBounds().width, background.getGlobalBounds().height};
}

void BagItemButton::render(sf::RenderTarget& target, sf::RenderStates states,
                           const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(background, states);
    target.draw(label, states);
    target.draw(qty, states);
}

void BagItemButton::onSelect() { background.setColor(sf::Color(40, 230, 140)); }

void BagItemButton::onDeselect() {
    background.setColor(item.id != core::item::Id::None ? sf::Color::White :
                                                          sf::Color(240, 40, 40));
}

} // namespace menu
} // namespace game
