#include <Game/States/Evolution.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float FadeTime      = 1.5f;
constexpr float FadeRate      = 255.f / FadeTime;
constexpr float FadeAlpha     = 95.f;
constexpr float FadeDiff      = 255.f - FadeAlpha;
constexpr float MaxColorRecip = 1.f / 255.f;

constexpr float ScalingTime = 7.f;
constexpr float InitialRpm  = 0.9f;
constexpr float SinMult     = 90.f / InitialRpm; // 90 so we end at 0

constexpr unsigned int SparkCount = 400;
constexpr float SparkSpawnRate    = 1600.f;
constexpr float SparkPercent      = 1.f / 2.3f;
constexpr float SparkStopColor    = 255.f * SparkPercent;
const sf::Color SparkColor(252, 230, 30);

sf::Color makeColor(float alpha) {
    return sf::Color(SparkColor.r, SparkColor.g, SparkColor.b, static_cast<std::uint8_t>(alpha));
}

sf::Color makeFade(float c) {
    const float af        = 255.f - (1.f - c * MaxColorRecip) * FadeDiff;
    const std::uint8_t a  = static_cast<std::uint8_t>(af);
    const std::uint8_t ci = static_cast<std::uint8_t>(c);
    return sf::Color(ci, ci, ci, a);
}
} // namespace

bl::engine::State::Ptr Evolution::create(core::system::Systems& systems,
                                         core::pplmn::OwnedPeoplemon& ppl) {
    return Ptr{new Evolution(systems, ppl)};
}

Evolution::Evolution(core::system::Systems& systems, core::pplmn::OwnedPeoplemon& ppl)
: State(systems)
, ppl(ppl)
, state(AnimState::IntroMsg)
, sparks(std::bind(&Evolution::spawnSpark, this, std::placeholders::_1), 0, 0.f)
//, spark(4.f)
, fadeColor(255.f) {
    oldTxtr = TextureManager::load(core::pplmn::Peoplemon::opponentImage(ppl.id()));
    newTxtr = TextureManager::load(core::pplmn::Peoplemon::opponentImage(ppl.evolvesInto()));
    oldThumb.setTexture(*oldTxtr, true);
    oldThumb.setOrigin(sf::Vector2f(oldTxtr->getSize()) * 0.5f);
    oldThumb.setPosition(core::Properties::WindowSize() * 0.5f);
    newThumb.setTexture(*newTxtr, true);
    newThumb.setOrigin(sf::Vector2f(newTxtr->getSize()) * 0.5f);
    newThumb.setPosition(core::Properties::WindowSize() * 0.5f);
    bgndTxtr = TextureManager::load(
        bl::util::FileUtil::joinPath(core::Properties::ImagePath(), "Battle/evolveBgnd.png"));
    background.setTexture(*bgndTxtr, true);
}

const char* Evolution::name() const { return "Evolution"; }

void Evolution::activate(bl::engine::Engine& engine) {
    /* engine.renderSystem().cameras().pushCamera(
         bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));*/
    engine.inputSystem().getActor().addListener(*this);

    if (ppl.evolvesInto() == core::pplmn::Id::Unknown) {
        BL_LOG_CRITICAL << "Tried to evolve " << ppl.id()
                        << " which does not have a valid evolution";
        engine.popState();
    }
    else {
        systems.hud().displayMessage("What?? Something is happening!");
        systems.hud().displayStickyMessage(
            ppl.name() + " is evolving!", true, std::bind(&Evolution::messageDone, this));
        state = AnimState::IntroMsg;
        // TODO - start music
    }
}

void Evolution::deactivate(bl::engine::Engine& engine) {
    // engine.renderSystem().cameras().popCamera();
    engine.inputSystem().getActor().removeListener(*this);
    // TODO - stop music
}

void Evolution::update(bl::engine::Engine&, float dt, float) {
    const auto updateSpark = [dt](Spark& spark) {
        spark.time += dt;
        spark.position += spark.velocity * dt;
        return spark.time < spark.lifetime;
    };

    switch (state) {
    case AnimState::OldFadeOut:
        fadeColor = std::max(0.f, fadeColor - FadeRate * dt);
        oldThumb.setColor(makeFade(fadeColor));
        if (fadeColor == 0.f) {
            state         = AnimState::SizeOscillating;
            oscillateTime = 0.f;
            newThumb.setColor(oldThumb.getColor());
            newThumb.setScale(0.f, 0.f);
        }
        break;

    case AnimState::SizeOscillating: {
        oscillateTime  = std::min(oscillateTime + dt, ScalingTime);
        const float s  = std::abs(bl::math::sin(SinMult * oscillateTime * oscillateTime));
        const float os = 1.f - s;
        oldThumb.setScale(os, os);
        newThumb.setScale(s, s);
        if (oscillateTime >= ScalingTime) {
            state = AnimState::NewFadeIn;
            newThumb.setScale(1.f, 1.f);
            fadeColor = 0.f;
            sparks.setTargetCount(SparkCount);
            sparks.setCreateRate(SparkSpawnRate);
        }
    } break;

    case AnimState::NewFadeIn:
        fadeColor = std::min(255.f, fadeColor + FadeRate * dt);
        newThumb.setColor(makeFade(fadeColor));
        sparks.update(updateSpark, dt);
        if (fadeColor >= SparkStopColor) {
            sparks.setTargetCount(0);
            sparks.setCreateRate(0.f);
        }
        if (fadeColor >= 255.f) {
            state = AnimState::EvolvedMsg;
            ppl.evolve();
            systems.hud().dismissStickyMessage();
            systems.hud().displayMessage(ppl.name() + " evolved into " +
                                             core::pplmn::Peoplemon::name(ppl.evolvesInto()) + "!",
                                         std::bind(&Evolution::messageDone, this));
        }
        break;
    case AnimState::EvolvedMsg:
        sparks.update(updateSpark, dt);
        break;

    case AnimState::IntroMsg:
    case AnimState::CancelMsg:
    case AnimState::CancelConfirm:
    default:
        break;
    }
}

// void Evolution::render(bl::engine::Engine& engine, float lag) {
//     engine.window().clear();
//     engine.window().draw(background);
//     systems.hud().render(engine.window(), lag);
//
//     const auto renderSpark = [this, &engine](const Spark& s) {
//         spark.setPosition(s.position);
//         spark.setCenterColor(makeColor(255.f - 255.f * (s.time / s.lifetime)));
//         spark.setRadius(s.radius);
//         engine.window().draw(spark);
//     };
//
//     switch (state) {
//     case AnimState::IntroMsg:
//     case AnimState::OldFadeOut:
//         engine.window().draw(oldThumb);
//         break;
//     case AnimState::CancelConfirm:
//     case AnimState::CancelMsg:
//     case AnimState::SizeOscillating:
//         engine.window().draw(oldThumb);
//         engine.window().draw(newThumb);
//         break;
//     case AnimState::EvolvedMsg:
//     case AnimState::NewFadeIn:
//         engine.window().draw(newThumb);
//         sparks.render(renderSpark);
//         break;
//     default:
//         break;
//     }
//
//     engine.window().display();
// }

void Evolution::messageDone() {
    switch (state) {
    case AnimState::IntroMsg:
        state     = AnimState::OldFadeOut;
        fadeColor = 255.f;
        break;
    case AnimState::CancelMsg:
    case AnimState::EvolvedMsg:
        systems.engine().popState();
        break;
    default:
        BL_LOG_WARN << "Received message done event while in invalid state";
        break;
    }
}

void Evolution::onCancelConfirm(const std::string& choice) {
    if (choice == "Yes") {
        systems.hud().displayMessage(ppl.name() + " did not evolve.",
                                     std::bind(&Evolution::messageDone, this));
        state = AnimState::CancelMsg;
    }
    else {
        state = cancelPriorState;
        systems.hud().displayStickyMessage(
            ppl.name() + " is evolving!", false, std::bind(&Evolution::messageDone, this));
    }
}

bool Evolution::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                        bool fromEvent) {
    if (fromEvent) {
        switch (state) {
        case AnimState::OldFadeOut:
        case AnimState::SizeOscillating:
        case AnimState::NewFadeIn:
            if (ctrl == core::input::Control::Back) {
                cancelPriorState = state;
                state            = AnimState::CancelConfirm;
                systems.hud().dismissStickyMessage();
                systems.hud().promptUser(
                    "Are you sure you want to stop " + ppl.name() + " from evolving?",
                    {"Yes", "No"},
                    std::bind(&Evolution::onCancelConfirm, this, std::placeholders::_1));
            }
            break;
        default:
            break;
        }
    }
    return true;
}

void Evolution::spawnSpark(Spark* sp) {
    const float a  = bl::util::Random::get<float>(0.f, 360.f);
    const float d  = bl::util::Random::get<float>(1.f, 40.f);
    const float v  = bl::util::Random::get<float>(150.f, 900.f);
    const float c  = bl::math::cos(a);
    const float s  = bl::math::sin(a);
    sp->position   = core::Properties::WindowSize() * 0.5f + sf::Vector2f(c, s) * d;
    sp->velocity.x = v * c;
    sp->velocity.y = v * s;
    sp->time       = 0.f;
    sp->lifetime   = bl::util::Random::get<float>(0.75f, 1.4f);
    sp->radius     = bl::util::Random::get<float>(2.f, 7.f);
}

} // namespace state
} // namespace game
