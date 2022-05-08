#ifndef GAME_BATTLES_COMMANDS_MESSAGE_HPP
#define GAME_BATTLES_COMMANDS_MESSAGE_HPP

#include <Core/Peoplemon/Ailment.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/PassiveAilment.hpp>
#include <Core/Peoplemon/Stat.hpp>
#include <cstdint>
#include <string>
#include <variant>

namespace core
{
namespace battle
{
namespace cmd
{
/**
 * @brief Represents a message to be displayed. The actual message text is generated by the battle
 * controller
 *
 * @ingroup Battles
 *
 */
class Message {
public:
    /**
     * @brief The type of message to display
     *
     */
    enum struct Type : std::uint8_t {
        _ERROR = 0,

        Attack = 1,
        SuperEffective,
        NotEffective,
        IsNotAffected,
        CriticalHit,
        AttackMissed,

        Callback,
        SendOut,

        TrainerIntro,
        WildIntro,
        NetworkIntro,

        OpponentFirstSendout,
        PlayerFirstSendout,

        AttackRestoredHp,
        GainedAilment,
        GainedPassiveAilment,
        AilmentGiveFail,
        PassiveAilmentGiveFail,
        GenericMoveFailed,

        WasProtected,
        RecoilDamage,
        SuicideEffect,
        CounterEffect,
        MirrorCoatEffect,
        OnlySleepAffected,
        EveryoneWokenUp,
        RandomMove,
        Absorb,
        NoPPDeath,
        MaxAtkMinAcc,
        DoubleBroPower,
        Move64Cancel,
        StatsStolen,

        DeathSwapSac,
        DeathSwapFailed,
        DeathSwapRevived,

        Guarded,
        GuardFailed,
        GuardBlockedAilment,
        GuardBlockedPassiveAilment,

        SubstituteSuicide,
        SubstituteCreated,
        SubstituteAlreadyExists,
        SubstituteTookDamage,
        SubstituteDied,
        SubstituteAilmentBlocked,
        SubstitutePassiveAilmentBlocked,

        HealBellHealed,
        HealBellAlreadyHealthy,

        StatIncreased,
        StatIncreasedSharply,
        StatIncreaseFailed,
        StatDecreased,
        StatDecreasedSharply,
        StatDecreaseFailed,

        ChargeStarted,
        ChargeUnleashed,

        PeanutAllergic,
        PeanutAte,

        EncoreStart,
        EncoreFailed,

        BatonPassFailed,
        BatonPassStart,
        BatonPassStatsCopied,

        DeathCountDown,
        DeathFromCountdown,
        DeathCountDownFailed,

        GambleOne,
        GambleTwenty,
        GambleMiddle,

        BallServed,
        BallBumped,
        BallSet,
        BallSetFail,
        BallSpiked,
        BallSpikeFail,
        BallSwiped,
        BallSwipeFail,
        BallBlocked,
        BallBlockFail,
        BallSpikeBlocked,
        BallSwipeBlocked,
        BallKillSelf,
        BallKillSpike,
        BallKillSwipe,
        BallKillTimeout,
        BallNoSwitchSuicide,

        PPLowered,
        PPLowerFail,

        EndureStart,
        Endured,
        EndureFail,

        SpikesApplied,
        SpikesFailed,
        SpikesDamage,

        HealNextStart,
        HealNextHealed,
        HealNextFail,

        Roar,
        RoarFailedNoSwitch,
        RoarClearedArea,

        AttackThenSwitched,
        AttackSwitchFailed,

        SleepHealed,
        SleepHealFailed,

        WokeUp,
        SleepingAilment,

        Fainted,
        TrainerLost,
        WonMoney,
        GotMoney,
        WhiteoutA,
        WhiteoutB,
        NetworkWinLose,

        AwardedXp,
        LevelUp,
        TryingToLearnMove,
        AskForgetMove,
        ForgotMove,
        LearnedMove,
        DidntLearnMove,

        IsConfused,
        HurtConfusion,
        SnappedConfusion,

        StolenAilment,
        TrappedAilment,
        DistractedAilment,
        AnnoyAilment,
        FrustratedAilment,
        StickyAilment,

        FrozenAilment,
        ThawedOut,

        BoardGameSwitchBlocked,
        ChillaxCritBlocked,
        ClassyFrustratedBlocked,
        GoonDamage,
        QuickDrawFirst,
        FriendlyAilmentHeal
    };

    /**
     * @brief Construct a new Message
     *
     * @param type The type of message to display
     */
    Message(Type type);

    /**
     * @brief Construct a new Message of the Attack type
     *
     * @param move The move being used
     */
    Message(pplmn::MoveId move);

    /**
     * @brief Construct a new Message for when moves change
     *
     * @param type The type of message
     * @param oldMove The original move
     * @param newMove The move it turned into
     */
    Message(Type type, pplmn::MoveId oldMove, pplmn::MoveId newMove);

    /**
     * @brief Construct a new Message with an extra boolean value
     *
     * @param type The type of message to create
     * @param forActive True if the message is for the active battler, false for inactive
     */
    Message(Type type, bool forActive);

    /**
     * @brief Construct a new Message with an ailment
     *
     * @param type The type of message to create
     * @param ailment The ailment to create it with
     * @param forActiveBattler True if the message is for the active battler, false for the inactive
     */
    Message(Type type, pplmn::Ailment ailment, bool forActiveBattler);

    /**
     * @brief Construct a new Message with a passive ailment
     *
     * @param type The type of message to create
     * @param ailment The ailment to create it with
     * @param forActiveBattler True if the message is for the active battler, false for the inactive
     */
    Message(Type type, pplmn::PassiveAilment ailment, bool forActiveBattler);

    /**
     * @brief Construct a new Message for stat increases and decreases
     *
     * @param type The type of message to create
     * @param stat The stat the message is for
     * @param forActiveBattler If the message is for the active battler or not
     */
    Message(Type type, pplmn::Stat stat, bool forActiveBattler);

    /**
     * @brief Construct a new Message for some integer type
     *
     * @param type The type of message
     * @param ival The integer type to have in the message
     * @param forActiveBattler If the message is for the active battler or not
     */
    Message(Type type, std::int16_t ival, bool forActiveBattler);

    /**
     * @brief Construct a new Message for some integer type
     *
     * @param type The type of message
     * @param move The move to have in the message
     * @param forActiveBattler If the message is for the active battler or not
     */
    Message(Type type, pplmn::MoveId move, bool forActiveBattler);

    /**
     * @brief Creates a new Message for the given type, index, and int value. Index is for player
     *        peoplemon only. Active battler field is used for this purpose
     *
     * @param type The type of message
     * @param index The index of the peoplemon it is for
     * @param ival The integer value to store
     */
    Message(Type type, std::uint8_t index, unsigned int ival);

    /**
     * @brief Creates a message with an index and a move
     *
     * @param type The type of message
     * @param index The index of the player peoplemon
     * @param move The move for the message
     */
    Message(Type type, std::uint8_t index, pplmn::MoveId move);

    /**
     * @brief Creates a message with a move
     *
     * @param type The type of message
     * @param move The move for the message
     */
    Message(Type type, pplmn::MoveId move);

    /**
     * @brief Returns the type of message this is
     *
     */
    Type getType() const;

    /**
     * @brief Returns the move id of the message
     *
     */
    pplmn::MoveId getMoveId() const;

    /**
     * @brief Returns whether or not this message is for the active battler or not
     *
     */
    bool forActiveBattler() const;

    /**
     * @brief Returns the peoplemon index the message is for
     *
     */
    std::uint8_t forIndex() const;

    /**
     * @brief Returns the ailment for this message
     *
     */
    pplmn::Ailment getAilment() const;

    /**
     * @brief Returns the passive ailment for this message
     *
     */
    pplmn::PassiveAilment getPassiveAilment() const;

    /**
     * @brief Returns the stat for this message
     *
     */
    pplmn::Stat getStat() const;

    /**
     * @brief Returns the original move for the message
     *
     */
    pplmn::MoveId getOriginalMove() const;

    /**
     * @brief Returns the new move for the message
     *
     */
    pplmn::MoveId getNewMove() const;

    /**
     * @brief Returns the integer bundled with this message
     *
     */
    std::int16_t getInt() const;

    /**
     * @brief Returns the unsigned int value stored in this message
     *
     */
    std::uint16_t getUnsigned() const;

private:
    struct Empty {};

    Type type;
    std::variant<Empty, pplmn::Ailment, pplmn::PassiveAilment, pplmn::MoveId, pplmn::Stat,
                 std::pair<pplmn::MoveId, pplmn::MoveId>, std::int16_t, std::uint16_t>
        data;
    union {
        bool forActive;
        std::uint8_t pplIndex;
    };
};

} // namespace cmd
} // namespace battle
} // namespace core

#endif
