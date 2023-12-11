#include <Game/Menus/BagItemButton.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace menu
{
BagItemButton::Ptr BagItemButton::create(const core::player::Bag::Item& item) {
    return Ptr(new BagItemButton(item));
}

BagItemButton::BagItemButton(const core::player::Bag::Item& item) {
    getSignal(Item::Selected).willAlwaysCall(std::bind(&BagItemButton::onSelect, this));
    getSignal(Item::Deselected).willAlwaysCall(std::bind(&BagItemButton::onDeselect, this));
}

void BagItemButton::update(const core::player::Bag::Item& i) {
    item = i;
    if (i.id == core::item::Id::None) {
        label.getSection().setString("Exit");
        qty.getSection().setString("");
        label.getTransform().setPosition(
            {txtr->size().x * 0.5f - label.getLocalBounds().width * 0.5f,
             txtr->size().y * 0.5f - label.getLocalBounds().height * 0.5f -
                 label.getLocalBounds().top * 0.5f});
    }
    else {
        label.getSection().setString(core::item::Item::getName(item.id));
        qty.getSection().setString("Qty: " + std::to_string(item.qty));
        label.getTransform().setPosition(
            8.f, txtr->size().y * 0.5f - label.getLocalBounds().height * 0.5f);
        qty.getTransform().setPosition(txtr->size().x * 0.75f,
                                       txtr->size().y * 0.5f - qty.getLocalBounds().height * 0.5f -
                                           qty.getLocalBounds().top * 0.5f);
    }

    label.getSection().setCharacterSize(22);
    while (label.getLocalBounds().width > txtr->size().x * 0.75f - 8.f) {
        label.getSection().setCharacterSize(label.getSection().getCharacterSize() - 1);
        if (label.getSection().getCharacterSize() <= 8) {
            BL_LOG_WARN << "Item name too long to fit: "
                        << label.getSection().getString().toAnsiString();
            break;
        }
    }
}

const core::player::Bag::Item& BagItemButton::getItem() const { return item; }

void BagItemButton::doCreate(bl::engine::Engine& engine) {
    txtr = engine.renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "Bag/button.png"));
    background.create(engine, txtr);

    label.create(engine, core::Properties::MenuFont(), "", 22, bl::sfcol(sf::Color::Black));
    label.setParent(background);

    qty.create(engine, core::Properties::MenuFont(), "", 18, bl::sfcol(sf::Color(30, 140, 230)));
    qty.setParent(background);

    update(item);
}

void BagItemButton::doSceneAdd(bl::rc::Overlay* overlay) {
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    label.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    qty.addToScene(overlay, bl::rc::UpdateSpeed::Static);
}

void BagItemButton::doSceneRemove() { background.removeFromScene(); }

bl::ecs::Entity BagItemButton::getEntity() const { return background.entity(); }

glm::vec2 BagItemButton::getSize() const { return txtr->size(); }

void BagItemButton::onSelect() { background.setColor(sf::Color(40, 230, 140)); }

void BagItemButton::onDeselect() {
    background.setColor(item.id != core::item::Id::None ? sf::Color::White :
                                                          sf::Color(240, 40, 40));
}

} // namespace menu
} // namespace game
