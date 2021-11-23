#include <BLIB/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/Waiter.hpp>

#include <Core/Files/ItemDB.hpp>
#include <Core/Files/MoveDB.hpp>
#include <Core/Files/PeoplemonDB.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <Game/States/MainMenu.hpp>

#include <iostream>

int main() {
    // TODO - make log roller
#ifdef PEOPLEMON_DEBUG
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Info);
    bl::logging::Config::addFileOutput("verbose.log", bl::logging::Config::Trace);
#else
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Warn);
    bl::logging::Config::addFileOutput("error.log", bl::logging::Config::Warn);
#endif

    BL_LOG_INFO << "Loading application properties";
    if (!core::Properties::load(false)) {
        BL_LOG_ERROR << "Failed to load application properties";
        return 1;
    }

    BL_LOG_INFO << "Loading game metadata";
    BL_LOG_INFO << "Loading items";
    core::file::ItemDB itemdb;
    if (!itemdb.load()) {
        BL_LOG_ERROR << "Failed to load item database";
        return 1;
    }
    core::item::Item::setDataSource(itemdb);
    BL_LOG_INFO << "Loading moves";
    core::file::MoveDB movedb;
    if (!movedb.load()) {
        BL_LOG_ERROR << "Failed to load move database";
        return 1;
    }
    core::pplmn::Move::setDataSource(movedb);
    BL_LOG_INFO << "Loading Peoplemon";
    core::file::PeoplemonDB ppldb;
    if (!ppldb.load()) {
        BL_LOG_ERROR << "Failed to load peoplemon database";
        return 1;
    }
    core::pplmn::Peoplemon::setDataSource(ppldb);
    BL_LOG_INFO << "Game metadata loaded";

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
        bl::util::Waiter::unblockAll();
        return 1;
    }

    BL_LOG_INFO << "Unblocking waiting threads";
    bl::util::Waiter::unblockAll();

    BL_LOG_INFO << "Exiting normally";
    return 0;
}
