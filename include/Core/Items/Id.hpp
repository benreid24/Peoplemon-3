#ifndef CORE_ITEMS_ID_HPP
#define CORE_ITEMS_ID_HPP

#include <cstdint>

/**
 * @addtogroup Items
 * @ingroup Core
 * @brief Collection of enums and classes for managing and using items
 *
 */

namespace core
{
/// Collection of enums and classes for managing and using items
namespace item
{
/**
 * @brief Represents an item in its simplist form
 *
 * @ingroup Items
 *
 */
enum struct Id : std::uint16_t {
    Unknown = 0,
    None    = Unknown,

    // Regular items
    Potion                = 1,
    SuperPotion           = 2,
    MegaPotion            = 3,
    SuperMegaUltraPotion  = 4,
    Terriball             = 5,
    Peopleball            = 6,
    GreatPeopleball       = 7,
    UltraPeopleball       = 8,
    PairOfPeopleballs     = 9,
    ClockPeopleball       = 10,
    Intelligiball         = 11,
    Gulliball             = 12,
    QuistionableAbuseBall = 13,
    BullyBall             = 14,
    CloneBall             = 15,
    MasterPeopleball      = 16,
    LegendBall            = 17,
    PpPack                = 18,
    Pp6Pack               = 19,
    SuperPpPack           = 20,
    SuperPp6Pack          = 21,
    KegOfProtein          = 22,
    SuperPowerJuice       = 23,
    TubOfIcedCream        = 24,
    JarOfEncouragement    = 25,
    SuperSpeedJuice       = 26,
    Compliments           = 27,
    FriendshipStone       = 28,
    SupahCoolStone        = 29,
    OooFancyStone         = 30,
    SupahFunnyStone       = 31,
    PartyStone            = 32,
    ShadyMushroom         = 33,
    BigShadyMushroom      = 34,
    GoAwaySpray           = 35,
    SuperGoAwaySpray      = 36,
    NeverComeBackSpray    = 37,
    PpRaiser              = 38,
    UnAnnoyerSoda         = 39,
    UnFrustratorSoda      = 40,
    WakeUpSoda            = 41,
    UnStickySoda          = 42,
    UnFreezeSoda          = 43,
    Log                   = 44,

    // Hold items
    BagOfGoldFish       = 50,
    BackwardsHoody      = 51,
    Glasses             = 52,
    Spoon               = 53,
    SlappingGlove       = 54,
    SuperTinyMiniFridge = 55,
    SketchySack         = 56,
    GoldfishCracker     = 57,
    SpeedJuice          = 58,
    PowerJuice          = 59,
    IcedTea             = 60,
    WakeUpBelle         = 61,

    // Key items
    Package               = 101,
    SackOfGreenbacks      = 102,
    FancyFossil           = 103,
    PartyFossil           = 104,
    Teleporter            = 105,
    Map                   = 106,
    TransportationCrystal = 107,
    Penny                 = 108,
    MeatAndPotatoes       = 109,
    TriliteCookie         = 110,
    JesusShoes            = 111,
    Lantern               = 112,
    OldYellowKey          = 113,
    OldBlueKey            = 114,
    OldRedKey             = 115,
    OldFishingRod         = 116,
    Shoe                  = 117,
    BigRedButton          = 118,
    MiniFridge            = 119,
    OldRelic              = 120,
    DoorKey               = 121,
    DoorKey2              = 122,
    RescuePorter          = 123,
    JesusShoesUpgrade     = 124,
    BoatPart              = 125,

    ID_COUNT
};

} // namespace item
} // namespace core

#endif
