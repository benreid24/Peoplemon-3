#ifndef GAME_STATES_MAP_EXPLORER_HPP
#define GAME_STATES_MAP_EXPLORER_HPP

#include <Core/Maps/Map.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
class MapExplorer : public State {
public:
    static bl::engine::State::Ptr create(core::game::Systems& systems, const std::string& map);

    virtual ~MapExplorer() = default;

    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;

    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt) override;

    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    const std::string file;
    core::map::Map map;
    float zoomFactor;

    MapExplorer(core::game::Systems& systems, const std::string& map);
};

} // namespace state
} // namespace game

#endif
