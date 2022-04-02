#include <Core/Battles/View/PeoplemonAnimation.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

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
constexpr float FadeRate  = 120.f;
constexpr float SlideRate = 285.f;
} // namespace

PeoplemonAnimation::PeoplemonAnimation(Position pos)
: position(pos) {}

void PeoplemonAnimation::configureView(const sf::View& pv) {
    bl::interface::ViewUtil::computeSubView(
        sf::FloatRect(position == Position::Player ? PlayerPos : OpponentPos, ViewSize), pv);
}

void PeoplemonAnimation::setPeoplemon(pplmn::Id ppl) {
    const auto path = position == Position::Player ? pplmn::Peoplemon::playerImage :
                                                     pplmn::Peoplemon::opponentImage;
    txtr            = bl::engine::Resources::textures().load(path(ppl)).data;
    peoplemon.setTexture(*txtr, true);
    peoplemon.setOrigin(static_cast<float>(txtr->getSize().x) * 0.5f,
                        static_cast<float>(txtr->getSize().y));
    peoplemon.setColor(sf::Color::White);
    state = State::Hidden;
}

void PeoplemonAnimation::triggerAnimation(Animation::Type anim) {
    state = State::Playing;
    type  = anim;

    switch (anim) {
    case Animation::Type::ComeBack:
        slideAmount = 255.f; // just a basic fadeout
        break;

    case Animation::Type::SendOut:
        slideAmount = 0.f; // just a basic fade in
        break;

    case Animation::Type::ShakeAndFlash:
        // TODO
        break;

    case Animation::Type::SlideDown:
        slideAmount = 0.f;
        break;

    default:
        BL_LOG_ERROR << "Invalid animation type for peoplemon: " << anim;
        state = State::Static;
        break;
    }
}

bool PeoplemonAnimation::completed() const { return state == State::Static; }

void PeoplemonAnimation::update(float dt) {
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
            // TODO
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
            if (useMe <= 0.f) {
                useMe = 0.f;
            }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(useMe)));
            break;

        case Animation::Type::SendOut:
            useMe += lag * FadeRate;
            if (slideAmount >= 255.f) {
                useMe = 255.f;
            }
            peoplemon.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(useMe)));
            break;

        case Animation::Type::ShakeAndFlash:
            // TODO
            break;

        case Animation::Type::SlideDown:
            useMe += SlideRate * lag;
            states.transform.translate(0.f, useMe);
            break;

        default:
            break;
        }
    }
    target.draw(peoplemon, states);

    target.setView(oldView);
}

} // namespace view
} // namespace battle
} // namespace core
