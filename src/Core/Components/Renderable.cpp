#include <Core/Components/Renderable.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
Renderable Renderable::fromSprite(const Position& pos, const std::string& file) {
    Renderable rc(pos);
    rc.data.emplace<StaticSprite>();
    StaticSprite& spr = *std::get_if<StaticSprite>(&rc.data);

    spr.texture = bl::engine::Resources::textures().load(file).data;
    spr.sprite.setTexture(*spr.texture, true);
    spr.sprite.setOrigin(spr.texture->getSize().x, spr.texture->getSize().y);

    return rc;
}

Renderable Renderable::fromMoveAnims(const Position& pos, Movable& movable,
                                     const std::string& path) {
    Renderable rc(pos);
    rc.data.emplace<MoveAnims>(movable);
    MoveAnims& mv = *std::get_if<MoveAnims>(&rc.data);

    mv.data[0] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(path, "up.anim"))
                     .data;
    mv.data[1] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(path, "right.anim"))
                     .data;
    mv.data[2] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(path, "down.anim"))
                     .data;
    mv.data[3] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(path, "left.anim"))
                     .data;

    mv.anim.setData(*mv.data[0]);
    rc.update(0.f);

    return rc;
}

Renderable Renderable::fromFastMoveAnims(const Position& pos, Movable& movable,
                                         const std::string& path) {
    Renderable rc(pos);
    rc.data.emplace<FastMoveAnims>(movable);
    FastMoveAnims& mv = *std::get_if<FastMoveAnims>(&rc.data);

    const std::string walkPath = bl::util::FileUtil::joinPath(path, "Walk");
    const std::string runPath  = bl::util::FileUtil::joinPath(path, "Run");

    mv.walk[0] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(walkPath, "up.anim"))
                     .data;
    mv.walk[1] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(walkPath, "right.anim"))
                     .data;
    mv.walk[2] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(walkPath, "down.anim"))
                     .data;
    mv.walk[3] = bl::engine::Resources::animations()
                     .load(bl::util::FileUtil::joinPath(walkPath, "left.anim"))
                     .data;

    mv.run[0] = bl::engine::Resources::animations()
                    .load(bl::util::FileUtil::joinPath(runPath, "up.anim"))
                    .data;
    mv.run[1] = bl::engine::Resources::animations()
                    .load(bl::util::FileUtil::joinPath(runPath, "right.anim"))
                    .data;
    mv.run[2] = bl::engine::Resources::animations()
                    .load(bl::util::FileUtil::joinPath(runPath, "down.anim"))
                    .data;
    mv.run[3] = bl::engine::Resources::animations()
                    .load(bl::util::FileUtil::joinPath(runPath, "left.anim"))
                    .data;

    mv.anim.setData(*mv.walk[0]);
    rc.update(0.f);

    return rc;
}

Renderable Renderable::fromAnimation(const Position& pos, const std::string& path) {
    Renderable rc(pos);
    rc.data.emplace<OneAnimation>(path);
    return rc;
}

Renderable::Renderable(const Position& pos)
: position(pos)
, shadowHeight(-1.f) {
    shadow.setFillColor(sf::Color(0, 0, 0, 145));
}

void Renderable::update(float dt) { cur()->update(dt, position); }

void Renderable::render(sf::RenderTarget& target, float lag) const {
    // Add one tile bc all origins are bottom right corner (handles odd size sprites/anims great)
    static const sf::Vector2f offset(static_cast<float>(Properties::PixelsPerTile()),
                                     static_cast<float>(Properties::PixelsPerTile()));
    const sf::Vector2f pos(position.positionPixels() + offset);
    if (shadowHeight >= 0.f) {
        sf::RenderStates states;
        states.transform.translate(pos);
        states.transform.translate(-offset.x * 0.5f, shadowHeight);
        target.draw(shadow, states);
    }
    cur()->render(target, lag, pos);
}

void Renderable::setAngle(float a) { cur()->setAngle(a); }

void Renderable::updateShadow(float height, float rad) {
    shadow.setRadius(rad);
    shadow.setOrigin({rad, rad});
    shadowHeight = height;
}

void Renderable::removeShadow() { shadowHeight = -1.f; }

Renderable::Base* Renderable::cur() {
    switch (data.index()) {
    case 0:
        return std::get_if<StaticSprite>(&data);
    case 1:
        return std::get_if<MoveAnims>(&data);
    case 2:
        return std::get_if<FastMoveAnims>(&data);
    case 3:
        return std::get_if<OneAnimation>(&data);
    default:
        BL_LOG_CRITICAL << "Invalid Renderable index: " << data.index();
        return nullptr;
    }
}

Renderable::Base* Renderable::cur() const { return const_cast<Renderable*>(this)->cur(); }

float Renderable::animLength() const { return cur()->length(); }

void Renderable::triggerAnim(bool loop) { cur()->trigger(loop); }

void Renderable::StaticSprite::render(sf::RenderTarget& target, float, const sf::Vector2f& pos) {
    sprite.setPosition(pos);
    target.draw(sprite);
}

float Renderable::StaticSprite::length() const { return 0.f; }

void Renderable::StaticSprite::trigger(bool) {}

void Renderable::StaticSprite::setAngle(float a) { sprite.setRotation(a); }

Renderable::MoveAnims::MoveAnims(Movable& movable)
: movable(movable) {}

void Renderable::MoveAnims::update(float dt, const Position& pos) {
    anim.setData(*data[static_cast<unsigned int>(pos.direction)]);
    anim.update(dt);
    if (movable.moving())
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

float Renderable::MoveAnims::length() const { return anim.getData().getLength(); }

void Renderable::MoveAnims::trigger(bool loop) {
    anim.setIsLoop(loop);
    anim.play();
}

void Renderable::MoveAnims::setAngle(float a) { anim.setRotation(a); }

Renderable::FastMoveAnims::FastMoveAnims(Movable& movable)
: movable(movable) {}

void Renderable::FastMoveAnims::update(float dt, const Position& pos) {
    if (movable.moving()) {
        if (movable.goingFast()) {
            auto& src = *run[static_cast<unsigned int>(pos.direction)];
            anim.setData(src);
        }
        else {
            auto& src = *walk[static_cast<unsigned int>(pos.direction)];
            anim.setData(src);
        }
        anim.play(false);
    }
    else {
        auto& src = *walk[static_cast<unsigned int>(pos.direction)];
        anim.setData(src);
        anim.stop();
    }
    anim.update(dt);
}

void Renderable::FastMoveAnims::render(sf::RenderTarget& target, float lag,
                                       const sf::Vector2f& pos) {
    const sf::Vector2f offset =
        anim.getData().frameCount() > 0 ? anim.getData().getFrameSize(0) : sf::Vector2f(0.f, 0.f);
    anim.setPosition(pos - offset);
    anim.render(target, lag);
}

float Renderable::FastMoveAnims::length() const { return anim.getData().getLength(); }

void Renderable::FastMoveAnims::trigger(bool loop) {
    anim.setIsLoop(loop);
    anim.play();
}

void Renderable::FastMoveAnims::setAngle(float a) { anim.setRotation(a); }

Renderable::OneAnimation::OneAnimation(const std::string& path) {
    src = bl::engine::Resources::animations()
              .load(bl::util::FileUtil::joinPath(Properties::AnimationPath(), path))
              .data;
    if (!src) {
        BL_LOG_WARN << "Failed to load animation: " << path;
        return;
    }

    anim.setData(*src);
    offset =
        anim.getData().frameCount() > 0 ? anim.getData().getFrameSize(0) : sf::Vector2f(0.f, 0.f);
}

void Renderable::OneAnimation::update(float dt, const Position&) { anim.update(dt); }

void Renderable::OneAnimation::render(sf::RenderTarget& target, float lag,
                                      const sf::Vector2f& pos) {
    anim.setPosition(pos - offset);
    anim.render(target, lag);
}

float Renderable::OneAnimation::length() const { return src->getLength(); }

void Renderable::OneAnimation::trigger(bool loop) {
    anim.setIsLoop(loop);
    anim.play();
}

void Renderable::OneAnimation::setAngle(float a) { anim.setRotation(a); }

} // namespace component
} // namespace core
