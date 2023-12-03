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

PeopledexRow::PeopledexRow(core::pplmn::Id ppl, const core::player::Peopledex& dex)
: ppl(ppl)
, dex(dex) {
    getSignal(Item::Selected).willAlwaysCall(std::bind(&PeopledexRow::makeActive, this));
    getSignal(Item::Deselected).willAlwaysCall(std::bind(&PeopledexRow::makeInactive, this));
}

glm::vec2 PeopledexRow::getSize() const { return bgndTxtr->size(); }

bl::com::Transform2D& PeopledexRow::doCreate(bl::engine::Engine& engine, bl::ecs::Entity parent) {
    const auto intel  = dex.getIntelLevel(ppl);
    const bool caught = dex.getCaught(ppl) > 0;

    const std::string& ImgPath = core::Properties::MenuImagePath();
    auto& jp                   = bl::util::FileUtil::joinPath;

    bgndTxtr = engine.renderer().texturePool().getOrLoadTexture(jp(ImgPath, "Peopledex/item.png"));
    activeBgndTxtr =
        engine.renderer().texturePool().getOrLoadTexture(jp(ImgPath, "Peopledex/itemActive.png"));
    background.create(engine, bgndTxtr);
    background.setParent(parent);
    const float mid = static_cast<float>(bgndTxtr->size().y) * 0.5f;

    ballTxtr = engine.renderer().texturePool().getOrLoadTexture(
        jp(core::Properties::ImagePath(), "item.png"));
    constexpr float BallX = 26.f;
    const float bw        = static_cast<float>(ballTxtr->size().x) * 0.5f;
    if (caught) {
        ball.create(engine, ballTxtr);
        ball.getTransform().setOrigin(ballTxtr->size() * 0.5f);
        ball.getTransform().setPosition({BallX, mid});
        ball.setParent(background);
    }

    id.create(engine, core::Properties::MenuFont(), toString(ppl), 14, sf::Color::Black);
    id.getTransform().setPosition(bw + BallX, mid);
    id.getTransform().setOrigin(0.f,
                                id.getLocalBounds().height * 0.5f + id.getLocalBounds().top * 0.5f);
    id.setParent(background);

    name.create(engine,
                core::Properties::MenuFont(),
                intel != core::player::Peopledex::NoIntel ? core::pplmn::Peoplemon::name(ppl) :
                                                            "???",
                24,
                sf::Color::Black);
    name.getTransform().setOrigin(
        0.f, name.getLocalBounds().height * 0.5f + name.getLocalBounds().top * 0.5f);
    name.getTransform().setPosition(id.getTransform().getLocalPosition().x + 65.f, mid);
    name.setParent(background);

    return background.getTransform();
}

void PeopledexRow::doSceneAdd(bl::rc::Overlay* overlay) {
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    id.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    name.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    if (ball.entity() != bl::ecs::InvalidEntity) {
        ball.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    }
}

void PeopledexRow::doSceneRemove() { background.removeFromScene(); }

bl::ecs::Entity PeopledexRow::getEntity() const { return background.entity(); }

void PeopledexRow::makeActive() { background.setTexture(activeBgndTxtr); }

void PeopledexRow::makeInactive() { background.setTexture(bgndTxtr); }

} // namespace menu
} // namespace game
