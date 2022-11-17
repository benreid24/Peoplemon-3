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
class ExplorerCamera : public bl::render::camera::Camera {
public:
    static Ptr create(const sf::Vector2f& position) { return Ptr(new ExplorerCamera(position)); }

    virtual ~ExplorerCamera() = default;

    virtual bool valid() const override { return true; }

    virtual void update(float dt) override {
        const float PixelsPerSecond =
            0.5f * zoom * static_cast<float>(core::Properties::WindowWidth());
        static const float ZoomPerSecond = 0.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { position.y -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { position.x += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { position.y += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { position.x -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            zoom -= ZoomPerSecond * dt;
            if (zoom < 0.1f) zoom = 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { zoom += ZoomPerSecond * dt; }

        setCenter(position);
        setZoomLevel(zoom, core::Properties::WindowSize());
    }

private:
    sf::Vector2f position;
    float zoom;

    ExplorerCamera(const sf::Vector2f& pos)
    : Camera(sf::FloatRect{pos, core::Properties::WindowSize() * 1.5f}, 0.f)
    , position(pos)
    , zoom(1.5f) {}
};

} // namespace

bl::engine::State::Ptr MapExplorer::create(core::system::Systems& systems) {
    return bl::engine::State::Ptr(new MapExplorer(systems));
}

MapExplorer::MapExplorer(core::system::Systems& systems)
: State(systems) {
    sf::Vector2f camPos(500.f, 500.f);
    core::component::Position* pos =
        systems.engine().ecs().getComponent<core::component::Position>(systems.player().player());
    if (pos) { camPos = pos->positionPixels(); }
    mapExplorer = ExplorerCamera::create(camPos);

    hintText.setFont(core::Properties::MenuFont());
    hintText.setFillColor(sf::Color(220, 220, 220));
    hintText.setCharacterSize(48);
    hintText.setString("Move around: WASD\nZoom out: V\nZoom in: C");
    hintText.setPosition(15.f, 15.f);

    hintBox.setPosition(5.f, 5.f);
    hintBox.setSize({hintText.getGlobalBounds().left + hintText.getGlobalBounds().width,
                     hintText.getGlobalBounds().top + hintText.getGlobalBounds().height});
    hintBox.setFillColor(sf::Color(30, 30, 30, 180));
    hintBox.setOutlineColor(sf::Color(20, 200, 20, 180));
    hintBox.setOutlineThickness(4.f);
}

const char* MapExplorer::name() const { return "MapExplorer"; }

void MapExplorer::activate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::subscribe(this);
    systems.player().removePlayerControlled(systems.player().player());
    engine.renderSystem().cameras().pushCamera(mapExplorer);
}

void MapExplorer::deactivate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::unsubscribe(this);
    systems.player().makePlayerControlled(systems.player().player());
    engine.renderSystem().cameras().popCamera();
}

void MapExplorer::update(bl::engine::Engine&, float dt) {
    systems.update(dt, false);

    hintTime += dt;
    if (hintTime < 4.f && hintTime >= 2.f) {
        const float ab = (4.f - hintTime) / 2.f * 180.f;
        const float a  = (4.f - hintTime) / 2.f * 255.f;
        hintBox.setFillColor(sf::Color(30, 30, 30, ab));
        hintBox.setOutlineColor(sf::Color(20, 200, 20, ab));
        hintText.setFillColor(sf::Color(220, 220, 220, a));
    }
    else if (hintTime >= 4.f) {
        hintBox.setFillColor(sf::Color::Transparent);
        hintBox.setOutlineColor(sf::Color::Transparent);
        hintText.setFillColor(sf::Color::Transparent);
    }
}

void MapExplorer::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.render().render(engine.window(), systems.world().activeMap(), lag);
    engine.window().draw(hintBox);
    engine.window().draw(hintText);
    engine.window().display();
}

void MapExplorer::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) { systems.engine().popState(); }
    }
}

} // namespace state
} // namespace game
