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
        const float PixelsPerSecond      = 0.5f * view.getSize().x;
        static const float ZoomPerSecond = 0.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { view.move(0, -PixelsPerSecond * dt); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { view.move(PixelsPerSecond * dt, 0); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { view.move(0, PixelsPerSecond * dt); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { view.move(-PixelsPerSecond * dt, 0); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            const sf::Vector2f sizeChange = view.getSize() * ZoomPerSecond * dt;
            view.setSize(view.getSize() - sizeChange);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
            const sf::Vector2f sizeChange = view.getSize() * ZoomPerSecond * dt;
            view.setSize(view.getSize() + sizeChange);
        }

        constrainView(systems);
    }

private:
    ExplorerCamera()
    : Camera(sf::View({0.f,
                       0.f,
                       static_cast<float>(core::Properties::WindowWidth()),
                       static_cast<float>(core::Properties::WindowHeight())})) {}
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
