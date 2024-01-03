#include <Core/Components/Renderable.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace component
{
Renderable& Renderable::fromSprite(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                   bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = Sprite;

    bl::gfx::Sprite sprite;
    sprite.create(engine, entity, engine.renderer().texturePool().getOrLoadTexture(path));
    sprite.deleteEntityOnDestroy(false);
    sprite.getTransform().setOrigin(sprite.getTexture()->size());
    sprite.addToScene(scene, bl::rc::UpdateSpeed::Static);
    rc.transform = &sprite.getTransform();

    return rc;
}

Renderable& Renderable::fromMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                      bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = Walk;

    bl::resource::Ref<res::WalkAnimations> data = WalkAnimationManager::getOrCreateGenerated(path);
    bl::gfx::Slideshow slideshow;
    slideshow.createWithUniquePlayer(engine, entity, data);
    slideshow.deleteEntityOnDestroy(false);
    slideshow.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);
    rc.transform = &slideshow.getTransform();
    rc.player    = &slideshow.getPlayer();
    rc.walkSrc   = data.get();

    return rc;
}

Renderable& Renderable::fromFastMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                          bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = Run;

    bl::resource::Ref<res::RunWalkAnimations> data =
        RunWalkAnimationManager::getOrCreateGenerated(path);
    bl::gfx::Slideshow slideshow;
    slideshow.createWithUniquePlayer(engine, entity, data);
    slideshow.deleteEntityOnDestroy(false);
    slideshow.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);
    rc.transform = &slideshow.getTransform();
    rc.player    = &slideshow.getPlayer();
    rc.runSrc    = data.get();

    return rc;
}

Renderable& Renderable::fromAnimation(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                      bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = SingleAnim;

    bl::resource::Ref<bl::gfx::a2d::AnimationData> data = AnimationManager::load(path);
    rc.animSrc                                          = data.get();
    if (data->isSlideshow()) {
        bl::gfx::Slideshow slideshow;
        slideshow.createWithUniquePlayer(engine, entity, data);
        slideshow.deleteEntityOnDestroy(false);
        slideshow.addToScene(scene, bl::rc::UpdateSpeed::Static);
        rc.transform = &slideshow.getTransform();
        rc.player    = &slideshow.getPlayer();
    }
    else {
        bl::gfx::Animation2D anim;
        anim.createWithUniquePlayer(engine, entity, data);
        anim.deleteEntityOnDestroy(false);
        anim.addToScene(scene, bl::rc::UpdateSpeed::Static);
        rc.transform = &anim.getTransform();
        rc.player    = &anim.getPlayer();
    }

    return rc;
}

Renderable::Renderable()
: srcType(Sprite)
, walkSrc(nullptr)
, transform(nullptr)
, player(nullptr)
, shadow(bl::ecs::InvalidEntity)
, shadowHeight(0.f) {}

// void Renderable::render(sf::RenderTarget& target, float lag) const {
//     // Add one tile bc all origins are bottom right corner (handles odd size sprites/anims great)
//     static const sf::Vector2f offset(static_cast<float>(Properties::PixelsPerTile()),
//                                      static_cast<float>(Properties::PixelsPerTile()));
//     const sf::Vector2f pos(position.positionPixels() + offset);
//     if (shadowHeight >= 0.f) {
//         sf::RenderStates states;
//         states.transform.translate(pos);
//         states.transform.translate(-offset.x * 0.5f, shadowHeight);
//         target.draw(shadow, states);
//     }
//     cur()->render(target, lag, pos);
// }

void Renderable::setAngle(float a) { transform->setRotation(a); }

void Renderable::updateShadow(float height, float rad) {
    /* shadow.setRadius(rad);
     shadow.setOrigin({rad, rad});*/
    shadowHeight = height;
}

void Renderable::removeShadow() { shadowHeight = -1.f; }

float Renderable::animLength() const {
    switch (srcType) {
    case Walk:
        return walkSrc->getMaxStateLength();
    case Run:
        return runSrc->getMaxStateLength();
    case SingleAnim:
        return animSrc->getLength();
    default:
        return 0.f;
    }
}

void Renderable::triggerAnim(bool loop) {
    if (player) {
        if (loop) { player->playLooping(); }
        else { player->play(); }
    }
}

void Renderable::notifyMoveState(bl::tmap::Direction dir, bool moving, bool running) {
    if (player) {
        if (srcType == Walk) {
            player->setState(res::WalkAnimations::getStateFromDirection(dir), false);
        }
        else if (srcType == Run) {
            player->setState(res::RunWalkAnimations::getStateFromDirection(dir, running));
        }
        if (moving) { player->playLooping(); }
        else { player->stop(); }
    }
}

// void Renderable::FastMoveAnims::render(sf::RenderTarget& target, float lag,
//                                        const sf::Vector2f& pos) {
//     const sf::Vector2f offset =
//         anim.getData().frameCount() > 0 ? anim.getData().getFrameSize(0) : sf::Vector2f(0.f,
//         0.f);
//     anim.setPosition(pos - offset);
//     anim.render(target, lag);
// }

} // namespace component
} // namespace core
