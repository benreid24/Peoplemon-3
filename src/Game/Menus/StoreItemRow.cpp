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
: item(item)
, qty(qty)
, price(price) {}

void StoreItemRow::updateQty(int qty) {
    qtyText.getSection().setString(std::to_string(std::min(qty, 999)));
}

glm::vec2 StoreItemRow::getSize() const { return {Width, Height}; }

core::item::Id StoreItemRow::getItem() const { return item; }

bl::com::Transform2D& StoreItemRow::doCreate(bl::engine::Engine& engine, bl::ecs::Entity parent) {
    dummy.create(engine);
    dummy.setParent(parent);
    dummy.setSize({Width, Height});

    qtyText.create(engine, core::Properties::MenuFont(), "", FontSize, sf::Color(30, 75, 240));
    qtyText.setParent(dummy);
    qtyText.getTransform().setPosition({Margin, TopMargin});
    if (qty > 0) { updateQty(qty); }
    const float LeftMargin = qtyText.getLocalBounds().width;

    nameText.create(engine,
                    core::Properties::MenuFont(),
                    core::item::Item::getName(item),
                    FontSize,
                    sf::Color::Black);
    nameText.getTransform().setPosition({Margin * 2.f + LeftMargin, TopMargin});
    nameText.setParent(dummy);

    priceText.create(engine,
                     core::Properties::MenuFont(),
                     "$" + std::to_string(price),
                     FontSize,
                     sf::Color::Black);
    priceText.getTransform().setPosition(
        {Width - Margin - priceText.getLocalBounds().width, TopMargin});
    priceText.setParent(dummy);

    return dummy.getTransform();
}

void StoreItemRow::doSceneAdd(bl::rc::Overlay* overlay) {
    nameText.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    priceText.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    qtyText.addToScene(overlay, bl::rc::UpdateSpeed::Static);
}

void StoreItemRow::doSceneRemove() {
    nameText.removeFromScene();
    priceText.removeFromScene();
    qtyText.removeFromScene();
}

bl::ecs::Entity StoreItemRow::getEntity() const { return bl::ecs::Entity(); }

} // namespace menu
} // namespace game
