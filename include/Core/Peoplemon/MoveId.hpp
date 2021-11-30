#ifndef CORE_PEOPLEMON_MOVEID_HPP
#define CORE_PEOPLEMON_MOVEID_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief The id of a move
 *
 * @ingroup Peoplemon
 *
 */
enum struct MoveId : std::uint8_t {
    Unknown = 0,

    _INVALID_1 = 1,

    Absent = 2,
    LateArrival,
    EarlyDismissal,
    RedHairGlare,
    LateHW,
    Frost,
    MakeDue,
    Optimism,
    Sarcasm,
    Distract,
    OffTopic,
    Awkwardness,
    Teach,
    MegaTeach,
    SuperTeach,
    ExtremeTeach,
    Frustration,
    Negation,
    Inquisitive,
    Logic,
    YellAtChris,
    SodaBomb,
    ParabolaAttack,
    BadJoke,
    I4Got,
    SkimpOut,
    LocusStrike,
    SidesOfACircle,
    Insanitize,
    FloatingSymbols,
    FalseName,
    Confuse,

    _INVALID_2 = 34,

    MathGuitar = 35,
    EssayStrike,
    DivideBy0,
    Athleticism,
    WorkOut,
    Quadratic,
    CoreDump,
    SuckyMove,
    Learn,
    GUIStrike,
    Harden,
    Nudge,
    TrustFall,
    SuperTrustFall,
    HealBelle,
    Retire,
    Demotivate,
    WetWilly,
    BackSass,
    TheChest,
    Dominize,
    Rave,
    Hope,
    BagSwipe,
    Concentrate,

    _INVALID_3 = 60,

    Pray = 61,
    LongStory,
    TrackFu,
    Kick,
    Oblivious,
    Carpool,
    BlackSass,
    SassySass,
    Borrow,
    Chill,
    Nap,
    Excuse,
    ThrowRock,
    Ambiguous,
    Persistent,
    Replace,
    Lastattempt,
    Patience,
    Jog,
    MovetoNC,
    Camp,
    MakeBank,
    Set,
    Spike,
    Bump,
    LockIn,
    RollD20,
    SilentCoolness,
    ChannelNinja,
    Serenade,
    GuiltTrip,
    DrunkFireworksDisplay,
    HumbleBrag,
    Peanut,
    SkypeGames,
    Mooch,
    GMPCannon,
    BoardGameBarrage,
    ChannelRedneck,
    SuperFan,
    CipherStrike,
    Bromance,
    MedicalAttention,
    Bark,
    Hospitality,
    HappyGoLucky,
    Encourage,
    EnergyBlast,
    ShoulderShrug,
    Cabinets,
    PinsAndNeedles,
    Block,
    Tool,
    Reliable,
    SubtleJoke,

    _NUM_MOVES
};

} // namespace pplmn
} // namespace core

#endif
