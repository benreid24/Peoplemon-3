#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Media/Shapes.hpp>
#include <Properties.hpp>
#include <SFML/Graphics.hpp>

class DemoEngineState : public bl::engine::State {
public:
    static constexpr float DegPerSec = 60.f;

    static bl::engine::State::Ptr create() { return Ptr(new DemoEngineState()); }

    virtual const char* name() const { return "DemoEngineState"; }

    virtual void makeActive(bl::engine::Engine& engine) {
        triangle.setPosition(engine.settings().videoMode().width / 2,
                             engine.settings().videoMode().height / 2);
        BL_LOG_INFO << "DemoEngineState activated";
    }

    virtual void update(bl::engine::Engine& engine, float dt) override {
        triangle.rotate(dt * DegPerSec);
    }

    virtual void render(bl::engine::Engine& engine, float lag) override {
        // Account for lag
        const float og = triangle.getRotation();
        triangle.rotate(lag * DegPerSec);

        engine.window().clear(sf::Color::Cyan);
        engine.window().draw(triangle);
        engine.window().display();

        triangle.setRotation(og);
    }

private:
    bl::shapes::Triangle triangle;

    DemoEngineState()
    : triangle({0, 0}, {120, 0}, {60, 120}) {
        triangle.setFillColor(sf::Color::Red);
    }
};

int main() {
    BL_LOG_INFO << "Loading application properties";
    if (!Properties::load()) {
        BL_LOG_ERROR << "Failed to load application properties";
        return 1;
    }

    BL_LOG_INFO << "Creating engine instance";
    const bl::engine::Settings engineSettings =
        bl::engine::Settings()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withWindowStyle(sf::Style::Close | sf::Style::Titlebar)
            .withWindowTitle("BLIB Project");
    bl::engine::Engine engine(engineSettings);
    BL_LOG_INFO << "Created engine";

    BL_LOG_INFO << "Running engine main loop";
    if (!engine.run(DemoEngineState::create())) {
        BL_LOG_ERROR << "Engine exited with error";
        return 1;
    }

    BL_LOG_INFO << "Exiting normally";
    return 0;
}
