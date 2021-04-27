#include <BLIB/Logging.hpp>
#include <Game/States/MapExplorer.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr MapExplorer::create(core::system::Systems& systems,
                                           const std::string& name) {
    return bl::engine::State::Ptr(new MapExplorer(systems, name));
}

MapExplorer::MapExplorer(core::system::Systems& systems, const std::string& name)
: State(systems)
, file(name)
, zoomFactor(1.f) {}

const char* MapExplorer::name() const { return "MapExplorer"; }

void MapExplorer::activate(bl::engine::Engine& engine) {
    if (!systems.world().switchMaps(file, 0)) {
        BL_LOG_ERROR << "Failed to switch to map: " << file;
        engine.flags().set(bl::engine::Flags::Terminate);
    }
}

void MapExplorer::deactivate(bl::engine::Engine&) {}

void MapExplorer::update(bl::engine::Engine& engine, float dt) {
    static const float PixelsPerSecond = 512 * zoomFactor;
    static const float ZoomPerSecond   = 0.5f;

    systems.update(dt);

    sf::View view = engine.window().getView();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { view.move(0, -PixelsPerSecond * dt); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { view.move(PixelsPerSecond * dt, 0); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { view.move(0, PixelsPerSecond * dt); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { view.move(-PixelsPerSecond * dt, 0); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) { zoomFactor -= ZoomPerSecond * dt; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { zoomFactor += ZoomPerSecond * dt; }
    view.setSize(engine.window().getDefaultView().getSize() * zoomFactor);
    engine.window().setView(view);
}

void MapExplorer::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.world().render(engine.window(), lag);
    engine.window().display();
}

} // namespace state
} // namespace game
