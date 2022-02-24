#ifndef GAME_BATTLES_BATTLESTATELOCAL_HPP
#define GAME_BATTLES_BATTLESTATELOCAL_HPP

#include <Core/Systems/Player.hpp>
#include <Game/Battles/BattleFSM.hpp>
#include <Game/Battles/BattleState.hpp>
#include <memory>

namespace game
{
namespace battle
{
/**
 * @brief This battle state is used for local battles and for online battles as the host
 *
 * @ingroup Battles
 *
 */
class BattleStateLocal : public BattleState {
public:
    /**
     * @brief Construct a new local battle state
     *
     * @param player The player
     * @param opponent The opponent fighting the player
     */
    BattleStateLocal(core::system::Player& player, std::unique_ptr<Battler>&& opponent);

    /**
     * @brief Updates the battle state
     *
     */
    virtual void update() override;

private:
    // TODO - player battler
    std::unique_ptr<Battler> notPlayer;
    BattleFSM fsm;
};

} // namespace battle
} // namespace game

#endif
