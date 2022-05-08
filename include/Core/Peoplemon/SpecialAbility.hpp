#ifndef CORE_PEOPLEMON_ABILITY_HPP
#define CORE_PEOPLEMON_ABILITY_HPP

#include <cstdint>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a special ability that a Peoplemon may have
 *
 * @ingroup Peoplemon
 *
 */
enum struct SpecialAbility : std::uint8_t {
    None             = 0,
    BoardGameMaster  = 1,
    Chillaxed        = 2,
    Classy           = 3,
    Goon             = 4,
    QuickDraw        = 5,
    AlwaysFriendly   = 6,
    Sassy            = 7,
    ImpulseBuy       = 8,
    RunAway          = 9,
    MrExtra          = 10,
    BeefedUp         = 11,
    Reserved         = 12,
    DukeOfJokes      = 13,
    Engaging         = 14,
    SnackShare       = 15,
    DerpDerp         = 16,
    Klutz            = 17,
    GenderBend       = 18,
    Garbage          = 19,
    BikeMechanic     = 20,
    SidetrackTeach   = 21,
    NoJokeTeach      = 22,
    FieryTeach       = 23,
    ExperiencedTeach = 24,
    NewTeach         = 25,
    DozeOff          = 26,
    DouseFlames      = 27,
    Flirty           = 28,
    UndyingFaith     = 29,
    TooCool          = 30,
    FakeStudy        = 31,
    Alcoholic        = 32,
    TotalBro         = 33,
    TotalMom         = 34,
    CantSwim         = 35,
    AllNighter       = 36,
    AilmentSaturated = 37,
    Adament          = 38,
    AbsolutePitch    = 39,
    GamemakerVirus   = 40,
    Snapshot         = 41,
    GetBaked         = 42
};

} // namespace pplmn
} // namespace core

#endif
