#include <Game/States/Evolution.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/States/Evolution/Particles.hpp>

namespace bl
{
namespace pcl
{
using namespace game::state;

template<>
struct RenderConfigMap<Evolution::Spark> {
    static constexpr std::uint32_t PipelineId  = core::Properties::EvolutionSparkPipelineId;
    static constexpr bool ContainsTransparency = false;

    static constexpr bool CreateRenderPipeline = true;
    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        RenderConfigDefaults<Evolution::Spark>::RenderPassIds;

    using GlobalShaderPayload = RenderConfigDefaults<Evolution::Spark>::GlobalShaderPayload;
    using DescriptorSets =
        RenderConfigDescriptorList<bl::rc::ds::Scene2DFactory,
                                   DescriptorSetFactory<Evolution::Spark, evo::GpuSpark>>;

    static constexpr bool EnableDepthTesting      = false;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    // TODO - how to constexpr join path? need constexpr??
    static constexpr const char* VertexShader =
        "Resources/Shaders/Particles/evolution_spark.vert.spv";
    static constexpr const char* FragmentShader =
        "Resources/Shaders/Particles/evolution_spark.frag.spv";
};

} // namespace pcl
} // namespace bl

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

constexpr float SparkPercent   = 1.f / 2.3f;
constexpr float SparkStopColor = 255.f * SparkPercent;

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
: State(systems, bl::engine::StateMask::Menu)
, ppl(ppl)
, state(AnimState::IntroMsg)
, sparks(nullptr)
, fadeColor(255.f) {
    auto& txtrs = systems.engine().renderer().texturePool();

    bgndTxtr = txtrs.getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::ImagePath(), "Battle/evolveBgnd.png"));
    background.create(systems.engine(), bgndTxtr);

    oldTxtr = txtrs.getOrLoadTexture(core::pplmn::Peoplemon::opponentImage(ppl.id()));
    oldThumb.create(systems.engine(), oldTxtr);
    oldThumb.getTransform().setOrigin(oldTxtr->size() * 0.5f);
    oldThumb.getTransform().setPosition(core::Properties::WindowSize().x * 0.5f,
                                        core::Properties::WindowSize().y * 0.5f);
    oldThumb.setParent(background);

    newTxtr = txtrs.getOrLoadTexture(core::pplmn::Peoplemon::opponentImage(ppl.evolvesInto()));
    newThumb.create(systems.engine(), newTxtr);
    newThumb.getTransform().setOrigin(newTxtr->size() * 0.5f);
    newThumb.getTransform().setPosition(core::Properties::WindowSize().x * 0.5f,
                                        core::Properties::WindowSize().y * 0.5f);
    newThumb.setParent(background);

    sparks = &systems.engine().particleSystem().getUniqueSystem<Spark>();
    sparks->addAffector<evo::SparkAffector>();
    sparks->addSink<evo::SparkSink>();
}

const char* Evolution::name() const { return "Evolution"; }

void Evolution::activate(bl::engine::Engine& engine) {
    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    oldThumb.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);
    newThumb.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);
    sparks->addToScene(overlay);
    engine.ecs().setEntityParent(sparks->getRenderer().getEntity(), background.entity());
    newThumb.setHidden(true);
    oldThumb.setHidden(false);

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
    engine.renderer().getObserver().popScene();
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
            state = AnimState::SizeOscillating;
            newThumb.setHidden(false);
            oscillateTime = 0.f;
            newThumb.component().setColor(oldThumb.component().getColor());
            newThumb.getTransform().setScale(0.f, 0.f);
        }
        break;

    case AnimState::SizeOscillating: {
        oscillateTime  = std::min(oscillateTime + dt, ScalingTime);
        const float s  = std::abs(bl::math::sin(SinMult * oscillateTime * oscillateTime));
        const float os = 1.f - s;
        oldThumb.getTransform().setScale(os, os);
        newThumb.getTransform().setScale(s, s);
        if (oscillateTime >= ScalingTime) {
            state = AnimState::NewFadeIn;
            oldThumb.setHidden(true);
            newThumb.getTransform().setScale(1.f, 1.f);
            fadeColor = 0.f;
            sparks->addEmitter<evo::SparkEmitter>();
        }
    } break;

    case AnimState::NewFadeIn:
        fadeColor = std::min(255.f, fadeColor + FadeRate * dt);
        newThumb.setColor(makeFade(fadeColor));
        if (fadeColor >= SparkStopColor) { sparks->removeAllEmitters(); }
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
    case AnimState::IntroMsg:
    case AnimState::CancelMsg:
    case AnimState::CancelConfirm:
    default:
        break;
    }
}

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

} // namespace state
} // namespace game
