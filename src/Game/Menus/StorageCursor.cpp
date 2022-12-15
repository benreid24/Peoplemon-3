#include <Game/Menus/StorageCursor.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Player/StorageSystem.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace menu
{
float StorageCursor::size = 40.f;
namespace
{
constexpr float MoveTime   = 0.3f;
constexpr float RotateRate = 360.f;

sf::Vector2f moveVector(core::input::EntityControl dir) {
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

StorageCursor::StorageCursor()
: position(0, 0)
, flasher(cursor, 0.3f, 0.3f)
, moveDir(core::input::Control::EntityControl::None) {
    cursorTxtr = TextureManager::load(bl::util::FileUtil::joinPath(
        core::Properties::MenuImagePath(), "StorageSystem/storageCursor.png"));
    cursor.setTexture(*cursorTxtr, true);

    size    = static_cast<float>(cursorTxtr->getSize().x);
    moveVel = size / MoveTime;
    offset  = 0.f;
}

void StorageCursor::update(float dt) {
    flasher.update(dt);
    peoplemon.rotate(RotateRate * dt);

    switch (moveDir) {
    case core::input::Control::MoveRight:
    case core::input::Control::MoveDown:
    case core::input::Control::MoveLeft:
    case core::input::Control::MoveUp:
        offset += moveVel * dt;
        if (offset >= size) {
            moveDir = core::input::Control::EntityControl::None;
            syncPos();
        }
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
        pplTxtr = TextureManager::load(core::pplmn::Peoplemon::thumbnailImage(ppl));
        peoplemon.setTexture(*pplTxtr, true);
        const sf::Vector2f pplSize(pplTxtr->getSize());
        peoplemon.setOrigin(pplSize * 0.5f);
        peoplemon.setPosition(size * 0.5f, size * 0.5f);
        peoplemon.setRotation(0.f);
        peoplemon.setScale(size / pplSize.x, size / pplSize.y);
    }
    else { pplTxtr.release(); }
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

void StorageCursor::render(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.transform.translate(moveVector(moveDir) * offset);
    flasher.render(target, states, 0.f);
    if (pplTxtr) {
        states.transform.translate(cursor.getPosition());
        target.draw(peoplemon, states);
    }
}

void StorageCursor::syncPos() { cursor.setPosition(sf::Vector2f(position) * size); }

float StorageCursor::TileSize() { return size; }

} // namespace menu
} // namespace game
