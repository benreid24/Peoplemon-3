#ifndef GAME_BATTLES_COMMANDS_ANIMATION_HPP
#define GAME_BATTLES_COMMANDS_ANIMATION_HPP

#include <Core/Items/Id.hpp>
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
        PassiveAilment,
        MakeWildVisible,
        ThrowPeopleball,
        PeopleballShake,
        PeopleballCaught,
        PeopleballBrokeout
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
     * @param forHost True to play on the host battler, false for other
     * @param type The type of animation to play
     */
    Animation(bool forHost, Type type);

    /**
     * @brief Construct a new Animation for the attack animation
     *
     * @param forHost True to play on the host battler, false for other
     * @param moveIndex Which move is being used [0, 3]
     */
    Animation(bool forHost, core::pplmn::MoveId move);

    /**
     * @brief Construct a new Animation for a stat increase or decrease
     *
     * @param forHost True to play on the host battler, false for other
     * @param type Which type of stat change
     * @param stat The stat being changed
     */
    Animation(bool forHost, Type type, pplmn::Stat stat);

    /**
     * @brief Construct a new Animation for the given ailment
     *
     * @param forHost True to play on the host battler, false for other
     * @param ailment The ailment animation to play
     */
    Animation(bool forHost, pplmn::Ailment ailment);

    /**
     * @brief Construct a new Animation for the given ailment
     *
     * @param forHost True to play on the host battler, false for other
     * @param ailment The ailment animation to play
     */
    Animation(bool forHost, pplmn::PassiveAilment ailment);

    /**
     * @brief Construct a new Animation for a peopleball
     *
     * @param forHost True to play on the host battler, false for other
     * @param type The type of animation to play
     * @param peopleball The peopleball to display
     */
    Animation(bool forHost, Type type, item::Id peopleball);

    /**
     * @brief Returns whether or not this animation is for the host or not
     *
     */
    bool forHostBattler() const;

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

    /**
     * @brief Get the Peopleball for this animation
     *
     */
    item::Id getPeopleball() const;

private:
    struct Empty {};

    bool forHost;
    Type type;
    std::variant<Empty, core::pplmn::MoveId, pplmn::Stat, pplmn::Ailment, pplmn::PassiveAilment,
                 item::Id>
        data;
};

} // namespace cmd
} // namespace battle
} // namespace core

#endif
