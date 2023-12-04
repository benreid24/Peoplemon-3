#include <Game/Menus/PeoplemonButton.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace menu
{
namespace
{
constexpr float GrowRate    = 100.f;
constexpr float HpBarWidth  = 178.f;
constexpr float HpBarHeight = 12.f;
} // namespace

PeoplemonButton::Ptr PeoplemonButton::create(const core::pplmn::OwnedPeoplemon& ppl) {
    return Ptr(new PeoplemonButton(ppl));
}

PeoplemonButton::PeoplemonButton(const core::pplmn::OwnedPeoplemon& ppl)
: ppl(ppl)
, enginePtr(nullptr)
, overlay(nullptr)
, color(sf::Color::White) {
    getSignal(bl::menu::Item::Selected).willAlwaysCall([this]() {
        image.setColor(sf::Color(170, 190, 30));
    });
    getSignal(bl::menu::Item::Deselected).willAlwaysCall([this]() { image.setColor(color); });
}

void PeoplemonButton::setHighlightColor(const sf::Color& c) {
    color = c;
    if (!isSelected) { image.setColor(color); }
}

glm::vec2 PeoplemonButton::getSize() const { return txtr->size(); }

void PeoplemonButton::sync(const core::pplmn::OwnedPeoplemon& ppl) {
    const unsigned int curHp = ppl.currentHp();
    const unsigned int maxHp = ppl.currentStats().hp;
    const float hpPercent    = static_cast<float>(curHp) / static_cast<float>(maxHp);
    hpText.getSection().setString(std::to_string(curHp) + " / " + std::to_string(maxHp));
    hpText.getTransform().setPosition(342.f - hpText.getLocalBounds().width - 8.f, 83.f);
    hpBarTarget = hpPercent * HpBarWidth;

    updateAilment(ppl.currentAilment());
}

bool PeoplemonButton::synced() const {
    return std::abs(hpBar.getGlobalSize().x - hpBarTarget) < 0.5f;
}

bl::com::Transform2D& PeoplemonButton::doCreate(bl::engine::Engine& engine,
                                                bl::ecs::Entity parent) {
    enginePtr = &engine;

    txtr = engine.renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "Peoplemon/button.png"));
    image.create(engine, txtr);
    image.setParent(parent);

    name.create(engine,
                core::Properties::MenuFont(),
                ppl.name(),
                22,
                sf::Color(60, 225, 200),
                sf::Text::Bold);
    name.getTransform().setPosition(135.f, 18.f);
    name.setParent(image);

    const unsigned int curHp = ppl.currentHp();
    const unsigned int maxHp = ppl.currentStats().hp;
    const float hpPercent    = static_cast<float>(curHp) / static_cast<float>(maxHp);
    const float hpBarWidth   = hpPercent * HpBarWidth;
    const sf::Color hpColor  = core::Properties::HPBarColor(curHp, maxHp);

    hpBar.create(engine, {100.f, 12.f});
    hpBar.getTransform().setPosition(164.f, 70.f);
    hpBar.setFillColor(hpColor);
    hpBar.scaleToSize({hpBarWidth, HpBarHeight});
    hpBar.setParent(image);
    hpBar.getTransform().setDepth(1.f);
    hpBarTarget = hpBarWidth;

    hpText.create(engine,
                  core::Properties::MenuFont(),
                  std::to_string(curHp) + " / " + std::to_string(maxHp),
                  18,
                  sf::Color::Black);
    hpText.getTransform().setPosition(342.f - hpText.getLocalBounds().width - 8.f, 83.f);
    hpText.setParent(image);

    item.create(engine,
                core::Properties::MenuFont(),
                ppl.holdItem() != core::item::Id::None ? core::item::Item::getName(ppl.holdItem()) :
                                                         "No hold item",
                16,
                sf::Color(30, 230, 160));
    item.getTransform().setPosition(85.f, 136.f);
    item.setParent(image);

    level.create(engine,
                 core::Properties::MenuFont(),
                 std::to_string(ppl.currentLevel()),
                 16,
                 sf::Color(50, 220, 250),
                 sf::Text::Bold);
    level.getTransform().setPosition(304.f, 16.f);
    level.setParent(image);

    updateAilment(ppl.currentAilment());

    const std::string faceSrc = core::pplmn::Peoplemon::thumbnailImage(ppl.id());
    faceTxtr                  = engine.renderer().texturePool().getOrLoadTexture(faceSrc);
    face.create(engine, faceTxtr);
    face.setParent(image);

    sf::Rect<unsigned int> bounds(100000, 100000, 0, 0);
    const auto faceRes   = ImageManager::load(faceSrc);
    const sf::Image& img = *faceRes;
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
    face.getTransform().setOrigin(faceTxtr->size() * scale * 0.5f);
    face.getTransform().setScale(scale, scale);
    face.getTransform().setPosition(71.f, 76.f);

    return image.getTransform();
}

void PeoplemonButton::doSceneAdd(bl::rc::Overlay* o) {
    overlay = o;

    image.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    face.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hpBar.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    name.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    level.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    item.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hpText.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    if (ailment.entity() != bl::ecs::InvalidEntity) {
        ailment.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    }
}

void PeoplemonButton::doSceneRemove() { image.removeFromScene(); }

bl::ecs::Entity PeoplemonButton::getEntity() const { return image.entity(); }

void PeoplemonButton::update(float dt) {
    if (hpBar.getGlobalSize().x < hpBarTarget) {
        const float nw = std::min(hpBarTarget, hpBar.getGlobalSize().x + GrowRate * dt);
        hpBar.scaleToSize({nw, HpBarHeight});
        hpBar.setFillColor(core::Properties::HPBarColor(nw / HpBarWidth));
    }
    else if (hpBar.getGlobalSize().x > hpBarTarget) {
        const float nw = std::max(hpBarTarget, hpBar.getGlobalSize().x - GrowRate * dt);
        hpBar.scaleToSize({nw, HpBarHeight});
        hpBar.setFillColor(core::Properties::HPBarColor(nw / HpBarWidth));
    }
}

void PeoplemonButton::updateAilment(core::pplmn::Ailment ail) {
    const std::string ailSrc = core::Properties::AilmentTexture(ppl.currentAilment());
    if (!ailSrc.empty()) {
        ailTxtr = enginePtr->renderer().texturePool().getOrLoadTexture(ailSrc);
        if (ailment.entity() == bl::ecs::InvalidEntity) {
            ailment.create(*enginePtr, ailTxtr);
            ailment.getTransform().setPosition(135.f, 90.f);
            ailment.setParent(image);
        }
        else if (ailment.getTexture().get() != ailTxtr.get()) { ailment.setTexture(ailTxtr, {}); }
        if (overlay) { ailment.addToScene(overlay, bl::rc::UpdateSpeed::Static); }
    }
    else {
        if (ailment.entity() != bl::ecs::InvalidEntity) { ailment.destroy(); }
    }
}

} // namespace menu
} // namespace game
