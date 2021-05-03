#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <Game/States/MainMenu.hpp>

int main() {
    BL_LOG_INFO << "Loading application properties";
    if (!core::Properties::load()) {
        BL_LOG_ERROR << "Failed to load application properties";
        return 1;
    }

    BL_LOG_INFO << "Creating engine instance";
    const bl::engine::Settings engineSettings =
        bl::engine::Settings()
            .withVideoMode(sf::VideoMode(
                core::Properties::WindowWidth(), core::Properties::WindowHeight(), 32))
            .withWindowStyle(sf::Style::Close | sf::Style::Titlebar)
            .withWindowTitle("Peoplemon")
            .withWindowIcon(core::Properties::WindowIconFile())
            .fromConfig();
    bl::engine::Engine engine(engineSettings);
    BL_LOG_INFO << "Created engine";

    BL_LOG_INFO << "Initializing game systems";
    core::system::Systems systems(engine);
    BL_LOG_INFO << "Core game systems initialized";

    BL_LOG_INFO << "Running engine main loop";
    if (!engine.run(game::state::MainMenu::create(systems))) {
        BL_LOG_ERROR << "Engine exited with error";
        return 1;
    }

    BL_LOG_INFO << "Exiting normally";
    return 0;
}
