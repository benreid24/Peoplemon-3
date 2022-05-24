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
constexpr float FadeRate         = 120.f;
constexpr float SlideRate        = 285.f;
constexpr float ShakesPerSecond  = 18.f;
constexpr float ShakeOffMultiple = ShakesPerSecond * 360.f;
constexpr float ShakeXMag        = 10.f;
constexpr float ShakeYMag        = 5.f;
constexpr float ShakeTime        = 0.75f;
constexpr float ExpandRate       = 400.f;
constexpr float BallFlashRadius  = 200.f;
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
, spark(4.f)
, flasher(peoplemon, 0.08f, 0.05f) {
    peoplemon.setPosition(ViewSize.x * 0.5f, ViewSize.y);
    placeholder.setFillColor(sf::Color::Red);
    placeholder.setFont(Properties::MenuFont());
    placeholder.setCharacterSize(26);

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
    ball.setTexture(*ballTxtr, true);
    ball.setPosition(ViewSize * 0.5f);
    ballFlash.setPosition(ball.getPosition().x, ball.getPosition().y + 15.f);
    spark.setOuterColor(sf::Color::Transparent);
    ballFlash.setOuterColor(sf::Color::Transparent);
}

void PeoplemonAnimation::configureView(const sf::View& pv) {
    const sf::Vector2f& pos = position == Position::Player ? PlayerPos : OpponentPos;
    view = bl::interface::ViewUtil::computeSubView(sf::FloatRect(pos, ViewSize), pv);
    view.setCenter(ViewSize * 0.5f);
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
        placeholder.setString("ComeBack");
        alpha = 255.f; // just a basic fadeout
        break;

    case Animation::Type::SendOut:
        placeholder.setString("");
        ballTime = 0.f;
        peoplemon.setColor(sf::Color(255, 255, 255, 0));
        ball.setColor(sf::Color::White);
        ball.setTexture(*ballTxtr, true);
        break;

    case Animation::Type::ShakeAndFlash:
        placeholder.setString("");
        shakeTime = 0.f;
        flasher.reset();
        break;

    case Animation::Type::SlideDown:
        placeholder.setString("");
        slideAmount = 0.f;
        break;

    case Animation::Type::SlideOut:
        placeholder.setString("SlideOut");
        // TODO
        break;

    default:
        BL_LOG_ERROR << "Invalid animation type for peoplemon: " << anim;
        state = State::Static;
        break;
    }

    const sf::FloatRect bounds = placeholder.getGlobalBounds();
    const sf::Vector2f size(bounds.width, bounds.height);
    placeholder.setPosition(ViewSize * 0.5f - size * 0.5f);
}

void PeoplemonAnimation::triggerAnimation(const Animation& anim) {
    // TODO - init stuff based on stat change or other params
    triggerAnimation(anim.getType());
}

bool PeoplemonAnimation::completed() const { return state != State::Playing; }

void PeoplemonAnimation::update(float dt) {
    const auto timedOnly = [this, dt]() {
        slideAmount += dt;
        if (slideAmount > 2.f) {
            slideAmount = 0.f;
            state       = State::Static;
        }
    };

    if (state == State::Playing) {
        switch (type) {
        case Animation::Type::ComeBack:
            slideAmount -= dt * FadeRate;
            if (slideAmount <= 0.f) {
                slideAmount = 0.f;
                state       = State::Static;
            }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(slideAmount)));
            break;

        case Animation::Type::SendOut:
            // closed ball state
            if (ball.getTexture() == ballTxtr.get()) {
                ballTime += dt;
                if (ballTime >= 0.75f) {
                    ball.setTexture(*ballOpenTxtr, true);
                    ballFlash.setRadius(1.f);
                    ballFlash.setCenterColor(makeColor(255.f));
                    alpha = 0.f;
                    sparks.setTargetCount(50);
                    sparks.setCreateRate(200.f);
                }
            }
            // ball open and expanding ppl state
            else {
                sparks.update(
                    [this, dt](Spark& sp) {
                        sp.time += dt;
                        sp.position += sp.velocity * dt;
                        return sp.time < sp.lifetime;
                    },
                    dt);
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
                if (a < 255) { peoplemon.setColor(sf::Color(122, 8, 128, a)); }
                else {
                    peoplemon.setColor(sf::Color::White);
                }
                peoplemon.setScale(ps * scale.x, ps * scale.y);
                ball.setColor(sf::Color(255, 255, 255, 255 - a));
            }
            break;

        case Animation::Type::ShakeAndFlash:
            shakeTime += dt;
            flasher.update(dt);
            if (shakeTime >= ShakeTime) { state = State::Static; }
            break;

        case Animation::Type::SlideOut:
            // TODO
            timedOnly();
            break;

        case Animation::Type::SlideDown:
            slideAmount += SlideRate * dt;
            if (slideAmount >= peoplemon.getGlobalBounds().height) {
                state = State::Static;
                peoplemon.setColor(sf::Color(255, 255, 255, 0)); // prevent showing
            }
            break;

        default:
            break;
        }
    }
}

void PeoplemonAnimation::render(sf::RenderTarget& target, float lag) const {
    if (state == State::Hidden) return;

    const sf::View oldView = target.getView();
    target.setView(view);

    sf::RenderStates states;
    if (state == State::Playing) {
        float useMe = slideAmount;
        switch (type) {
        case Animation::Type::ComeBack:
            useMe -= lag * FadeRate;
            if (useMe <= 0.f) { useMe = 0.f; }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(useMe)));
            target.draw(peoplemon, states);
            break;

        case Animation::Type::SendOut:
            target.draw(ball, states);
            target.draw(peoplemon, states);
            break;

        case Animation::Type::ShakeAndFlash: {
            const float t = (shakeTime + lag) * ShakeOffMultiple;
            const float m = bl::math::sin(t / ShakeTime * 180.f);
            states.transform.translate(m * ShakeXMag * bl::math::sin(t),
                                       m * ShakeYMag * bl::math::cos(-t));
            flasher.render(target, states, lag);
        } break;

        case Animation::Type::SlideDown:
            useMe += SlideRate * lag;
            states.transform.translate(0.f, useMe);
            target.draw(peoplemon, states);
            break;

        default:
            break;
        }

        target.draw(placeholder);
    }
    else if (state == State::Static) {
        target.draw(peoplemon, states);
    }

    target.setView(oldView);

    if (state == State::Playing) {
        states.transform.translate(offset);

        switch (type) {
        case Animation::Type::SendOut:
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

        default:
            break;
        }
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

} // namespace view
} // namespace battle
} // namespace core
