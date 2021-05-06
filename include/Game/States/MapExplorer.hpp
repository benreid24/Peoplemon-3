#ifndef GAME_STATES_MAP_EXPLORER_HPP
#define GAME_STATES_MAP_EXPLORER_HPP

#include <Core/Events/StateChange.hpp>
#include <Core/Maps/Map.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
class MapExplorer
: public State
, public bl::event::Listener<sf::Event, core::event::StateChange> {
public:
    static bl::engine::State::Ptr create(core::system::Systems& systems, const std::string& map);

    virtual ~MapExplorer() = default;

    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;

    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt) override;

    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    const std::string file;
    core::system::camera::Camera::Ptr mapExplorer;
    bool activated;

    MapExplorer(core::system::Systems& systems, const std::string& map);

    virtual void observe(const sf::Event& event) override;

    virtual void observe(const core::event::StateChange& event) override;
};

} // namespace state
} // namespace game

#endif
