#include <Game/Menus/StorageCursor.hpp>

#include <BLIB/Components/Velocity2D.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Player/StorageSystem.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/Menus/StorageGrid.hpp>

namespace game
{
namespace menu
{
float StorageCursor::size = 40.f;
namespace
{
constexpr float MoveTime    = 0.3f;
constexpr float RotateRate  = 360.f;
constexpr float FlashPeriod = 0.4f;

glm::vec2 moveVector(core::input::EntityControl dir) {
    switch (dir) {
    case core::input::Control::MoveRight:
        return {1.f, 0.f};
    case core::input::Control::MoveDown:
        return {0.f, 1.f};
    case core::input::Control::MoveLeft:
        return {-1.f, 0.f};
    case core::input::Control::MoveUp:
        return {0.f, -1.f};
    default:
        return {0.f, 0.f};
    }
}
} // namespace

StorageCursor::StorageCursor(bl::engine::Engine& engine)
: engine(engine)
, position(0, 0)
, moveDir(core::input::Control::EntityControl::None)
, pplInScene(false) {
    cursorTxtr = engine.renderer().texturePool().getOrLoadTexture(bl::util::FileUtil::joinPath(
        core::Properties::MenuImagePath(), "StorageSystem/storageCursor.png"));
    cursor.create(engine, cursorTxtr);
    cursor.getTransform().setDepth(bl::cam::OverlayCamera::MinDepth + 1.f);

    size    = cursorTxtr->size().x;
    moveVel = size / MoveTime;
    offset  = 0.f;
}

void StorageCursor::activate(bl::ecs::Entity parent) {
    cursor.setParent(parent);
    bl::rc::Scene* overlay = engine.renderer().getObserver().getCurrentOverlay();
    cursor.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);
    if (peoplemon.entity() != bl::ecs::InvalidEntity) {
        peoplemon.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);
        pplInScene = true;
    }
    else {
        pplInScene = false;
        cursor.flash(FlashPeriod, FlashPeriod);
    }
    syncPos();
}

void StorageCursor::deactivate() {
    cursor.removeFromScene();
    if (peoplemon.entity() != bl::ecs::InvalidEntity) { peoplemon.removeFromScene(); }
    pplInScene = false;
}

void StorageCursor::setHidden(bool hide) {
    if (hide) {
        cursor.stopFlashing();
        cursor.setHidden(true);
    }
    else if (!pplInScene && !engine.ecs().hasComponent<bl::com::Toggler>(cursor.entity())) {
        cursor.flash(FlashPeriod, FlashPeriod);
    }
}

void StorageCursor::update(float dt) {
    if (pplInScene) { peoplemon.getTransform().rotate(RotateRate * dt); }

    switch (moveDir) {
    case core::input::Control::MoveRight:
    case core::input::Control::MoveDown:
    case core::input::Control::MoveLeft:
    case core::input::Control::MoveUp:
        offset += moveVel * dt;
        cursor.getTransform().move(moveVector(moveDir) * moveVel * dt);
        if (offset >= size) {
            moveDir = core::input::Control::EntityControl::None;
            syncPos();
        }
        break;

    default:
        break;
    }
}

bool StorageCursor::process(core::input::EntityControl cmd, bool skip) {
    if (moving()) {
        if (!skip) return false;

        // finish in-progress move
        syncPos();
    }

    moveDir = cmd;
    offset  = 0.f;
    switch (cmd) {
    case core::input::Control::MoveUp:
        if (position.y > 0) {
            --position.y;
            return true;
        }
        return false;

    case core::input::Control::MoveRight:
        if (position.x < core::player::StorageSystem::BoxWidth - 1) {
            ++position.x;
            return true;
        }
        return false;

    case core::input::Control::MoveDown:
        if (position.y < core::player::StorageSystem::BoxHeight - 1) {
            ++position.y;
            return true;
        }
        return false;

    case core::input::Control::MoveLeft:
        if (position.x > 0) {
            --position.x;
            return true;
        }
        return false;

    default:
        moveDir = core::input::Control::EntityControl::None;
        return false;
    }
}

void StorageCursor::setHolding(core::pplmn::Id ppl) {
    if (ppl != core::pplmn::Id::Unknown) {
        pplTxtr = engine.renderer().texturePool().getOrLoadTexture(
            core::pplmn::Peoplemon::thumbnailImage(ppl));

        if (peoplemon.entity() == bl::ecs::InvalidEntity) {
            peoplemon.create(engine, pplTxtr);
            peoplemon.getTransform().setOrigin(pplTxtr->size() * 0.5f);
            peoplemon.getTransform().setPosition(size * 0.5f, size * 0.5f);
            peoplemon.setParent(cursor);
        }
        else { peoplemon.setTexture(pplTxtr); }

        if (!pplInScene) {
            pplInScene = true;
            peoplemon.addToScene(engine.renderer().getObserver().getCurrentOverlay(),
                                 bl::rc::UpdateSpeed::Dynamic);
        }

        peoplemon.getTransform().setRotation(0.f);
        peoplemon.scaleToSize({size, size});
        cursor.stopFlashing();
    }
    else if (peoplemon.entity() != bl::ecs::InvalidEntity) {
        peoplemon.removeFromScene();
        pplInScene = false;
        cursor.flash(FlashPeriod, FlashPeriod);
    }
}

bool StorageCursor::moving() const { return moveDir != core::input::Control::EntityControl::None; }

const sf::Vector2i& StorageCursor::getPosition() const { return position; }

void StorageCursor::setX(int x) {
    position.x = x;
    syncPos();
}

void StorageCursor::pageLeft() {
    position.x = core::player::StorageSystem::BoxWidth - 1;
    syncPos();
}

void StorageCursor::pageRight() {
    position.x = 0;
    syncPos();
}

void StorageCursor::syncPos() { cursor.getTransform().setPosition(makePos()); }

glm::vec2 StorageCursor::makePos() const {
    return glm::vec2(position.x, position.y) * size + menu::StorageGrid::BoxPosition;
}

float StorageCursor::TileSize() { return size; }

} // namespace menu
} // namespace game
