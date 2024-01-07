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
    ExplorerCameraController(core::system::Systems& systems)
    : position(systems.player().position().transform->getLocalPosition())
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
: State(systems, bl::engine::StateMask::Menu) {
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
        ->setController<ExplorerCameraController>(systems);

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
    else if (event.type == sf::Event::MouseMoved) {
        glm::vec2 wpos = systems.engine().renderer().getObserver().transformToWorldSpace(
            {event.mouseMove.x, event.mouseMove.y});
        const glm::i32vec2 tiles = wpos / static_cast<float>(core::Properties::PixelsPerTile());
        const bl::tmap::Position pos(0, {tiles.x, tiles.y}, bl::tmap::Direction::Up);
        core::event::EntityMoved move(bl::ecs::InvalidEntity, pos, pos);
        systems.world().activeMap().observe(move);
    }
    else if (event.type == sf::Event::MouseButtonPressed &&
             event.mouseButton.button == sf::Mouse::Left) {
        const glm::i32vec2 wpos = systems.engine().renderer().getObserver().transformToWorldSpace(
            {event.mouseButton.x, event.mouseButton.y});
        bl::tmap::Position npos = systems.player().position();
        npos.position           = wpos / core::Properties::PixelsPerTile();
        if (systems.world().activeMap().contains(npos)) {
            const bl::tmap::Position ogPos = systems.player().position();
            bl::tmap::Position& playerPos =
                *systems.engine().ecs().getComponent<bl::tmap::Position>(systems.player().player());
            playerPos = npos;
            playerPos.syncTransform(core::Properties::PixelsPerTile());
            bl::event::Dispatcher::dispatch<core::event::EntityMoved>(
                {systems.player().player(), ogPos, playerPos});
        }
    }
}

} // namespace state
} // namespace game
