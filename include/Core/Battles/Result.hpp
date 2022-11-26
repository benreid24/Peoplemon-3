#ifndef CORE_BATTLES_RESULT_HPP
#define CORE_BATTLES_RESULT_HPP

#include <vector>

namespace core
{
namespace battle
{
/**
 * @brief Represents the result of a battle
 *
 * @ingroup Battle
 *
 */
struct Result {
    bool localPlayerWon;

    /**
     * @brief Constructs an empty result
     *
     */
    Result()
    : localPlayerWon(false) {}
};

} // namespace battle
} // namespace core

#endif
