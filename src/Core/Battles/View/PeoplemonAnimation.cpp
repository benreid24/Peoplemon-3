#include <Core/Battles/View/PeoplemonAnimation.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <BLIB/Math/Trig.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
const sf::Vector2f PlayerPos(88.f, 199.f);
const sf::Vector2f OpponentPos(508.f, 2.f);
const sf::Vector2f ViewSize(200.f, 200.f);
const sf::Vector2f PeoplemonPos(ViewSize.x * 0.5f, ViewSize.y);
constexpr float FadeRate         = 120.f;
constexpr float SlideRate        = 285.f;
constexpr float ShakesPerSecond  = 18.f;
constexpr float ShakeOffMultiple = ShakesPerSecond * 360.f;
constexpr float ShakeXMag        = 10.f;
constexpr float ShakeYMag        = 5.f;
constexpr float ShakeTime        = 0.75f;

using Animation = cmd::Animation;
} // namespace

PeoplemonAnimation::PeoplemonAnimation(Position pos)
: position(pos)
, flasher(peoplemon, 0.08f, 0.05f) {
    peoplemon.setPosition(ViewSize * 0.5f);
    placeholder.setFillColor(sf::Color::Red);
    placeholder.setFont(Properties::MenuFont());
    placeholder.setCharacterSize(26);
}

void PeoplemonAnimation::configureView(const sf::View& pv) {
    view = bl::interface::ViewUtil::computeSubView(
        sf::FloatRect(position == Position::Player ? PlayerPos : OpponentPos, ViewSize), pv);
    view.setCenter(ViewSize * 0.5f);
}

void PeoplemonAnimation::setPeoplemon(pplmn::Id ppl) {
    const auto path = position == Position::Player ? pplmn::Peoplemon::playerImage :
                                                     pplmn::Peoplemon::opponentImage;
    txtr            = bl::engine::Resources::textures().load(path(ppl)).data;

    const sf::Vector2f ts(txtr->getSize());
    peoplemon.setTexture(*txtr, true);
    peoplemon.setOrigin(ts * 0.5f);
    peoplemon.setColor(sf::Color::White);
    peoplemon.setScale(ViewSize.x / ts.x, ViewSize.y / ts.y);
    state = State::Hidden;
}

void PeoplemonAnimation::triggerAnimation(Animation::Type anim) {
    state       = State::Playing;
    type        = anim;
    slideAmount = 0.f;

    switch (anim) {
    case Animation::Type::ComeBack:
        placeholder.setString("ComeBack");
        alpha = 255.f; // just a basic fadeout
        break;

    case Animation::Type::SendOut:
        placeholder.setString("SendOut");
        alpha = 0.f; // just a basic fade in
        break;

    case Animation::Type::ShakeAndFlash:
        placeholder.setString("");
        shakeTime = 0.f;
        flasher.reset();
        break;

    case Animation::Type::SlideDown:
        placeholder.setString("SlideDown");
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
            slideAmount += dt * FadeRate;
            if (slideAmount >= 255.f) {
                slideAmount = 255.f;
                state       = State::Static;
            }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(slideAmount)));
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
            useMe += lag * FadeRate;
            if (slideAmount >= 255.f) { useMe = 255.f; }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(useMe)));
            target.draw(peoplemon, states);
            break;

        case Animation::Type::ShakeAndFlash: {
            const float t = (shakeTime + lag) * ShakeOffMultiple;
            const float m = bl::math::sin(t / ShakeTime * 360.f);
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
}

} // namespace view
} // namespace battle
} // namespace core
