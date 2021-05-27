#include <Game/States/MapExplorer.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>
#include <Game/States/PauseMenu.hpp>

namespace game
{
namespace state
{
namespace
{
class ExplorerCamera : public core::system::camera::Camera {
public:
    static Ptr create() { return Ptr(new ExplorerCamera()); }

    virtual ~ExplorerCamera() = default;

    virtual bool valid() const { return true; }

    virtual void update(core::system::Systems& systems, float dt) {
        const float PixelsPerSecond =
            0.5f * size * static_cast<float>(core::Properties::WindowWidth());
        static const float ZoomPerSecond = 0.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { position.y -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { position.x += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { position.y += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { position.x -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            size -= ZoomPerSecond * dt;
            if (size < 0.1f) size = 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { size += ZoomPerSecond * dt; }
    }

private:
    ExplorerCamera()
    : Camera() {}
};
} // namespace

bl::engine::State::Ptr MapExplorer::create(core::system::Systems& systems,
                                           const std::string& name) {
    return bl::engine::State::Ptr(new MapExplorer(systems, name));
}

MapExplorer::MapExplorer(core::system::Systems& systems, const std::string& name)
: State(systems)
, file(name)
, mapExplorer(ExplorerCamera::create())
, activated(false) {}

const char* MapExplorer::name() const { return "MapExplorer"; }

void MapExplorer::activate(bl::engine::Engine& engine) {
    if (!activated) {
        activated = true;
        if (!systems.world().switchMaps(file, 5)) {
            BL_LOG_ERROR << "Failed to switch to map: " << file;
            engine.flags().set(bl::engine::Flags::Terminate);
        }
    }
    systems.engine().eventBus().subscribe(this);
}

void MapExplorer::deactivate(bl::engine::Engine&) { systems.engine().eventBus().unsubscribe(this); }

void MapExplorer::update(bl::engine::Engine& engine, float dt) { systems.update(dt); }

void MapExplorer::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.render().render(engine.window(), lag);
    engine.window().display();
}

void MapExplorer::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P &&
            systems.cameras().activeCamera().get() == mapExplorer.get()) {
            systems.cameras().popCamera();
            systems.player().makePlayerControlled(systems.player().player());
        }
        else if (event.key.code == sf::Keyboard::O &&
                 systems.cameras().activeCamera().get() != mapExplorer.get()) {
            systems.cameras().pushCamera(mapExplorer);
            systems.player().removePlayerControlled(systems.player().player());
        }
    }
}

void MapExplorer::observe(const core::event::StateChange& event) {
    switch (event.type) {
    case core::event::StateChange::GamePaused:
        systems.engine().pushState(PauseMenu::create(systems));
        break;

    default:
        break;
    }
}

} // namespace state
} // namespace game
