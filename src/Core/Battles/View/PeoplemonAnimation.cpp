#include <Core/Battles/View/PeoplemonAnimation.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <BLIB/Math/Trig.hpp>
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
const sf::Vector2f PlayerPos(88.f, 265.f);
const sf::Vector2f OpponentPos(508.f, 25.f);
const sf::Vector2f ViewSize(200.f, 200.f);
constexpr float FadeRate                = 120.f;
constexpr float SlideRate               = 285.f;
constexpr float ShakesPerSecond         = 18.f;
constexpr float ShakeOffMultiple        = ShakesPerSecond * 360.f;
constexpr float ShakeXMag               = 10.f;
constexpr float ShakeYMag               = 5.f;
constexpr float ShakeTime               = 0.75f;
constexpr float ExpandRate              = 400.f;
constexpr float ContractRate            = 225.f;
constexpr float BallFlashRadius         = 200.f;
constexpr std::uint8_t ScreenFlashAlpha = 100;
const sf::Color FlashColor(252, 230, 30);

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
, flasher(peoplemon, 0.08f, 0.05f) {
    peoplemon.setPosition(ViewSize.x * 0.5f, ViewSize.y);

    ballOpenTxtr = bl::engine::Resources::textures()
                       .load(bl::util::FileUtil::joinPath(Properties::ImagePath(),
                                                          "Battle/Balls/peopleball_open.png"))
                       .data;
    if (position == Position::Opponent) {
        ballTxtr = bl::engine::Resources::textures()
                       .load(bl::util::FileUtil::joinPath(Properties::ImagePath(),
                                                          "Battle/Balls/peopleball.png"))
                       .data;
    }
    else {
        // TODO - consider using multiple graphics based on what ball it was caught in
        ballTxtr = bl::engine::Resources::textures()
                       .load(bl::util::FileUtil::joinPath(Properties::ImagePath(),
                                                          "Battle/Balls/peopleball_player.png"))
                       .data;
    }
    setBallTexture(*ballTxtr);
    ball.setPosition(ViewSize.x * 0.5f, ViewSize.y * 0.75f);
    ballFlash.setPosition(ball.getPosition().x, ball.getPosition().y + 15.f);
    spark.setOuterColor(sf::Color::Transparent);
    ballFlash.setOuterColor(sf::Color::Transparent);
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
        break;

    case Animation::Type::SendOut:
        ballTime = 0.f;
        peoplemon.setColor(sf::Color(255, 255, 255, 0));
        ball.setColor(sf::Color::White);
        setBallTexture(*ballTxtr);
        screenFlash.setFillColor(sf::Color::Transparent);
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

    default:
        BL_LOG_ERROR << "Invalid animation type for peoplemon: " << anim;
        state = State::Static;
        break;
    }
}

void PeoplemonAnimation::triggerAnimation(const Animation& anim) {
    // TODO - init stuff based on stat change or other params
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
            // TODO
            break;

        case Animation::Type::PlayerFirstSendout:
        case Animation::Type::OpponentFirstSendout:
        case Animation::Type::UseMove:
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
            const sf::View oldView = target.getView();
            target.setView(view);
            states.transform.translate(0.f, (slideAmount + SlideRate * lag));
            target.draw(peoplemon, states);
            target.setView(oldView);
        } break;

        case Animation::Type::SlideOut: {
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
            // TODO
            break;

        case Animation::Type::PlayerFirstSendout:
        case Animation::Type::OpponentFirstSendout:
        case Animation::Type::UseMove:
        case Animation::Type::_ERROR:
            // do nothing
            break;
        }
    }
    else if (state == State::Static) {
        target.draw(peoplemon, states);
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
    sp->position.x = ViewSize.x * 0.5f + d * c;
    sp->position.y = ViewSize.y * 0.5f + d * s;
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

} // namespace view
} // namespace battle
} // namespace core
