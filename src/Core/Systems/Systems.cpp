#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Systems::Systems(bl::engine::Engine& engine)
: _engine(engine)
, _clock(*this)
, _world(*this)
, _cameras(*this)
, _position(*this)
, _movement(*this) {
    _engine.eventBus().subscribe(&_position);
    _position.init();
}

void Systems::update(float dt) {
    _position.update();
    _cameras.update(dt);
    _clock.update(dt);
    _world.update(dt);

    // TODO - make rendering system. can handle fades for maps too
    _engine.window().setView(_cameras.getView());
}

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

const Clock& Systems::clock() const { return _clock; }

World& Systems::world() { return _world; }

const World& Systems::world() const { return _world; }

Cameras& Systems::cameras() { return _cameras; }

const Cameras& Systems::cameras() const { return _cameras; }

} // namespace system
} // namespace core
