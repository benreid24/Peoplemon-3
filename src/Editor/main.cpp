#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/Waiter.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/States/MainEditor.hpp>

#include <iostream>

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Info);
    bl::logging::Config::addFileOutput("editor.log", bl::logging::Config::Debug);

    BL_LOG_INFO << "Loading application properties";
    if (!core::Properties::load()) {
        BL_LOG_ERROR << "Failed to load application properties";
        return 1;
    }

    BL_LOG_INFO << "Creating engine instance";
    const bl::engine::Settings engineSettings =
        bl::engine::Settings()
            .withVideoMode(sf::VideoMode(
                core::Properties::WindowWidth() + 300, core::Properties::WindowHeight() + 120, 32))
            .withWindowStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withWindowTitle("Peoplemon Editor")
            .withWindowIcon("EditorResources/icon.png")
            .fromConfig();
    bl::engine::Engine engine(engineSettings);
    BL_LOG_INFO << "Created engine";

    BL_LOG_INFO << "Initializing game systems";
    core::system::Systems systems(engine);
    BL_LOG_INFO << "Core game systems initialized";

    BL_LOG_INFO << "Running engine main loop";
    if (!engine.run(editor::state::MainEditor::create(systems))) {
        BL_LOG_ERROR << "Engine exited with error";
        bl::util::Waiter::unblockAll();
        return 1;
    }

    BL_LOG_INFO << "Unblocking waiting threads";
    bl::util::Waiter::unblockAll();

    BL_LOG_INFO << "Exiting normally";
    return 0;
}
