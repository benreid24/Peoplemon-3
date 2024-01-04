#include <Core/Components/Renderable.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace component
{
Renderable& Renderable::createFromSprite(bl::engine::Engine& engine, bl::ecs::Entity entity,
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

Renderable& Renderable::createFromMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                            bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = Walk;

    bl::resource::Ref<res::WalkAnimations> data = WalkAnimationManager::getOrCreateGenerated(path);
    bl::gfx::Slideshow slideshow;
    slideshow.createWithUniquePlayer(engine, entity, data);
    slideshow.deleteEntityOnDestroy(false);
    slideshow.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);
    slideshow.getTransform().setOrigin(
        slideshow.getLocalSize().x - static_cast<float>(Properties::PixelsPerTile()),
        slideshow.getLocalSize().y - static_cast<float>(Properties::PixelsPerTile()));
    rc.transform = &slideshow.getTransform();
    rc.player    = &slideshow.getPlayer();
    rc.walkSrc   = data.get();

    return rc;
}

Renderable& Renderable::createFromFastMoveAnims(bl::engine::Engine& engine, bl::ecs::Entity entity,
                                                bl::rc::Scene* scene, const std::string& path) {
    Renderable& rc = *engine.ecs().emplaceComponent<Renderable>(entity);
    rc.srcType     = Run;

    bl::resource::Ref<res::RunWalkAnimations> data =
        RunWalkAnimationManager::getOrCreateGenerated(path);
    bl::gfx::Slideshow slideshow;
    slideshow.createWithUniquePlayer(engine, entity, data);
    slideshow.deleteEntityOnDestroy(false);
    slideshow.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);
    slideshow.getTransform().setOrigin(
        slideshow.getLocalSize().x - static_cast<float>(Properties::PixelsPerTile()),
        slideshow.getLocalSize().y - static_cast<float>(Properties::PixelsPerTile()));
    rc.transform = &slideshow.getTransform();
    rc.player    = &slideshow.getPlayer();
    rc.runSrc    = data.get();

    return rc;
}

Renderable& Renderable::createFromAnimation(bl::engine::Engine& engine, bl::ecs::Entity entity,
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

} // namespace component
} // namespace core
