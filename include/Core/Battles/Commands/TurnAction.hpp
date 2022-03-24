#ifndef GAME_BATTLES_MESSAGES_TURNACTION_HPP
#define GAME_BATTLES_MESSAGES_TURNACTION_HPP

#include <cstdint>

namespace core
{
namespace battle
{
/**
 * @brief Represents an action that a battler may take on their turn
 *
 * @ingroup Battles
 *
 */
enum struct TurnAction : std::uint8_t { Fight = 0, Switch = 1, Item = 2, Run = 3 };

} // namespace battle
} // namespace core

#endif
