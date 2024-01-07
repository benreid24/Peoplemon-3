#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
using namespace bl::engine;

constexpr StateMask::V WorldVisible = StateMask::Running | StateMask::Paused;

Systems::Systems(Engine& engine)
: _engine(engine)
, _clock(engine.systems().registerSystem<Clock>(FrameStage::Update0, StateMask::Running, *this))
, _ai(engine.systems().registerSystem<AI>(FrameStage::Update1, StateMask::Running, *this))
, _controllable(*this)
, _entity(*this)
, _player(engine.systems().registerSystem<Player>(FrameStage::Update0, StateMask::All, *this))
, _world(engine.systems().registerSystem<World>(FrameStage::Update0, WorldVisible, *this))
, _position(engine.systems().registerSystem<Position>(FrameStage::Update0, WorldVisible, *this))
, _movement(
      engine.systems().registerSystem<Movement>(FrameStage::Update0, StateMask::Running, *this))
, _interaction(*this)
, _hud(engine.systems().registerSystem<HUD>(FrameStage::Update0, StateMask::All, *this))
, _scripts(*this)
, _trainers(
      engine.systems().registerSystem<Trainers>(FrameStage::Update0, StateMask::Running, *this))
, _wildPeoplemon(*this)
, _flight(engine.systems().registerSystem<Flight>(FrameStage::Update0, StateMask::Running, *this))
, _render(engine.systems().registerSystem<Render>(FrameStage::Update2, WorldVisible, *this)) {
    _interaction.init();
    _scripts.init();
    _wildPeoplemon.init();
}

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

Clock& Systems::clock() { return _clock; }

const Clock& Systems::clock() const { return _clock; }

World& Systems::world() { return _world; }

const World& Systems::world() const { return _world; }

Position& Systems::position() { return _position; }

const Position& Systems::position() const { return _position; }

Movement& Systems::movement() { return _movement; }

const Movement& Systems::movement() const { return _movement; }

Entity& Systems::entity() { return _entity; }

const Entity& Systems::entity() const { return _entity; }

Player& Systems::player() { return _player; }

const Player& Systems::player() const { return _player; }

Controllable& Systems::controllable() { return _controllable; }

AI& Systems::ai() { return _ai; }

Interaction& Systems::interaction() { return _interaction; }

HUD& Systems::hud() { return _hud; }

Scripts& Systems::scripts() { return _scripts; }

const Scripts& Systems::scripts() const { return _scripts; }

Trainers& Systems::trainers() { return _trainers; }

const Trainers& Systems::trainers() const { return _trainers; }

WildPeoplemon& Systems::wildPeoplemon() { return _wildPeoplemon; }

const WildPeoplemon& Systems::wildPeoplemon() const { return _wildPeoplemon; }

Flight& Systems::flight() { return _flight; }

const Flight& Systems::flight() const { return _flight; }

Render& Systems::render() { return _render; }

} // namespace system
} // namespace core
