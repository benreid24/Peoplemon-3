#include <Core/Battles/View/PeoplemonAnimation.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <cmath>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
constexpr float PlayerPosX        = 88.f;
constexpr float PlayerPosY        = 265.f;
constexpr float OpponentPosX      = 508.f;
constexpr float OpponentPosY      = 25.f;
static constexpr float SquareSize = 200.f;
const glm::vec2 PlayerPos(PlayerPosX, PlayerPosY);
const glm::vec2 OpponentPos(OpponentPosX, OpponentPosY);
const glm::vec2 ViewSize(SquareSize, SquareSize);
constexpr float SlideRate               = 285.f;
constexpr float ShakesPerSecond         = 18.f;
constexpr float ShakeOffMultiple        = ShakesPerSecond * 360.f;
constexpr float ShakeXMag               = 10.f;
constexpr float ShakeYMag               = 5.f;
constexpr float ShakeTime               = 0.75f;
constexpr float ExpandRate              = 600.f;
constexpr float ContractRate            = 325.f;
constexpr float BallFlashRadius         = 200.f;
constexpr float ArrowOscillations       = 4.f;
constexpr float ArrowShowTime           = 2.5f;
constexpr float ArrowShakeAmount        = 10.f;
constexpr std::uint8_t ScreenFlashAlpha = 100;
constexpr float ThrowStartX             = -25.f;
constexpr float ThrowStartY             = PlayerPosY + SquareSize * 0.5f;
constexpr float ThrowEndX               = OpponentPosX + SquareSize * 0.5f;
constexpr float ThrowEndY               = OpponentPosY;
constexpr float ThrowTime               = 1.f;
constexpr float ThrowXVel               = (ThrowEndX - ThrowStartX) / ThrowTime;
constexpr float ThrowMiddle             = ThrowStartX + (ThrowEndX - ThrowStartX) * 0.75f;
constexpr float ThrowSecondHalfWidth    = ThrowEndX - ThrowMiddle;
constexpr float ThrowTop                = 10.f;
constexpr float ThrowA     = (ThrowEndY - ThrowTop) / (ThrowSecondHalfWidth * ThrowSecondHalfWidth);
constexpr float ThrowSpins = 3.f;
constexpr float ThrowSpinRate    = ThrowSpins / ThrowTime * 360.f;
constexpr float BallBounceHeight = ThrowEndY + SquareSize * 0.75f - OpponentPosY - 20.f;
constexpr float BallBounceTime   = 1.4f;
const glm::vec2 BallFinalPos(ThrowEndX, OpponentPosY + BallBounceHeight);
constexpr float BallShakeTime = 0.5f;
constexpr float ShakeRestTime = 0.65f;
constexpr float ShakeXAmount  = 10.f;
constexpr float ShakeYAmount  = 2.f;
constexpr float ShakeCount    = 6.f;
constexpr float ShakeSpeed    = 360.f * ShakeCount;
constexpr float GreenRate     = 255.f / 0.5f;
constexpr glm::vec3 FlashColor(252.f / 256.f, 230.f / 256.f, 30.f / 256.f);

constexpr float PeoplemonFlashOn  = 0.08f;
constexpr float PeoplemonFlashOff = 0.05f;

using Animation = cmd::Animation;

glm::vec4 makeColor(float alpha) { return glm::vec4(FlashColor, alpha / 256.f); }

VkViewport makeViewport(PeoplemonAnimation::Position pos) {
    const glm::vec2 corner = pos == PeoplemonAnimation::Player ? PlayerPos : OpponentPos;
    VkViewport vp{};
    vp.x        = corner.x;
    vp.y        = corner.y;
    vp.width    = ViewSize.x;
    vp.height   = ViewSize.y;
    vp.minDepth = 0.f;
    vp.maxDepth = 1.f;
    return vp;
}

glm::vec4 sfcol(const sf::Color& c) {
    return glm::vec4(static_cast<float>(c.r),
                     static_cast<float>(c.g),
                     static_cast<float>(c.b),
                     static_cast<float>(c.a)) /
           256.f;
}
} // namespace

PeoplemonAnimation::PeoplemonAnimation(bl::engine::Engine& engine, Position pos)
: engine(engine)
, position(pos)
, viewport(makeViewport(pos))
, renderBall(false) {}

void PeoplemonAnimation::init(bl::rc::scene::CodeScene* s) {
    const auto join = bl::util::FileUtil::joinPath;
    scene           = s;

    peoplemon.create(engine, engine.renderer().texturePool().getBlankTexture());
    peoplemon.getTransform().setPosition(ViewSize.x * 0.5f, ViewSize.y);
    peoplemon.addToScene(scene, bl::rc::UpdateSpeed::Static);

    ballOpenTxtr = engine.renderer().texturePool().getOrLoadTexture(
        join(Properties::ImagePath(), "Battle/Balls/peopleball_open.png"));
    // TODO - consider using multiple graphics based on what ball it was caught in
    ballTxtr = engine.renderer().texturePool().getOrLoadTexture(
        join(Properties::ImagePath(), "Battle/Balls/peopleball.png"));
    ball.create(engine, ballTxtr);
    setBallTexture(ballTxtr);
    ball.getTransform().setPosition(ViewSize.x * 0.5f, ViewSize.y * 0.75f);
    ball.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    ballFlash.create(engine, 100.f);
    ballFlash.getTransform().setPosition(ball.getTransform().getLocalPosition().x,
                                         ball.getTransform().getLocalPosition().y + 15.f);
    ballFlash.setColorGradient(glm::vec4(FlashColor, 1.f), sfcol(sf::Color::Transparent));
    ballFlash.addToScene(scene, bl::rc::UpdateSpeed::Static);

    statTxtr = engine.renderer().texturePool().getOrLoadTexture(
        join(Properties::ImagePath(), "Battle/statArrow.png"));
    statArrow.create(engine, statTxtr);
    statArrow.getTransform().setOrigin(statTxtr->size().x * 0.5f, statTxtr->size().y * 0.5f);
    statArrow.getTransform().setPosition(ViewSize.x, ViewSize.y * 0.5f);
    statArrow.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    annoySrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Annoyed.anim"));
    confuseSrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Confusion.anim"));
    frozenSrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Frozen.anim"));
    frustrationSrc = AnimationManager::load(
        join(Properties::AnimationPath(), "Battle/Ailments/Frustration.anim"));
    sleepSrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Sleep.anim"));
    stickySrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Sticky.anim"));
    trappedSrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Trapped.anim"));
    // TODO - update to jumped anim when we have it
    jumpedSrc =
        AnimationManager::load(join(Properties::AnimationPath(), "Battle/Ailments/Trapped.anim"));
    recreateAilmentAnimation(trappedSrc);
    ailmentAnim.getTransform().setPosition(ViewSize * 0.5f);

    throwBallTxtr = engine.renderer().texturePool().getOrLoadTexture(
        join(Properties::ImagePath(), "Battle/Balls/peopleball_centered.png"));
    throwBall.create(engine, throwBallTxtr);
    setThrowBallTxtr(throwBallTxtr);
    throwBall.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    sparks = &engine.particleSystem().addRepeatedSystem<PeoplemonSpark>();
    sparks->addAffector<SparkAffector>();
    sparks->addSink<SparkSink>();
    sparkExplosionEmitter = sparks->addEmitter<SparkExplosionEmitter>();
    sparks->addToScene(scene);

    implosion = &engine.particleSystem().addRepeatedSystem<PeoplemonSpark>();
    implosion->addAffector<SparkAffector>();
    implosion->addSink<SparkSink>();
    sparkImplosionEmitter = implosion->addEmitter<SparkImplosionEmitter>();
    implosion->addToScene(scene);
}

void PeoplemonAnimation::setPeoplemon(pplmn::Id ppl) {
    const auto path = position == Position::Player ? pplmn::Peoplemon::playerImage :
                                                     pplmn::Peoplemon::opponentImage;
    txtr            = engine.renderer().texturePool().getOrLoadTexture(path(ppl));

    const glm::vec2 ts(txtr->size());
    peoplemon.setTexture(txtr, true);
    peoplemon.getTransform().setOrigin(ts.x * 0.5f, ts.y);
    peoplemon.setColor(sf::Color::White);
    scale.x = ViewSize.x / ts.x;
    scale.y = ViewSize.y / ts.y;
    peoplemon.getTransform().setScale(scale);
    state = State::Hidden;
}

void PeoplemonAnimation::triggerAnimation(Animation::Type anim) {
    state = State::Playing;
    type  = anim;

    switch (anim) {
    case Animation::Type::ComeBack:
        alpha = 255.f;
        sparkImplosionEmitter->setEnabled(true,
                                          glm::vec2(viewport.x, viewport.y) + ViewSize * 0.5f);
        setBallTexture(ballOpenTxtr);
        ball.setColor(sf::Color::White);
        break;

    case Animation::Type::SendOut:
        ballTime = 0.f;
        peoplemon.setColor(sf::Color(255, 255, 255, 0));
        ball.setColor(sf::Color::White);
        setBallTexture(ballTxtr);
        screenFlash.setFillColor(sf::Color::Transparent);
        renderBall = false;
        break;

    case Animation::Type::ShakeAndFlash:
        shakeTime = 0.f;
        peoplemon.flash(0.08f, 0.05f);
        break;

    case Animation::Type::SlideDown:
        slideAmount = 0.f;
        break;

    case Animation::Type::SlideOut:
        slideAmount = 0.f;
        break;

    case Animation::Type::MultipleStateDecrease:
        statArrow.setColor(sf::Color(70, 70, 70));
        [[fallthrough]];

    case Animation::Type::StatDecrease:
        arrowTime = 0.f;
        statArrow.getTransform().setScale(1.f, 1.f);
        break;

    case Animation::Type::MultipleStateIncrease:
        statArrow.setColor(sf::Color(70, 70, 70));
        [[fallthrough]];

    case Animation::Type::StatIncrease:
        arrowTime = 0.f;
        statArrow.getTransform().setScale(1.f, -1.f);
        break;

    case Animation::Type::Ailment:
    case Animation::Type::PassiveAilment:
        ailmentAnim.getPlayer().play(true);
        break;

    case Animation::Type::MakeWildVisible:
        state = State::Static;
        break;

    case Animation::Type::ThrowCloneBall:
    case Animation::Type::ThrowPeopleball:
        throwX     = ThrowStartX;
        throwState = BallThrowState::Arcing;
        setThrowBallTxtr(throwBallTxtr);
        throwBall.getTransform().setPosition(ThrowStartX, ThrowStartY);
        throwBall.getTransform().setRotation(0.f);
        throwBall.setColor(sf::Color::White);
        renderBall = true;
        toEat      = &peoplemon;
        if (anim == Animation::Type::ThrowCloneBall) {
            clone.create(engine, peoplemon.getTexture());
            clone.getTransform().setPosition(peoplemon.getTransform().getLocalPosition());
            clone.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);
            toEat = &clone;
        }
        break;

    case Animation::Type::PeopleballShake:
        shakeTime = 0.f;
        break;

    case Animation::Type::PeopleballCaught:
        alpha = 0.f;
        break;

    default:
        BL_LOG_ERROR << "Invalid animation type for peoplemon: " << anim;
        state = State::Static;
        break;
    }
}

void PeoplemonAnimation::triggerAnimation(const Animation& anim) {
    switch (anim.getType()) {
    case Animation::Type::StatDecrease:
    case Animation::Type::StatIncrease:
        switch (anim.getStat()) {
        case pplmn::Stat::Accuracy:
            statArrow.setColor(sf::Color(50, 50, 180));
            break;
        case pplmn::Stat::Attack:
        case pplmn::Stat::SpecialAttack:
            statArrow.setColor(sf::Color(180, 50, 50));
            break;
        case pplmn::Stat::SpecialDefense:
        case pplmn::Stat::Defense:
            statArrow.setColor(sf::Color(50, 180, 50));
            break;
        case pplmn::Stat::Speed:
            statArrow.setColor(sf::Color(120, 120, 50));
            break;
        case pplmn::Stat::Critical:
        case pplmn::Stat::Evasion:
            statArrow.setColor(sf::Color(70, 70, 70));
            break;
        default:
            BL_LOG_ERROR << "Invalid stat for animation: " << anim.getStat();
            break;
        }
        break;

    case Animation::Type::Ailment:
        updateAilmentAnimation(anim.getAilment());
        break;

    case Animation::Type::PassiveAilment:
        updateAilmentAnimation(anim.getPassiveAilment());
        break;

    default:
        break;
    }

    triggerAnimation(anim.getType());
}

bool PeoplemonAnimation::completed() const { return state != State::Playing; }

void PeoplemonAnimation::update(float dt) {
    if (state == State::Playing) {
        switch (type) {
        case Animation::Type::ComeBack: {
            // open ball state
            if (ball.getTexture().id() == ballOpenTxtr.id()) {
                alpha -= ContractRate * dt;
                if (alpha <= 0.f) {
                    ballTime = 0.f;
                    setBallTexture(ballTxtr);
                }
                else if (alpha <= 140.f) { sparkImplosionEmitter->setEnabled(false); }
                const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                const float p        = alpha / 255.f;
                const float ps       = std::sqrt(p);
                peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
                peoplemon.getTransform().setScale(ps * scale.x, ps * scale.y);
                if (a <= ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                }
                else if (a >= 255 - ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                }
                else { screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha)); }
            }
            // closed ball state
            else {
                ballTime += dt;
                if (ballTime >= 0.75f) { state = State::Static; }
            }
        } break;

        case Animation::Type::SendOut:
            // closed ball state
            if (ball.getTexture().id() == ballTxtr.id()) {
                ballTime += dt;
                if (ballTime >= 0.75f) {
                    setBallTexture(ballOpenTxtr);
                    ballFlash.setRadius(1.f);
                    ballFlash.setColorGradient(makeColor(255.f), {0.f, 0.f, 0.f, 0.f});
                    alpha = 0.f;
                    sparkExplosionEmitter->setEnabled(true);
                }
            }
            // ball open and expanding ppl state
            else {
                alpha += ExpandRate * dt;
                if (alpha > 180.f) { sparkExplosionEmitter->setEnabled(false); }
                if (alpha >= 255.f) {
                    alpha = 255.f;
                    if (sparks->getParticleCountLocked() == 0) { state = State::Static; }
                }
                const float p        = alpha / 255.f;
                const float ps       = std::sqrt(p);
                const float pss      = std::sqrt(ps);
                const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                ballFlash.setColorGradient(makeColor(255.f - 255.f * p), {0.f, 0.f, 0.f, 0.f});
                ballFlash.setRadius(BallFlashRadius * pss);
                if (a < 255) {
                    peoplemon.setColor(sf::Color(122, 8, 128, std::max(a, ScreenFlashAlpha)));
                }
                else { peoplemon.setColor(sf::Color::White); }
                peoplemon.getTransform().setScale(ps * scale.x, ps * scale.y);
                ball.setColor(sf::Color(255, 255, 255, 255 - a));
                if (a <= ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                }
                else if (a >= 255 - ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                }
                else { screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha)); }
            }
            break;

        case Animation::Type::ShakeAndFlash:
            shakeTime += dt;
            if (shakeTime >= ShakeTime) { state = State::Static; }
            break;

        case Animation::Type::SlideOut:
            slideAmount += SlideRate * dt;
            if (slideAmount >= peoplemon.getGlobalSize().x) {
                state = State::Static;
                peoplemon.setColor(sf::Color(255, 255, 255, 0)); // prevent showing
            }
            break;

        case Animation::Type::SlideDown:
            slideAmount += SlideRate * dt;
            if (slideAmount >= peoplemon.getGlobalSize().y) {
                state = State::Static;
                peoplemon.setColor(sf::Color(255, 255, 255, 0)); // prevent showing
            }
            break;

        case Animation::Type::MultipleStateDecrease:
        case Animation::Type::MultipleStateIncrease:
        case Animation::Type::StatDecrease:
        case Animation::Type::StatIncrease:
            arrowTime += dt;
            arrowOffset = ArrowShakeAmount *
                          bl::math::sin(arrowTime / ArrowShowTime * 360.f * ArrowOscillations);
            if (arrowTime >= ArrowShowTime) {
                arrowOffset = 0.f;
                state       = State::Static;
            }
            statArrow.getTransform().setPosition(statArrow.getTransform().getLocalPosition().x,
                                                 ViewSize.y * 0.5f + arrowOffset);
            break;

        case Animation::Type::Ailment:
        case Animation::Type::PassiveAilment:
            if (!ailmentAnim.getPlayer().isPlaying) { state = State::Static; }
            break;

        case Animation::Type::ThrowCloneBall:
        case Animation::Type::ThrowPeopleball: {
            switch (throwState) {
            case BallThrowState::Arcing: {
                throwX += ThrowXVel * dt;
                const float xp = throwX - ThrowMiddle;
                throwBall.getTransform().setPosition(throwX, ThrowA * xp * xp + ThrowTop);
                throwBall.getTransform().rotate(ThrowSpinRate * dt);
                if (throwX >= ThrowEndX) {
                    alpha = 255.f;
                    throwBall.getTransform().setPosition(ThrowEndX, ThrowEndY);
                    setThrowBallTxtr(ballOpenTxtr);
                    throwState = BallThrowState::Eating;
                    sparkImplosionEmitter->setEnabled(true, {ThrowEndX, ThrowEndY});
                    // move to global for render
                    if (type == Animation::Type::ThrowCloneBall) {
                        peoplemon.getTransform().setPosition(OpponentPosX + SquareSize * 0.5f,
                                                             OpponentPosY + SquareSize);
                        peoplemon.setColor(sf::Color(255, 255, 255, 128));
                    }
                }
            } break;

            case BallThrowState::Eating:
                if (throwBall.getTexture().id() == ballOpenTxtr.id()) {
                    alpha -= ContractRate * dt;
                    if (alpha <= 0.f) {
                        ballTime = 0.f;
                        setThrowBallTxtr(ballTxtr);
                    }
                    else if (alpha <= 140.f) { sparkImplosionEmitter->setEnabled(false); }
                    const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                    const float p        = alpha / 255.f;
                    const float ps       = std::sqrt(p);
                    toEat->setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
                    toEat->getTransform().setScale(ps * scale.x, p * scale.y);
                    toEat->getTransform().setPosition(
                        OpponentPosX + SquareSize * 0.5f,
                        OpponentPosY - (BallBounceHeight + 35.f) * (255.f - alpha) / 255.f +
                            SquareSize);
                    if (a <= ScreenFlashAlpha) {
                        screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                    }
                    else if (a >= 255 - ScreenFlashAlpha) {
                        screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                    }
                    else { screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha)); }
                }
                else {
                    ballTime += dt;
                    if (ballTime >= 0.5f) {
                        peoplemon.getTransform().setPosition(ViewSize.x * 0.5f, ViewSize.y);
                        if (type == Animation::Type::ThrowCloneBall) {
                            throwState = BallThrowState::CloneFading;
                            alpha      = 255.f;
                            peoplemon.setColor(sf::Color::White);
                        }
                        else {
                            throwState = BallThrowState::Bouncing;
                            bounceTime = 0.f;
                        }
                    }
                }
                break;

            case BallThrowState::Bouncing:
                bounceTime = std::min(BallBounceTime, bounceTime + dt);
                throwBall.getTransform().setPosition(
                    throwBall.getTransform().getLocalPosition().x,
                    OpponentPosY + BallBounceHeight -
                        std::abs(bl::math::cos(bl::math::radiansToDegrees(
                            3.5f * bl::math::Pi / BallBounceTime * bounceTime))) /
                            std::max(4.f * bounceTime, 1.f) * BallBounceHeight);
                if (bounceTime >= BallBounceTime) {
                    throwBall.getTransform().setPosition(BallFinalPos);
                    state = State::Static;
                }
                break;

            case BallThrowState::CloneFading:
                alpha = std::max(alpha - dt * GreenRate, 0.f);
                throwBall.setColor(sf::Color(255, 255, 255, static_cast<int>(alpha)));
                if (alpha <= 0.f) {
                    renderBall = false;
                    state      = State::Static;
                }
                break;
            }
        } break;

        case Animation::Type::PeopleballShake:
            shakeTime += dt;
            if (shakeTime <= BallShakeTime) {
                throwBall.getTransform().setPosition(
                    BallFinalPos +
                    glm::vec2(ShakeXAmount * bl::math::cos(ShakeSpeed * shakeTime * 2.f),
                              ShakeYAmount * bl::math::cos(ShakeSpeed * shakeTime)));
            }
            else {
                throwBall.getTransform().setPosition(BallFinalPos);
                if (shakeTime >= BallShakeTime + ShakeRestTime) { state = State::Static; }
            }
            break;

        case Animation::Type::PeopleballCaught:
            alpha = std::min(alpha + dt * GreenRate, 255.f);
            throwBall.setColor(
                sf::Color(255 - static_cast<int>(alpha), static_cast<int>(alpha), 0));
            if (alpha >= 255.f) { state = State::Static; }
            break;

        case Animation::Type::PlayerFirstSendout:
        case Animation::Type::OpponentFirstSendout:
        case Animation::Type::UseMove:
        case Animation::Type::MakeWildVisible:
        case Animation::Type::_ERROR:
            // do nothing
            break;
        }
    }
}

void PeoplemonAnimation::render(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (state == State::Hidden) return;

    ctx.setViewport(viewport, true);

    if (state == State::Playing) {
        switch (type) {
        case Animation::Type::ComeBack:
            peoplemon.draw(ctx);
            ball.draw(ctx);
            ctx.resetViewportAndScissor();
            screenFlash.draw(ctx);
            implosion->draw(ctx);
            break;

        case Animation::Type::SendOut:
            ball.draw(ctx);
            peoplemon.draw(ctx);
            ctx.resetViewportAndScissor();
            screenFlash.draw(ctx);
            if (ball.getTexture().id() == ballOpenTxtr.id()) {
                ctx.setViewport(viewport, true);
                ballFlash.draw(ctx);
                ctx.resetViewportAndScissor();
                sparks->draw(ctx);
            }
            break;

        case Animation::Type::ShakeAndFlash: {
            const float t = shakeTime * ShakeOffMultiple;
            const float m = bl::math::sin(t / ShakeTime * 180.f);
            VkViewport vp = viewport;
            vp.x += m * ShakeXMag * bl::math::sin(t);
            vp.y += m * ShakeYMag * bl::math::cos(-t);
            ctx.setViewport(vp, true);
            peoplemon.draw(ctx);
        } break;

        case Animation::Type::SlideDown: {
            VkViewport vp = viewport;
            vp.y += slideAmount;
            ctx.setViewport(vp, false);
            peoplemon.draw(ctx);
        } break;

        case Animation::Type::SlideOut: {
            VkViewport vp = viewport;
            vp.x += slideAmount * (position == Position::Player ? -1.f : 1.f);
            ctx.setViewport(vp, false);
            peoplemon.draw(ctx);
        } break;

        case Animation::Type::MultipleStateDecrease:
        case Animation::Type::MultipleStateIncrease:
        case Animation::Type::StatDecrease:
        case Animation::Type::StatIncrease:
            peoplemon.draw(ctx);
            statArrow.draw(ctx);
            break;

        case Animation::Type::Ailment:
        case Animation::Type::PassiveAilment:
            peoplemon.draw(ctx);
            ailmentAnim.draw(ctx);
            break;

        case Animation::Type::ThrowCloneBall:
        case Animation::Type::ThrowPeopleball:
            switch (throwState) {
            case BallThrowState::Arcing:
                peoplemon.draw(ctx);
                ctx.resetViewportAndScissor();
                throwBall.draw(ctx);
                break;

            case BallThrowState::Eating:
                // in global coords
                ctx.resetViewportAndScissor();
                peoplemon.draw(ctx);
                if (type == Animation::Type::ThrowCloneBall) { clone.draw(ctx); }
                throwBall.draw(ctx);
                screenFlash.draw(ctx);
                implosion->draw(ctx);
                break;

            case BallThrowState::CloneFading:
                peoplemon.draw(ctx);
                [[fallthrough]];

            case BallThrowState::Bouncing:
            default:
                // in global coords
                ctx.resetViewportAndScissor();
                throwBall.draw(ctx);
                break;
            }
            break;

        case Animation::Type::PeopleballShake:
            // in global coords
            ctx.resetViewportAndScissor();
            throwBall.draw(ctx);
            break;

        case Animation::Type::PeopleballCaught:
            // in global coords
            ctx.resetViewportAndScissor();
            throwBall.draw(ctx);
            break;

        case Animation::Type::PlayerFirstSendout:
        case Animation::Type::OpponentFirstSendout:
        case Animation::Type::UseMove:
        case Animation::Type::MakeWildVisible:
        case Animation::Type::_ERROR:
            // do nothing
            break;
        }
    }
    else if (state == State::Static) {
        peoplemon.draw(ctx);
        if (renderBall) {
            ctx.resetViewportAndScissor();
            throwBall.draw(ctx);
        }
    }

    ctx.resetViewportAndScissor();
}

void PeoplemonAnimation::setBallTexture(bl::rc::res::TextureRef& t) {
    ball.setTexture(t, true);
    ball.getTransform().setOrigin(t->size().x / 2.f, t->size().y);
}

void PeoplemonAnimation::setThrowBallTxtr(bl::rc::res::TextureRef& t) {
    throwBall.setTexture(t, true);
    throwBall.getTransform().setOrigin(t->size() * 0.5f);
}

void PeoplemonAnimation::updateAilmentAnimation(pplmn::Ailment ail) {
    switch (ail) {
    case pplmn::Ailment::Annoyed:
        recreateAilmentAnimation(annoySrc);
        break;
    case pplmn::Ailment::Frozen:
        recreateAilmentAnimation(frozenSrc);
        break;
    case pplmn::Ailment::Frustrated:
        recreateAilmentAnimation(frustrationSrc);
        break;
    case pplmn::Ailment::Sleep:
        recreateAilmentAnimation(sleepSrc);
        break;
    case pplmn::Ailment::Sticky:
        recreateAilmentAnimation(stickySrc);
        break;
    default:
        BL_LOG_WARN << "Invalid ailment: " << ail;
        break;
    }
}

void PeoplemonAnimation::updateAilmentAnimation(pplmn::PassiveAilment ail) {
    switch (ail) {
    case pplmn::PassiveAilment::Confused:
        recreateAilmentAnimation(confuseSrc);
        break;
    case pplmn::PassiveAilment::Trapped:
        recreateAilmentAnimation(trappedSrc);
        break;
    case pplmn::PassiveAilment::Stolen:
        recreateAilmentAnimation(jumpedSrc);
        break;
    default:
        BL_LOG_WARN << "Invalid ailment animation: " << ail;
        break;
    }
}

void PeoplemonAnimation::recreateAilmentAnimation(
    bl::resource::Ref<bl::gfx::a2d::AnimationData>& src) {
    ailmentAnim.createWithUniquePlayer(engine, src);
    ailmentAnim.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

} // namespace view
} // namespace battle
} // namespace core
