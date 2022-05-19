#ifndef CORE_PLAYER_STATE_HPP
#define CORE_PLAYER_STATE_HPP

#include <Core/Components/Position.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Player/Bag.hpp>
#include <Core/Player/Gender.hpp>

namespace core
{
namespace player
{
/**
 * @brief Stores all player state
 *
 * @ingroup Player
 *
 */
struct State {
    /**
     * @brief Initializes to default values
     *
     */
    State();

    /**
     * @brief Restores HP and removes all ailments
     *
     */
    void healPeoplemon();

    std::string name;
    player::Gender sex;
    player::Bag bag;
    long monei;
    std::vector<pplmn::OwnedPeoplemon> peoplemon;

    component::Position savePos;
    std::string whiteoutMap;
    unsigned int whiteoutSpawn;
    unsigned int repelSteps;
};

} // namespace player
} // namespace core

#endif
