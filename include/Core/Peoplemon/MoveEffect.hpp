#ifndef CORE_PEOPLEMON_MOVEEFFECT_HPP
#define CORE_PEOPLEMON_MOVEEFFECT_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents an effect that a move can have. Copied verbatim from Peoplemon 2, may refactor
 *        later when implementing battles
 *
 * @ingroup Peoplemon
 *
 */
enum struct MoveEffect : std::uint8_t {
    None                 = 0,
    Heal                 = 1,
    Poison               = 2,
    Burn                 = 3,
    Paralyze             = 4,
    Freeze               = 5,
    Confuse              = 6,
    LeechSeed            = 7,
    Flinch               = 8,
    Trap                 = 9,
    Sleep                = 10,
    Protection           = 11,
    Safegaurd            = 12,
    Substitute           = 13,
    HealBell             = 14,
    CritUp               = 15,
    AtkUp                = 16,
    DefUp                = 17,
    AccUp                = 18,
    EvdUp                = 19,
    SpdUp                = 20,
    SpAtkUp              = 21,
    SpDefUp              = 22,
    CritDown             = 23,
    AtkDown              = 24,
    DefDown              = 25,
    AccDown              = 26,
    EvdDown              = 27,
    SpdDown              = 28,
    SpAtkDown            = 29,
    SpDefDown            = 30,
    Recoil               = 31,
    Charge               = 32,
    Suicide              = 33,
    Counter              = 34,
    MirrorCoat           = 35,
    OnlySleeping         = 36,
    Peanut               = 37,
    SetBall              = 38,
    WakeBoth             = 39,
    HealPercent          = 40,
    Encore               = 41,
    RandomMove           = 42,
    BatonPass            = 43,
    DieIn3Turns          = 44,
    CritEvdUp            = 45,
    BumpBall             = 46,
    SpikeBall            = 47,
    DeathSwap            = 48,
    Gamble               = 49,
    StayAlive            = 50,
    MaxAtkMinAcc         = 51,
    FrustConfuse         = 52,
    Spikes               = 53,
    DoubleFamily         = 54,
    EnemyPPDown          = 55,
    HealNext             = 56,
    Roar                 = 57,
    FailOnMove64         = 58,
    SleepHeal            = 59,
    SpdAtkUp             = 60,
    StealStats           = 61,
    BlockBall            = 62, // not used explicitly; affects other effects
    SwipeBall            = 63,
    DamageThenSwitch     = 64,
    RoarCancelBallSpikes = 65
};

} // namespace pplmn
} // namespace core

#endif
