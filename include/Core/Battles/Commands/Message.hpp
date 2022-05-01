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
        EncoreFailed
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
     * @param type
     * @param stat
     * @param forActiveBattler
     */
    Message(Type type, pplmn::Stat stat, bool forActiveBattler);

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

private:
    struct Empty {};

    Type type;
    std::variant<Empty, pplmn::Ailment, pplmn::PassiveAilment, pplmn::MoveId, pplmn::Stat> data;
    bool forActive;
};

} // namespace cmd
} // namespace battle
} // namespace core

#endif
