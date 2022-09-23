#include <Core/Battles/View/PeoplemonAnimation.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <cmath>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
constexpr float PlayerPosX   = 88.f;
constexpr float PlayerPosY   = 265.f;
constexpr float OpponentPosX = 508.f;
constexpr float OpponentPosY = 25.f;
constexpr float SquareSize   = 200.f;
const sf::Vector2f PlayerPos(PlayerPosX, PlayerPosY);
const sf::Vector2f OpponentPos(OpponentPosX, OpponentPosY);
const sf::Vector2f ViewSize(SquareSize, SquareSize);
constexpr float FadeRate                = 120.f;
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
const sf::Color FlashColor(252, 230, 30);
constexpr float ThrowStartX          = -25.f;
constexpr float ThrowStartY          = PlayerPosY + SquareSize * 0.5f;
constexpr float ThrowEndX            = OpponentPosX + SquareSize * 0.5f;
constexpr float ThrowEndY            = OpponentPosY;
constexpr float ThrowTime            = 1.f;
constexpr float ThrowXVel            = (ThrowEndX - ThrowStartX) / ThrowTime;
constexpr float ThrowMiddle          = ThrowStartX + (ThrowEndX - ThrowStartX) * 0.75f;
constexpr float ThrowSecondHalfWidth = ThrowEndX - ThrowMiddle;
constexpr float ThrowTop             = 10.f;
constexpr float ThrowA     = (ThrowEndY - ThrowTop) / (ThrowSecondHalfWidth * ThrowSecondHalfWidth);
constexpr float ThrowSpins = 3.f;
constexpr float ThrowSpinRate    = ThrowSpins / ThrowTime * 360.f;
constexpr float BallBounceHeight = ThrowEndY + SquareSize * 0.75f - OpponentPosY - 20.f;
constexpr float BallBounceTime   = 1.4f;
const sf::Vector2f BallFinalPos(ThrowEndX, OpponentPosY + BallBounceHeight);
constexpr float BallShakeTime = 0.5f;
constexpr float ShakeRestTime = 0.65f;
constexpr float ShakeXAmount  = 10.f;
constexpr float ShakeYAmount  = 2.f;
constexpr float ShakeCount    = 6.f;
constexpr float ShakeSpeed    = 360.f * ShakeCount;
constexpr float GreenRate     = 255.f / 0.5f;

sf::Color makeColor(float alpha) {
    return sf::Color(FlashColor.r, FlashColor.g, FlashColor.b, static_cast<std::uint8_t>(alpha));
}

using Animation = cmd::Animation;
} // namespace

PeoplemonAnimation::PeoplemonAnimation(Position pos)
: position(pos)
, ballFlash(100.f)
, sparks(std::bind(&PeoplemonAnimation::spawnSpark, this, std::placeholders::_1), 0, 0.f)
, implosion(std::bind(&PeoplemonAnimation::spawnImplodeSpark, this, std::placeholders::_1), 0, 0.f)
, spark(4.f)
, renderBall(false)
, flasher(peoplemon, 0.08f, 0.05f) {
    auto& textures  = bl::engine::Resources::textures();
    auto& anims     = bl::engine::Resources::animations();
    const auto join = bl::util::FileUtil::joinPath;

    peoplemon.setPosition(ViewSize.x * 0.5f, ViewSize.y);

    ballOpenTxtr =
        textures.load(join(Properties::ImagePath(), "Battle/Balls/peopleball_open.png")).data;
    // TODO - consider using multiple graphics based on what ball it was caught in
    ballTxtr = textures.load(join(Properties::ImagePath(), "Battle/Balls/peopleball.png")).data;
    setBallTexture(*ballTxtr);
    ball.setPosition(ViewSize.x * 0.5f, ViewSize.y * 0.75f);
    ballFlash.setPosition(ball.getPosition().x, ball.getPosition().y + 15.f);
    spark.setOuterColor(sf::Color::Transparent);
    ballFlash.setOuterColor(sf::Color::Transparent);

    statTxtr = textures.load(join(Properties::ImagePath(), "Battle/statArrow.png")).data;
    statArrow.setTexture(*statTxtr, true);
    statArrow.setOrigin(statArrow.getGlobalBounds().width * 0.5f,
                        statArrow.getGlobalBounds().height * 0.5f);
    statArrow.setPosition(ViewSize.x, ViewSize.y * 0.5f);

    annoySrc = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Annoyed.anim")).data;
    confuseSrc =
        anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Confusion.anim")).data;
    frozenSrc = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Frozen.anim")).data;
    frustrationSrc =
        anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Frustration.anim")).data;
    sleepSrc   = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Sleep.anim")).data;
    stickySrc  = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Sticky.anim")).data;
    trappedSrc = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Trapped.anim")).data;
    // TODO - update to jumped anim when we have it
    jumpedSrc = anims.load(join(Properties::AnimationPath(), "Battle/Ailments/Trapped.anim")).data;
    ailmentAnim.setIsLoop(false);
    ailmentAnim.setPosition(ViewSize * 0.5f);
    ailmentAnim.setData(*trappedSrc);

    throwBallTxtr =
        textures.load(join(Properties::ImagePath(), "Battle/Balls/peopleball_centered.png")).data;
    setThrowBallTxtr(*throwBallTxtr);
}

void PeoplemonAnimation::configureView(const sf::View& pv) {
    const sf::Vector2f& pos = position == Position::Player ? PlayerPos : OpponentPos;
    view = bl::interface::ViewUtil::computeSubView(sf::FloatRect(pos, ViewSize), pv);
    view.setCenter(ViewSize * 0.5f);
    screenFlash.setSize(pv.getSize());
    screenFlash.setPosition(pv.getCenter());
    screenFlash.setOrigin(screenFlash.getSize() * 0.5f);
    offset = pos;
}

void PeoplemonAnimation::setPeoplemon(pplmn::Id ppl) {
    const auto path = position == Position::Player ? pplmn::Peoplemon::playerImage :
                                                     pplmn::Peoplemon::opponentImage;
    txtr            = bl::engine::Resources::textures().load(path(ppl)).data;

    const sf::Vector2f ts(txtr->getSize());
    peoplemon.setTexture(*txtr, true);
    peoplemon.setOrigin(ts.x * 0.5f, ts.y);
    peoplemon.setColor(sf::Color::White);
    scale.x = ViewSize.x / ts.x;
    scale.y = ViewSize.y / ts.y;
    peoplemon.setScale(scale);
    state = State::Hidden;
}

void PeoplemonAnimation::triggerAnimation(Animation::Type anim) {
    state = State::Playing;
    type  = anim;

    switch (anim) {
    case Animation::Type::ComeBack:
        alpha = 255.f;
        implosion.setTargetCount(50);
        implosion.setCreateRate(200.f);
        setBallTexture(*ballOpenTxtr);
        ball.setColor(sf::Color::White);
        implodeOrigin = ViewSize * 0.5f;
        break;

    case Animation::Type::SendOut:
        ballTime = 0.f;
        peoplemon.setColor(sf::Color(255, 255, 255, 0));
        ball.setColor(sf::Color::White);
        setBallTexture(*ballTxtr);
        screenFlash.setFillColor(sf::Color::Transparent);
        renderBall = false;
        break;

    case Animation::Type::ShakeAndFlash:
        shakeTime = 0.f;
        flasher.reset();
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
        statArrow.setScale(1.f, 1.f);
        break;

    case Animation::Type::MultipleStateIncrease:
        statArrow.setColor(sf::Color(70, 70, 70));
        [[fallthrough]];

    case Animation::Type::StatIncrease:
        arrowTime = 0.f;
        statArrow.setScale(1.f, -1.f);
        break;

    case Animation::Type::Ailment:
    case Animation::Type::PassiveAilment:
        ailmentAnim.play(true);
        break;

    case Animation::Type::MakeWildVisible:
        state = State::Static;
        break;

    case Animation::Type::ThrowCloneBall:
    case Animation::Type::ThrowPeopleball:
        throwX     = ThrowStartX;
        throwState = BallThrowState::Arcing;
        setThrowBallTxtr(*throwBallTxtr);
        throwBall.setPosition(ThrowStartX, ThrowStartY);
        throwBall.setRotation(0.f);
        throwBall.setColor(sf::Color::White);
        renderBall = true;
        toEat      = &peoplemon;
        if (anim == Animation::Type::ThrowCloneBall) {
            clone = peoplemon;
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
        const auto updateSpark = [this, dt](Spark& sp) {
            sp.time += dt;
            sp.position += sp.velocity * dt;
            return sp.time < sp.lifetime;
        };

        switch (type) {
        case Animation::Type::ComeBack: {
            // open ball state
            if (ball.getTexture() == ballOpenTxtr.get()) {
                alpha -= ContractRate * dt;
                implosion.update(updateSpark, dt);
                if (alpha <= 0.f) {
                    ballTime = 0.f;
                    setBallTexture(*ballTxtr);
                }
                else if (alpha <= 140.f) {
                    implosion.setTargetCount(0);
                    implosion.setCreateRate(0.f);
                }
                const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                const float p        = alpha / 255.f;
                const float ps       = std::sqrt(p);
                peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
                peoplemon.setScale(ps * scale.x, ps * scale.y);
                if (a <= ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                }
                else if (a >= 255 - ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                }
                else {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha));
                }
            }
            // closed ball state
            else {
                ballTime += dt;
                if (ballTime >= 0.75f) { state = State::Static; }
            }
        } break;

        case Animation::Type::SendOut:
            // closed ball state
            if (ball.getTexture() == ballTxtr.get()) {
                ballTime += dt;
                if (ballTime >= 0.75f) {
                    setBallTexture(*ballOpenTxtr);
                    ballFlash.setRadius(1.f);
                    ballFlash.setCenterColor(makeColor(255.f));
                    alpha = 0.f;
                    sparks.setTargetCount(50);
                    sparks.setCreateRate(200.f);
                }
            }
            // ball open and expanding ppl state
            else {
                sparks.update(updateSpark, dt);
                alpha += ExpandRate * dt;
                if (alpha > 180.f) {
                    sparks.setTargetCount(0);
                    sparks.setCreateRate(0.f);
                }
                if (alpha >= 255.f) {
                    alpha = 255.f;
                    if (sparks.particleCount() == 0) { state = State::Static; }
                }
                const float p        = alpha / 255.f;
                const float ps       = std::sqrt(p);
                const float pss      = std::sqrt(ps);
                const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                ballFlash.setCenterColor(makeColor(255.f - 255.f * p));
                ballFlash.setRadius(BallFlashRadius * pss);
                if (a < 255) {
                    peoplemon.setColor(sf::Color(122, 8, 128, std::max(a, ScreenFlashAlpha)));
                }
                else {
                    peoplemon.setColor(sf::Color::White);
                }
                peoplemon.setScale(ps * scale.x, ps * scale.y);
                ball.setColor(sf::Color(255, 255, 255, 255 - a));
                if (a <= ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                }
                else if (a >= 255 - ScreenFlashAlpha) {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                }
                else {
                    screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha));
                }
            }
            break;

        case Animation::Type::ShakeAndFlash:
            shakeTime += dt;
            flasher.update(dt);
            if (shakeTime >= ShakeTime) { state = State::Static; }
            break;

        case Animation::Type::SlideOut:
            slideAmount += SlideRate * dt;
            if (slideAmount >= peoplemon.getGlobalBounds().width) {
                state = State::Static;
                peoplemon.setColor(sf::Color(255, 255, 255, 0)); // prevent showing
            }
            break;

        case Animation::Type::SlideDown:
            slideAmount += SlideRate * dt;
            if (slideAmount >= peoplemon.getGlobalBounds().height) {
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
            break;

        case Animation::Type::Ailment:
        case Animation::Type::PassiveAilment:
            ailmentAnim.update(dt);
            if (ailmentAnim.finished()) { state = State::Static; }
            break;

        case Animation::Type::ThrowCloneBall:
        case Animation::Type::ThrowPeopleball: {
            switch (throwState) {
            case BallThrowState::Arcing: {
                throwX += ThrowXVel * dt;
                const float xp = throwX - ThrowMiddle;
                throwBall.setPosition(throwX, ThrowA * xp * xp + ThrowTop);
                throwBall.rotate(ThrowSpinRate * dt);
                if (throwX >= ThrowEndX) {
                    alpha = 255.f;
                    throwBall.setPosition(ThrowEndX, ThrowEndY);
                    setThrowBallTxtr(*ballOpenTxtr);
                    throwState = BallThrowState::Eating;
                    implosion.setTargetCount(50);
                    implosion.setCreateRate(200.f);
                    implodeOrigin = {ThrowEndX, ThrowEndY};
                    // move to global for render
                    if (type == Animation::Type::ThrowCloneBall) {
                        peoplemon.setPosition(OpponentPosX + SquareSize * 0.5f,
                                              OpponentPosY + SquareSize);
                        peoplemon.setColor(sf::Color(255, 255, 255, 128));
                    }
                }
            } break;

            case BallThrowState::Eating:
                if (throwBall.getTexture() == ballOpenTxtr.get()) {
                    alpha -= ContractRate * dt;
                    implosion.update(updateSpark, dt);
                    if (alpha <= 0.f) {
                        ballTime = 0.f;
                        setThrowBallTxtr(*ballTxtr);
                    }
                    else if (alpha <= 140.f) {
                        implosion.setTargetCount(0);
                        implosion.setCreateRate(0.f);
                    }
                    const std::uint8_t a = static_cast<std::uint8_t>(alpha);
                    const float p        = alpha / 255.f;
                    const float ps       = std::sqrt(p);
                    toEat->setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
                    toEat->setScale(ps * scale.x, p * scale.y);
                    toEat->setPosition(OpponentPosX + SquareSize * 0.5f,
                                       OpponentPosY -
                                           (BallBounceHeight + 35.f) * (255.f - alpha) / 255.f +
                                           SquareSize);
                    if (a <= ScreenFlashAlpha) {
                        screenFlash.setFillColor(sf::Color(255, 255, 255, a));
                    }
                    else if (a >= 255 - ScreenFlashAlpha) {
                        screenFlash.setFillColor(sf::Color(255, 255, 255, 255 - a));
                    }
                    else {
                        screenFlash.setFillColor(sf::Color(255, 255, 255, ScreenFlashAlpha));
                    }
                }
                else {
                    ballTime += dt;
                    if (ballTime >= 0.5f) {
                        peoplemon.setPosition(ViewSize.x * 0.5f, ViewSize.y);
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
                throwBall.setPosition(throwBall.getPosition().x,
                                      OpponentPosY + BallBounceHeight -
                                          std::abs(bl::math::cos(bl::math::radiansToDegrees(
                                              3.5f * bl::math::Pi / BallBounceTime * bounceTime))) /
                                              std::max(4.f * bounceTime, 1.f) * BallBounceHeight);
                if (bounceTime >= BallBounceTime) {
                    throwBall.setPosition(BallFinalPos);
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
                throwBall.setPosition(
                    BallFinalPos +
                    sf::Vector2f(ShakeXAmount * bl::math::cos(ShakeSpeed * shakeTime * 2.f),
                                 ShakeYAmount * bl::math::cos(ShakeSpeed * shakeTime)));
            }
            else {
                throwBall.setPosition(BallFinalPos);
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

void PeoplemonAnimation::render(sf::RenderTarget& target, float lag) const {
    if (state == State::Hidden) return;

    sf::RenderStates states;
    states.transform.translate(offset);

    if (state == State::Playing) {
        switch (type) {
        case Animation::Type::ComeBack:
            target.draw(peoplemon, states);
            target.draw(ball, states);
            target.draw(screenFlash);
            implosion.render([this, &target, &states](const Spark& s) {
                const float p = s.time / s.lifetime;
                spark.setPosition(s.position);
                spark.setCenterColor(makeColor(255.f - 150.f * p * p));
                spark.setRadius(s.radius);
                target.draw(spark, states);
            });
            break;

        case Animation::Type::SendOut:
            target.draw(ball, states);
            target.draw(peoplemon, states);
            target.draw(screenFlash);
            if (ball.getTexture() == ballOpenTxtr.get()) {
                target.draw(ballFlash, states);
                sparks.render([this, &target, &states](const Spark& s) {
                    spark.setPosition(s.position);
                    spark.setCenterColor(makeColor(255.f - 255.f * (s.time / s.lifetime)));
                    spark.setRadius(s.radius);
                    target.draw(spark, states);
                });
            }
            break;

        case Animation::Type::ShakeAndFlash: {
            const float t = (shakeTime + lag) * ShakeOffMultiple;
            const float m = bl::math::sin(t / ShakeTime * 180.f);
            states.transform.translate(m * ShakeXMag * bl::math::sin(t),
                                       m * ShakeYMag * bl::math::cos(-t));
            flasher.render(target, states, lag);
        } break;

        case Animation::Type::SlideDown: {
            states.transform       = {};
            const sf::View oldView = target.getView();
            target.setView(view);
            states.transform.translate(0.f, (slideAmount + SlideRate * lag));
            target.draw(peoplemon, states);
            target.setView(oldView);
        } break;

        case Animation::Type::SlideOut: {
            states.transform       = {};
            const sf::View oldView = target.getView();
            target.setView(view);
            states.transform.translate(
                (slideAmount + SlideRate * lag) * (position == Position::Player ? -1.f : 1.f), 0.f);
            target.draw(peoplemon, states);
            target.setView(oldView);
        } break;

        case Animation::Type::MultipleStateDecrease:
        case Animation::Type::MultipleStateIncrease:
        case Animation::Type::StatDecrease:
        case Animation::Type::StatIncrease:
            target.draw(peoplemon, states);
            states.transform.translate(0.f, arrowOffset);
            target.draw(statArrow, states);
            break;

        case Animation::Type::Ailment:
        case Animation::Type::PassiveAilment:
            target.draw(peoplemon, states);
            ailmentAnim.render(target, lag, states);
            break;

        case Animation::Type::ThrowCloneBall:
        case Animation::Type::ThrowPeopleball:
            switch (throwState) {
            case BallThrowState::Arcing:
                target.draw(peoplemon, states);
                target.draw(throwBall); // in global coords
                break;

            case BallThrowState::Eating:
                target.draw(peoplemon);
                if (type == Animation::Type::ThrowCloneBall) { target.draw(clone); }
                target.draw(throwBall); // in global coords
                target.draw(screenFlash);
                implosion.render([this, &target, &states](const Spark& s) {
                    const float p = s.time / s.lifetime;
                    spark.setPosition(s.position);
                    spark.setCenterColor(makeColor(255.f - 150.f * p * p));
                    spark.setRadius(s.radius);
                    target.draw(spark); // in global coords
                });
                break;

            case BallThrowState::CloneFading:
                target.draw(peoplemon, states);
                [[fallthrough]];

            case BallThrowState::Bouncing:
            default:
                target.draw(throwBall); // in global coords
                break;
            }
            break;

        case Animation::Type::PeopleballShake:
            target.draw(throwBall); // in global coords
            break;

        case Animation::Type::PeopleballCaught:
            target.draw(throwBall); // in global coords
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
        target.draw(peoplemon, states);
        if (renderBall) { target.draw(throwBall); }
    }
}

void PeoplemonAnimation::spawnSpark(Spark* sp) {
    const float a  = bl::util::Random::get<float>(0.f, 360.f);
    const float d  = bl::util::Random::get<float>(1.f, 3.f);
    const float v  = bl::util::Random::get<float>(150.f, 900.f);
    const float c  = bl::math::cos(a);
    const float s  = bl::math::sin(a);
    sp->position.x = ViewSize.x * 0.5f + d * c;
    sp->position.y = ViewSize.y * 0.5f + d * s;
    sp->velocity.x = v * c;
    sp->velocity.y = v * s;
    sp->time       = 0.f;
    sp->lifetime   = bl::util::Random::get<float>(0.6, 1.1);
    sp->radius     = bl::util::Random::get<float>(2.f, 12.f);
}

void PeoplemonAnimation::spawnImplodeSpark(Spark* sp) {
    const float a  = bl::util::Random::get<float>(0.f, 360.f);
    const float d  = bl::util::Random::get<float>(190.f, 210.f);
    const float v  = bl::util::Random::get<float>(450.f, 900.f);
    const float c  = bl::math::cos(a);
    const float s  = bl::math::sin(a);
    sp->position.x = implodeOrigin.x + d * c;
    sp->position.y = implodeOrigin.y + d * s;
    sp->velocity.x = v * -c;
    sp->velocity.y = v * -s;
    sp->time       = 0.f;
    sp->lifetime   = d / v;
    sp->radius     = bl::util::Random::get<float>(2.f, 12.f);
}

void PeoplemonAnimation::setBallTexture(sf::Texture& t) {
    ball.setTexture(t, true);
    ball.setOrigin(t.getSize().x / 2, t.getSize().y);
}

void PeoplemonAnimation::setThrowBallTxtr(sf::Texture& t) {
    throwBall.setTexture(t, true);
    throwBall.setOrigin(t.getSize().x / 2, t.getSize().y / 2);
}

void PeoplemonAnimation::updateAilmentAnimation(pplmn::Ailment ail) {
    switch (ail) {
    case pplmn::Ailment::Annoyed:
        ailmentAnim.setData(*annoySrc);
        break;
    case pplmn::Ailment::Frozen:
        ailmentAnim.setData(*frozenSrc);
        break;
    case pplmn::Ailment::Frustrated:
        ailmentAnim.setData(*frustrationSrc);
        break;
    case pplmn::Ailment::Sleep:
        ailmentAnim.setData(*sleepSrc);
        break;
    case pplmn::Ailment::Sticky:
        ailmentAnim.setData(*stickySrc);
        break;
    default:
        BL_LOG_WARN << "Invalid ailment: " << ail;
        break;
    }
}

void PeoplemonAnimation::updateAilmentAnimation(pplmn::PassiveAilment ail) {
    switch (ail) {
    case pplmn::PassiveAilment::Confused:
        ailmentAnim.setData(*confuseSrc);
        break;
    case pplmn::PassiveAilment::Trapped:
        ailmentAnim.setData(*trappedSrc);
        break;
    case pplmn::PassiveAilment::Stolen:
        ailmentAnim.setData(*jumpedSrc);
        break;
    default:
        BL_LOG_WARN << "Invalid ailment animation: " << ail;
        break;
    }
}

} // namespace view
} // namespace battle
} // namespace core
