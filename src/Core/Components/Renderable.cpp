#include <Core/Components/Renderable.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Files/Util.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
Renderable Renderable::fromSprite(
    const bl::entity::Registry::ComponentHandle<component::Position>& pos,
    const std::string& file) {
    Renderable rc(pos);
    StaticSprite& spr = *new StaticSprite();
    rc.data.reset(&spr);

    spr.texture = bl::engine::Resources::textures().load(file).data;
    spr.sprite.setTexture(*spr.texture, true);
    spr.sprite.setOrigin(spr.texture->getSize().x, spr.texture->getSize().y);

    return rc;
}

Renderable Renderable::fromMoveAnims(
    const bl::entity::Registry::ComponentHandle<component::Position>& pos,
    const bl::entity::Registry::ComponentHandle<component::Movable>& movable,
    const std::string& path) {
    Renderable rc(pos);
    MoveAnims& mv = *new MoveAnims(movable);
    rc.data.reset(&mv);

    mv.data[0] =
        bl::engine::Resources::animations().load(bl::file::Util::joinPath(path, "up.anim")).data;
    mv.data[1] =
        bl::engine::Resources::animations().load(bl::file::Util::joinPath(path, "right.anim")).data;
    mv.data[2] =
        bl::engine::Resources::animations().load(bl::file::Util::joinPath(path, "down.anim")).data;
    mv.data[3] =
        bl::engine::Resources::animations().load(bl::file::Util::joinPath(path, "left.anim")).data;

    mv.anim.setData(*mv.data[0]);
    mv.anim.setIsCentered(false);

    return rc;
}

Renderable Renderable::fromFastMoveAnims(
    const bl::entity::Registry::ComponentHandle<component::Position>& pos,
    const bl::entity::Registry::ComponentHandle<component::Movable>& movable,
    const std::string& path) {
    Renderable rc(pos);
    FastMoveAnims& mv = *new FastMoveAnims(movable);
    rc.data.reset(&mv);

    const std::string walkPath = bl::file::Util::joinPath(path, "Walk");
    const std::string runPath  = bl::file::Util::joinPath(path, "Run");

    mv.walk[0] = bl::engine::Resources::animations()
                     .load(bl::file::Util::joinPath(walkPath, "up.anim"))
                     .data;
    mv.walk[1] = bl::engine::Resources::animations()
                     .load(bl::file::Util::joinPath(walkPath, "right.anim"))
                     .data;
    mv.walk[2] = bl::engine::Resources::animations()
                     .load(bl::file::Util::joinPath(walkPath, "down.anim"))
                     .data;
    mv.walk[3] = bl::engine::Resources::animations()
                     .load(bl::file::Util::joinPath(walkPath, "left.anim"))
                     .data;

    mv.run[0] =
        bl::engine::Resources::animations().load(bl::file::Util::joinPath(runPath, "up.anim")).data;
    mv.run[1] = bl::engine::Resources::animations()
                    .load(bl::file::Util::joinPath(runPath, "right.anim"))
                    .data;
    mv.run[2] = bl::engine::Resources::animations()
                    .load(bl::file::Util::joinPath(runPath, "down.anim"))
                    .data;
    mv.run[3] = bl::engine::Resources::animations()
                    .load(bl::file::Util::joinPath(runPath, "left.anim"))
                    .data;

    mv.anim.setData(*mv.walk[0]);
    mv.anim.setIsCentered(false);

    return rc;
}

Renderable::Renderable(const bl::entity::Registry::ComponentHandle<component::Position>& pos)
: position(pos) {}

void Renderable::update(float dt) { data->update(dt, position); }

void Renderable::render(sf::RenderTarget& target, float lag) const {
    // Add one tile bc all origins are bottom right corner (handles odd size sprites/anims great)
    const sf::Vector2f pos(position.get().positionPixels() +
                           sf::Vector2f(static_cast<float>(Properties::PixelsPerTile()),
                                        static_cast<float>(Properties::PixelsPerTile())));
    data->render(target, lag, pos);
}

void Renderable::StaticSprite::render(sf::RenderTarget& target, float, const sf::Vector2f& pos) {
    sprite.setPosition(pos);
    target.draw(sprite);
}

Renderable::MoveAnims::MoveAnims(
    const bl::entity::Registry::ComponentHandle<component::Movable>& movable)
: movable(movable) {}

void Renderable::MoveAnims::update(
    float dt, const bl::entity::Registry::ComponentHandle<component::Position>& pos) {
    anim.setData(*data[static_cast<unsigned int>(pos.get().direction)]);
    anim.update(dt);
    if (movable.get().moving())
        anim.play(false);
    else
        anim.stop();
}

void Renderable::MoveAnims::render(sf::RenderTarget& target, float lag, const sf::Vector2f& pos) {
    const sf::Vector2f offset =
        anim.getData().frameCount() > 0 ? anim.getData().getFrameSize(0) : sf::Vector2f(0.f, 0.f);
    anim.setPosition(pos - offset);
    anim.render(target, lag);
}

Renderable::FastMoveAnims::FastMoveAnims(
    const bl::entity::Registry::ComponentHandle<component::Movable>& movable)
: movable(movable) {}

void Renderable::FastMoveAnims::update(
    float dt, const bl::entity::Registry::ComponentHandle<component::Position>& pos) {
    anim.setData(*walk[static_cast<unsigned int>(pos.get().direction)]);
    anim.update(dt);
    if (movable.get().moving()) {
        if (movable.get().goingFast()) {
            anim.setData(*run[static_cast<unsigned int>(pos.get().direction)]);
        }
        anim.play(false);
    }
    else
        anim.stop();
}

void Renderable::FastMoveAnims::render(sf::RenderTarget& target, float lag,
                                       const sf::Vector2f& pos) {
    const sf::Vector2f offset =
        anim.getData().frameCount() > 0 ? anim.getData().getFrameSize(0) : sf::Vector2f(0.f, 0.f);
    anim.setPosition(pos - offset);
    anim.render(target, lag);
}

} // namespace component
} // namespace core
