#include <Game/Menus/MoveInfoRow.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace menu
{
bl::menu::Item::Ptr MoveInfoRow::create(core::pplmn::MoveId move) {
    return Ptr(new MoveInfoRow(move));
}

MoveInfoRow::MoveInfoRow(core::pplmn::MoveId move)
: move(move) {
    getSignal(Selected).willAlwaysCall(std::bind(&MoveInfoRow::makeActive, this));
    getSignal(Deselected).willAlwaysCall(std::bind(&MoveInfoRow::makeInactive, this));
}

glm::vec2 MoveInfoRow::getSize() const { return bgndTxtr->size(); }

bl::com::Transform2D& MoveInfoRow::doCreate(bl::engine::Engine& engine, bl::ecs::Entity parent) {
    const auto& joinPath      = bl::util::FileUtil::joinPath;
    const std::string ImgPath = joinPath(core::Properties::MenuImagePath(), "PplInfo");

    bgndTxtr = engine.renderer().texturePool().getOrLoadTexture(joinPath(ImgPath, "move.png"));
    activeBgndTxtr =
        engine.renderer().texturePool().getOrLoadTexture(joinPath(ImgPath, "moveActive.png"));
    background.create(engine, bgndTxtr);
    background.setParent(parent);

    name.create(engine,
                core::Properties::MenuFont(),
                core::pplmn::Move::name(move),
                26,
                bl::sfcol(sf::Color(30, 50, 65)));
    name.getTransform().setPosition(getSize() * 0.5f);
    name.getTransform().setOrigin(name.getLocalBounds().width * 0.5f,
                                  name.getLocalBounds().height * 0.5f);
    name.setParent(background);

    return background.getTransform();
}

void MoveInfoRow::doSceneAdd(bl::rc::Overlay* overlay) {
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    name.addToScene(overlay, bl::rc::UpdateSpeed::Static);
}

void MoveInfoRow::doSceneRemove() { background.removeFromScene(); }

bl::ecs::Entity MoveInfoRow::getEntity() const { return background.entity(); }

void MoveInfoRow::makeActive() { background.setTexture(activeBgndTxtr); }

void MoveInfoRow::makeInactive() { background.setTexture(bgndTxtr); }

} // namespace menu
} // namespace game
