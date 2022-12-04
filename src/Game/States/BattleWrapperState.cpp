#include <Game/States/BattleWrapperState.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/States/BattleState.hpp>
#include <Game/States/Evolution.hpp>
#include <array>

namespace game
{
namespace state
{
namespace intros
{
constexpr float IntroLength = 1.6f;

class SequenceBase {
public:
    virtual ~SequenceBase()                                  = default;
    virtual void start()                                     = 0;
    virtual void update(float dt)                            = 0;
    virtual void render(sf::RenderTarget& target, float lag) = 0;
    virtual bool finished() const                            = 0;
};

class TrainerSequence : public SequenceBase {
public:
    TrainerSequence();
    virtual ~TrainerSequence() = default;
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void render(sf::RenderTarget& target, float lag) override;
    virtual bool finished() const override;

private:
    float time;
    bl::resource::Resource<sf::Texture>::Ref ballTxtr;
    sf::Sprite ball;
    sf::CircleShape barCircle;
};

class WildSequence : public SequenceBase {
public:
    WildSequence();
    virtual ~WildSequence() = default;
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void render(sf::RenderTarget& target, float lag) override;
    virtual bool finished() const override;

private:
    float time;
    float shuffleTime;
    unsigned int si;
    sf::CircleShape barCircle;
    std::array<sf::Color, 6> barColors;
};

} // namespace intros

bl::engine::State::Ptr BattleWrapperState::create(core::system::Systems& systems,
                                                  std::unique_ptr<core::battle::Battle>&& battle) {
    std::unique_ptr<intros::SequenceBase> seq;
    if (battle->type == core::battle::Battle::Type::WildPeoplemon) {
        seq = std::make_unique<intros::WildSequence>();
    }
    else {
        seq = std::make_unique<intros::TrainerSequence>();
    }
    return Ptr{new BattleWrapperState(
        systems, std::forward<std::unique_ptr<core::battle::Battle>>(battle), std::move(seq))};
}

BattleWrapperState::BattleWrapperState(core::system::Systems& systems,
                                       std::unique_ptr<core::battle::Battle>&& battle,
                                       std::unique_ptr<intros::SequenceBase>&& sequence)
: State(systems)
, state(Substate::BattleIntro)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle))
, sequence(std::forward<std::unique_ptr<intros::SequenceBase>>(sequence))
, evolveIndex(0) {}

const char* BattleWrapperState::name() const { return "BattleWrapperState"; }

void BattleWrapperState::activate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().addListener(*this);

    switch (state) {
    case Substate::Battling:
        if (systems.player().state().evolutionPending()) {
            state       = Substate::Evolving;
            evolveIndex = 0;
            incEvolveIndex();
            startEvolve();
        }
        else {
            engine.popState();
        }
        break;
    case Substate::Evolving:
        if (systems.player().state().evolutionPending()) {
            incEvolveIndex();
            startEvolve();
        }
        else {
            engine.popState();
        }
        break;
    case Substate::BattleIntro:
    default:
        sequence->start();
        sequenceView = engine.window().getView();
        sequenceView.setCenter(core::Properties::WindowSize() * 0.5f);
        sequenceView.setSize(core::Properties::WindowSize());
        break;
    }
}

void BattleWrapperState::deactivate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().removeListener(*this);
}

void BattleWrapperState::update(bl::engine::Engine& engine, float dt) {
    systems.update(dt, false);
    sequence->update(dt);
    if (sequence->finished()) {
        state = Substate::Battling;
        engine.pushState(BattleState::create(systems, std::move(battle)));
    }
}

void BattleWrapperState::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();

    systems.render().render(engine.window(), systems.world().activeMap(), lag);
    engine.window().setView(sequenceView); // engine resets it for us every frame
    sequence->render(engine.window(), lag);

    engine.window().display();
}

bool BattleWrapperState::observe(const bl::input::Actor&, unsigned int, bl::input::DispatchType,
                                 bool) {
    return true;
}

void BattleWrapperState::incEvolveIndex() {
    const auto& team = systems.player().state().peoplemon;
    while (evolveIndex < team.size() && !team[evolveIndex].pendingEvolution()) { ++evolveIndex; }
}

void BattleWrapperState::startEvolve() {
    auto& ppl              = systems.player().state().peoplemon[evolveIndex];
    ppl.pendingEvolution() = false;
    systems.engine().pushState(Evolution::create(systems, ppl));
}

namespace intros
{
TrainerSequence::TrainerSequence()
: time(0.f) {
    ballTxtr = TextureManager::load(bl::util::FileUtil::joinPath(core::Properties::ImagePath(),
                                                                 "Battle/battleBall.png"))
                   .data;
    ball.setTexture(*ballTxtr);
    ball.setOrigin(sf::Vector2f(ballTxtr->getSize()) * 0.5f);
    ball.setPosition(core::Properties::WindowSize() * 0.5f);
    barCircle.setRadius(core::Properties::WindowSize().y / 12.f / 2.f); // 12 bars
    barCircle.setOrigin(barCircle.getRadius(), barCircle.getRadius());
    barCircle.setFillColor(sf::Color::Black);
}

void TrainerSequence::start() {
    // TODO - play sound and start music
    ball.setScale(0.f, 0.f);
    time = 0.f;
}

void TrainerSequence::update(float dt) { time += dt; }

void TrainerSequence::render(sf::RenderTarget& target, float lag) {
    // compute progress and positions
    const float t              = time + lag;
    const float progress       = t / IntroLength;
    const sf::View& view       = target.getView();
    const sf::Vector2f scorner = view.getCenter() - view.getSize() * 0.5f;
    const sf::Vector2f ecorner = scorner + view.getSize();

    // render bars
    const float bw = view.getSize().x * 0.5f * progress * 1.15f;
    for (float y = scorner.y + barCircle.getRadius(); y <= ecorner.y - barCircle.getRadius();
         y += barCircle.getRadius() * 2.f) {
        for (float x = 0.f; x <= bw; x += 1.f) {
            barCircle.setPosition(scorner.x + x, y);
            target.draw(barCircle);
            barCircle.setPosition(ecorner.x - x, y);
            target.draw(barCircle);
        }
    }

    // render circle
    ball.setScale(progress, progress);
    target.draw(ball);
}

bool TrainerSequence::finished() const { return time >= IntroLength; }

WildSequence::WildSequence()
: time(0.f)
, shuffleTime(0.f)
, si(0)
, barColors({sf::Color(235, 64, 52),
             sf::Color(38, 168, 34),
             sf::Color(240, 185, 77),
             sf::Color(39, 86, 217),
             sf::Color(219, 219, 9),
             sf::Color(219, 9, 202)}) {
    barCircle.setRadius(core::Properties::WindowSize().y / 16.f / 2.f); // 16 bars
    barCircle.setOrigin(barCircle.getRadius(), barCircle.getRadius());
}

void WildSequence::start() {
    // TODO - play sound and start music
    time        = 0.f;
    shuffleTime = 0.f;
    si          = 0;
}

void WildSequence::update(float dt) {
    constexpr float ShuffleTime = 0.07f;

    time += dt;
    shuffleTime += dt;
    if (shuffleTime >= ShuffleTime) {
        si          = si < barColors.size() - 1 ? si + 1 : 0;
        shuffleTime = 0.f;
    }
}

void WildSequence::render(sf::RenderTarget& target, float lag) {
    // compute progress and positions
    const float t              = time + lag;
    const float progress       = t / IntroLength;
    const sf::View& view       = target.getView();
    const sf::Vector2f scorner = view.getCenter() - view.getSize() * 0.5f;
    const sf::Vector2f ecorner = scorner + view.getSize();

    // render bars
    const float bw = view.getSize().x * 0.5f * progress;
    unsigned int i = si;
    for (float y = scorner.y + barCircle.getRadius(); y <= ecorner.y - barCircle.getRadius();
         y += barCircle.getRadius() * 2.f) {
        i = i < barColors.size() - 1 ? i + 1 : 0;
        barCircle.setFillColor(barColors[i]);
        for (float x = 0.f; x <= bw; x += 1.f) {
            barCircle.setPosition(scorner.x + x, y);
            target.draw(barCircle);
            barCircle.setPosition(ecorner.x - x, y);
            target.draw(barCircle);
        }
    }
}

bool WildSequence::finished() const { return time >= 2.5f; }

} // namespace intros

} // namespace state
} // namespace game
