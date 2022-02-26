#ifndef GAME_BATTLES_BATTLESTATE_HPP
#define GAME_BATTLES_BATTLESTATE_HPP

#include <Game/Battles/Battler.hpp>

namespace game
{
namespace battle
{
/**
 * @brief Stores and represents the current state of a battle. This is the base class for local and
 *        remote battle states and provides the data storage as well as the local command queue
 *        resolution.
 *
 * @ingroup Battles
 *
 */
class BattleState {
public:
    /**
     * @brief Creates a new BattleState
     *
     */
    BattleState();

    /**
     * @brief Returns the local player Battler
     *
     */
    Battler& localPlayer();

    /**
     * @brief Returns the opponent Battler
     *
     */
    Battler& enemy();

private:
    Battler player;
    Battler opponent;
};

} // namespace battle
} // namespace game

#endif
