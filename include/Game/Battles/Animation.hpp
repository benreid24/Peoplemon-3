#ifndef GAME_BATTLES_ANIMATION_HPP
#define GAME_BATTLES_ANIMATION_HPP

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
    enum struct Target : std::uint8_t { Player = 0, Opponent = 1 };

    /**
     * @brief The type of animation to play
     *
     */
    enum struct Type : std::uint8_t {
        ShakeAndFlash = 0,
        SlideDown     = 1,
        ComeBack      = 2,
        SendOut       = 3,
        Attack        = 4
    };

    /**
     * @brief Construct a new Animation for a non-move effect
     *
     * @param target The peoplemon to play the animation on
     * @param type The type of animation to play
     */
    Animation(Target target, Type type)
    : target(target)
    , type(type) {}

    /**
     * @brief Construct a new Animation for the attack animation
     * 
     * @param target The peoplemon using the attack
     * @param moveIndex Which move is being used [0, 3]
     */
    Animation(Target target, std::uint8_t moveIndex)
    : target(target)
    , type(Type::Attack)
    , moveIndex(moveIndex) {}

    Target target;
    Type type;
    std::uint8_t moveIndex;
};

} // namespace battle
} // namespace game

#endif
