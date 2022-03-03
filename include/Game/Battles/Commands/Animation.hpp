#ifndef GAME_BATTLES_MESSAGES_ANIMATION_HPP
#define GAME_BATTLES_MESSAGES_ANIMATION_HPP

#include <Core/Peoplemon/MoveId.hpp>
#include <cstdint>

namespace game
{
namespace battle
{
/**
 * @brief Represents a type of animation that can be played in battle
 *
 * @ingroup Battles
 *
 */
struct Animation {
    /**
     * @brief Which Peoplemon the animation should be played on. For moves the target should be the
     *        peoplemon using the move
     *
     */
    enum struct Target : std::uint8_t {
        /// The target is the battler who is current resolving their turn
        User = 0,

        /// The target is the battler who is not currently resolving their turn
        Other = 1
    };

    /**
     * @brief The type of animation to play
     *
     */
    enum struct Type : std::uint8_t {
        _ERROR        = 0,
        ShakeAndFlash = 1,
        SlideDown,
        ComeBack,
        SendOut,
        UseMove
    };

    /**
     * @brief Construct a new Animation for a non-move effect
     *
     * @param target The peoplemon to play the animation on
     * @param type The type of animation to play
     */
    Animation(Target target, Type type);

    /**
     * @brief Construct a new Animation for the attack animation
     *
     * @param target The peoplemon using the attack
     * @param moveIndex Which move is being used [0, 3]
     */
    Animation(Target target, core::pplmn::MoveId move);

    const Target target;
    const Type type;
    const core::pplmn::MoveId move;
};

} // namespace battle
} // namespace game

#endif
