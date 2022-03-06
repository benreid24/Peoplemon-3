#ifndef GAME_BATTLES_BATTLESKIPPER_HPP
#define GAME_BATTLES_BATTLESKIPPER_HPP

namespace game
{
namespace battle
{
/**
 * @brief A debug-only utility for skipping battles when testing
 *
 * @ingroup Battles
 *
 */
class BattleSkipper {
public:
    /**
     * @brief Returns whether or not battles should be skipped
     *
     */
    static bool skipBattles() { return value(); }

    /**
     * @brief Toggles the setting of skipping battles
     *
     */
    static void toggleSkipBattles() { value() = !value(); }

private:
    static bool& value() {
        static bool skip = false;
        return skip;
    }
};

} // namespace battle
} // namespace game

#endif
