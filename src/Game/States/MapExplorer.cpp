#include <Game/States/MapExplorer.hpp>

#include <BLIB/Cameras.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Game/States/PauseMenu.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float Padding  = 12.f;
constexpr float FadeTime = 0.5f;
const sf::Color BoxColor(30, 30, 30, 180);
const sf::Color BoxOutlineColor(20, 200, 20, 180);
const sf::Color TextColor(220, 220, 220);

class ExplorerCameraController : public bl::cam::CameraController2D {
public:
    ExplorerCameraController(const core::map::Map& map)
    : position(map.sizePixels().x * 0.5f, map.sizePixels().y * 0.5f)
    , zoom(1.5f) {}

private:
    glm::vec2 position;
    float zoom;

    virtual void update(float dt) override {
        const float PixelsPerSecond =
            0.5f * zoom * static_cast<float>(core::Properties::WindowWidth());
        const float ZoomPerSecond = std::max(0.5f, zoom * 0.5f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { position.y -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { position.x += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { position.y += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { position.x -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            zoom -= ZoomPerSecond * dt;
            if (zoom < 0.1f) zoom = 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { zoom += ZoomPerSecond * dt; }

        const sf::Vector2f size = zoom * core::Properties::WindowSize();
        camera().setCenter(position);
        camera().setSize({size.x, size.y});
    }
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

    hintBox.create(systems.engine(), {100.f, 100.f});
    hintBox.setFillColor(BoxColor);
    hintBox.setOutlineColor(BoxOutlineColor);
    hintBox.setOutlineThickness(4.f);
    hintBox.getTransform().setPosition(5.f, 5.f);

    hintText.create(systems.engine(),
                    core::Properties::MenuFont(),
                    "Move around: WASD\nZoom out: V\nZoom in: C\nExit: Esc",
                    28,
                    TextColor);
    hintText.getTransform().setPosition(Padding, Padding);
    hintText.setParent(hintBox);
    hintBox.setSize(hintText.getLocalSize() + glm::vec2(Padding * 2.f));
}

const char* MapExplorer::name() const { return "MapExplorer"; }

void MapExplorer::activate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::subscribe(this);
    systems.player().removePlayerControlled(systems.player().player());
    static_cast<bl::cam::Camera2D*>(engine.renderer().getObserver().getCurrentCamera())
        ->setController<ExplorerCameraController>(systems.world().activeMap());

    bl::rc::Overlay* overlay = engine.renderer().getObserver().getOrCreateSceneOverlay();
    hintBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hintText.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hintState = Hidden;
    hintBox.setHidden(true);
    hintTime = 0.f;
}

void MapExplorer::deactivate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::unsubscribe(this);
    systems.player().makePlayerControlled(systems.player().player());
    systems.world().activeMap().setupCamera(systems);
    hintBox.removeFromScene();
}

void MapExplorer::update(bl::engine::Engine&, float dt, float) {
    systems.update(dt, false);

    switch (hintState) {
    case Hidden:
        hintTime += dt;
        if (hintTime >= 4.f) {
            hintState = Showing;
            hintTime  = 0.f;
            hintBox.setHidden(false);
            hintBox.setFillColor(BoxColor);
            hintBox.setOutlineColor(BoxOutlineColor);
            hintText.getSection().setFillColor(TextColor);
        }
        break;
    case Fading:
        hintTime += dt;
        if (hintTime <= FadeTime) {
            const float p  = (1.f - hintTime / FadeTime);
            const float ab = p * static_cast<float>(BoxColor.a);
            const float a  = p * 255.f;
            hintBox.setFillColor(sf::Color(BoxColor.r, BoxColor.g, BoxColor.b, ab));
            hintBox.setOutlineColor(
                sf::Color(BoxOutlineColor.r, BoxOutlineColor.g, BoxOutlineColor.b, ab));
            hintText.getSection().setFillColor(sf::Color(TextColor.r, TextColor.g, TextColor.b, a));
        }
        else {
            hintBox.setHidden(true);
            hintState = Hidden;
        }
        break;
    }
}

void MapExplorer::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (hintState == Showing) { hintState = Fading; }
        else if (hintState == Hidden) { hintTime = 0.f; }
        if (event.key.code == sf::Keyboard::Escape) { systems.engine().popState(); }
    }
}

} // namespace state
} // namespace game
