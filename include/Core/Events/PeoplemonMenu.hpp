#ifndef CORE_EVENTS_PEOPLEMONMENU_HPP
#define CORE_EVENTS_PEOPLEMONMENU_HPP

namespace core
{
namespace event
{
/**
 * @brief Special event to trigger the opening of the peoplemon menu
 *
 * @ingroup Events
 *
 */
struct OpenPeoplemonMenu {
    /**
     * @brief Represents where the menu is being opened from
     *
     */
    enum struct Context {
        BattleSwitch,
        BattleFaint,
        BattleMustSwitch,
        BattleReviveSwitch,
        StorageSelect,
        PauseMenu,
        GiveItem
    };

    /**
     * @brief Construct a new Open Peoplemon Menu event
     *
     * @param ctx The context in which the menu is opened for
     * @param outNow Index of the peoplemon out now
     * @param chosen To store the index of the selected peoplemon
     */
    OpenPeoplemonMenu(Context ctx, int outNow = -1, int* chosen = nullptr)
    : context(ctx)
    , outNow(outNow)
    , chosen(chosen) {}

    const Context context;
    const int outNow;
    int* chosen;
};
} // namespace event
} // namespace core

#endif
