#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <iostream>

int main() {
    bl::logging::Config::rollLogs("logs", "bundle", 3);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    bl::logging::Config::addFileOutput("logs/bundle.log", bl::logging::Config::Debug);
    core::Properties::load(false);
    core::res::installDevLoaders();

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

    BL_LOG_INFO << "Beginning bundle creation";
    if (core::res::createBundles()) { BL_LOG_INFO << "Finished bundle creation"; }
    else {
        BL_LOG_ERROR << "Bundle creation failed";
        return 1;
    }

    return 0;
}
