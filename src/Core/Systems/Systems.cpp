#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Systems::Systems(bl::engine::Engine& engine)
: _engine(engine)
, _cameras(*this)
, _clock(*this)
, _ai(*this)
, _controllable(*this)
, _entity(*this)
, _player(*this)
, _world(*this)
, _position(*this)
, _movement(*this)
, _render(*this)
, _interaction(*this)
, _hud(*this)
, _scripts(*this)
, _trainers(*this)
, _wildPeoplemon(*this)
, _flight(*this) {
    _world.init();
    _position.init();
    _movement.init();
    _render.init();
    _controllable.init();
    _player.init();
    _ai.init();
    _interaction.init();
    _scripts.init();
    _clock.init();
    _trainers.init();
    _wildPeoplemon.init();
}

void Systems::update(float dt, bool ent) {
    _clock.update(dt);

    if (ent) {
        _ai.update(dt);
        _player.update(dt);
        _movement.update(dt);
        _trainers.update(dt);
        _flight.update(dt);
    }
    _position.update();
    _cameras.update(dt);
    _hud.update(dt);

    _world.update(dt);
    _render.update(dt);

    _engine.entities().doDestroy();
}

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

Clock& Systems::clock() { return _clock; }

const Clock& Systems::clock() const { return _clock; }

World& Systems::world() { return _world; }

const World& Systems::world() const { return _world; }

Cameras& Systems::cameras() { return _cameras; }

const Cameras& Systems::cameras() const { return _cameras; }

Position& Systems::position() { return _position; }

const Position& Systems::position() const { return _position; }

Movement& Systems::movement() { return _movement; }

const Movement& Systems::movement() const { return _movement; }

Render& Systems::render() { return _render; }

const Render& Systems::render() const { return _render; }

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

} // namespace system
} // namespace core
