#ifndef CORE_PLAYER_STATE_HPP
#define CORE_PLAYER_STATE_HPP

#include <BLIB/Tilemap/Position.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/StoredPeoplemon.hpp>
#include <Core/Player/Bag.hpp>
#include <Core/Player/Gender.hpp>
#include <Core/Player/Peopledex.hpp>
#include <Core/Player/StorageSystem.hpp>

namespace core
{
namespace player
{
/**
 * @brief Stores all player state
 *
 * @ingroup Player
 */
struct State {
    /**
     * @brief Initializes to default values
     */
    State();

    /**
     * @brief Restores HP and removes all ailments
     */
    void healPeoplemon();

    /**
     * @brief Returns whether or not any peoplemon have a pending evolution
     */
    bool evolutionPending() const;

    /**
     * @brief Returns whether or not the player has any living peoplemon
     */
    bool hasLivingPeoplemon() const;

    std::string name;
    player::Gender sex;
    player::Bag bag;
    long monei;
    std::vector<pplmn::OwnedPeoplemon> peoplemon;
    StorageSystem storage;
    std::unordered_set<std::string> visitedTowns;
    Peopledex peopledex;

    bl::tmap::Position savePos;
    std::string whiteoutMap;
    unsigned int whiteoutSpawn;
    unsigned int repelSteps;
};

} // namespace player
} // namespace core

#endif
