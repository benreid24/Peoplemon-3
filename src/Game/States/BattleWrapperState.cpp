#include <Game/States/BattleWrapperState.hpp>

#include <Core/Properties.hpp>
#include <Game/States/BattleState.hpp>

namespace game
{
namespace state
{
namespace intros
{
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
    sf::Text placeholder;
    float time;
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
    sf::Text placeholder;
    float time;
};

} // namespace intros

bl::engine::State::Ptr BattleWrapperState::create(core::system::Systems& systems,
                                                  std::unique_ptr<core::battle::Battle>&& battle) {
    std::unique_ptr<intros::SequenceBase> seq;
    if (battle->type == core::battle::Battle::Type::WildPeoplemon) {
        seq = std::make_unique<intros::WildSequence>();
    }
    else { seq = std::make_unique<intros::TrainerSequence>(); }
    return Ptr{new BattleWrapperState(
        systems, std::forward<std::unique_ptr<core::battle::Battle>>(battle), std::move(seq))};
}

BattleWrapperState::BattleWrapperState(core::system::Systems& systems,
                                       std::unique_ptr<core::battle::Battle>&& battle,
                                       std::unique_ptr<intros::SequenceBase>&& sequence)
: State(systems)
, state(Substate::BattleIntro)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle))
, sequence(std::forward<std::unique_ptr<intros::SequenceBase>>(sequence)) {}

const char* BattleWrapperState::name() const { return "BattleWrapperState"; }

void BattleWrapperState::activate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().addListener(*this);

    switch (state) {
    case Substate::Battling:
        // TODO - check for evolutions and set state if any
        if (false) {
            state = Substate::Evolving;
            // TODO - push evolution state
        }
        else { engine.popState(); }
        break;
    case Substate::Evolving:
        // TODO - check if done
        if (false) {
            // TODO - push evolution state
        }
        else { engine.popState(); }
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

namespace intros
{
TrainerSequence::TrainerSequence()
: time(0.f) {
    placeholder.setFont(core::Properties::MenuFont());
    placeholder.setCharacterSize(42);
    placeholder.setFillColor(sf::Color::Red);
    placeholder.setPosition(core::Properties::WindowSize() * 0.5f);
    placeholder.setString("Trainer Intro");
    const sf::FloatRect lb = placeholder.getLocalBounds();
    const float w          = lb.width + lb.left;
    const float h          = lb.height + lb.top;
    placeholder.setOrigin(w * 0.5f, h * 0.5f);
}

void TrainerSequence::start() {
    // TODO - play sound and start music
    time = 0.f;
}

void TrainerSequence::update(float dt) { time += dt; }

void TrainerSequence::render(sf::RenderTarget& target, float) { target.draw(placeholder); }

bool TrainerSequence::finished() const { return time >= 2.5f; }

WildSequence::WildSequence()
: time(0.f) {
    placeholder.setFont(core::Properties::MenuFont());
    placeholder.setCharacterSize(42);
    placeholder.setFillColor(sf::Color::Red);
    placeholder.setPosition(core::Properties::WindowSize() * 0.5f);
    placeholder.setString("Wild Intro");
    const sf::FloatRect lb = placeholder.getLocalBounds();
    const float w          = lb.width + lb.left;
    const float h          = lb.height + lb.top;
    placeholder.setOrigin(w * 0.5f, h * 0.5f);
}

void WildSequence::start() {
    // TODO - play sound and start music
    time = 0.f;
}

void WildSequence::update(float dt) { time += dt; }

void WildSequence::render(sf::RenderTarget& target, float) { target.draw(placeholder); }

bool WildSequence::finished() const { return time >= 2.5f; }

} // namespace intros

} // namespace state
} // namespace game
