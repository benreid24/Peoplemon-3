#ifndef GAME_BATTLES_COMMANDS_ANIMATION_HPP
#define GAME_BATTLES_COMMANDS_ANIMATION_HPP

#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/PassiveAilment.hpp>
#include <Core/Peoplemon/Stat.hpp>
#include <cstdint>
#include <variant>

namespace core
{
namespace battle
{
/// Collection of data classes for battle commands
namespace cmd
{
/**
 * @brief Represents a type of animation that can be played in battle
 *
 * @ingroup Battles
 *
 */
struct Animation {
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
        UseMove,
        PlayerFirstSendout,
        OpponentFirstSendout,
        StatIncrease,
        MultipleStateIncrease,
        StatDecrease,
        MultipleStateDecrease,
        SlideOut,
        Ailment,
        PassiveAilment
    };

    /**
     * @brief For animations that are not specific to either battler
     *
     * @param type The type of animation to play
     */
    Animation(Type type);

    /**
     * @brief Construct a new Animation for a non-move effect
     *
     * @param forActiveBattler True to play on the active battler, false on inactive
     * @param type The type of animation to play
     */
    Animation(bool forActiveBattler, Type type);

    /**
     * @brief Construct a new Animation for the attack animation
     *
     * @param forActiveBattler True to play on the active battler, false on inactive
     * @param moveIndex Which move is being used [0, 3]
     */
    Animation(bool forActiveBattler, core::pplmn::MoveId move);

    /**
     * @brief Construct a new Animation for a stat increase or decrease
     *
     * @param forActiveBattler True to play on the active battler, false on inactive
     * @param type Which type of stat change
     * @param stat The stat being changed
     */
    Animation(bool forActiveBattler, Type type, pplmn::Stat stat);

    /**
     * @brief Construct a new Animation for the given ailment
     *
     * @param forActiveBattler True to play on the active battler, false on inactive
     * @param ailment The ailment animation to play
     */
    Animation(bool forActiveBattler, pplmn::Ailment ailment);

    /**
     * @brief Construct a new Animation for the given ailment
     *
     * @param forActiveBattler True to play on the active battler, false on inactive
     * @param ailment The ailment animation to play
     */
    Animation(bool forActiveBattler, pplmn::PassiveAilment ailment);

    /**
     * @brief Returns whether or not this animation is for the active peoplemon or inactive
     *
     */
    bool forActiveBattler() const;

    /**
     * @brief Returns the type of animation to play
     *
     */
    Type getType() const;

    /**
     * @brief Returns the move for this animation
     *
     */
    pplmn::MoveId getMove() const;

    /**
     * @brief Returns the stat for this animation
     *
     */
    pplmn::Stat getStat() const;

    /**
     * @brief Get the ailment for this animation
     *
     */
    pplmn::Ailment getAilment() const;

    /**
     * @brief Get the ailment for this animation
     *
     */
    pplmn::PassiveAilment getPassiveAilment() const;

private:
    struct Empty {};

    bool forActive;
    Type type;
    std::variant<Empty, core::pplmn::MoveId, pplmn::Stat, pplmn::Ailment, pplmn::PassiveAilment>
        data;
};

} // namespace cmd
} // namespace battle
} // namespace core

#endif
