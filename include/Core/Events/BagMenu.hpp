#ifndef CORE_EVENTS_BAGMENU_HPP
#define CORE_EVENTS_BAGMENU_HPP

#include <Core/Items/Id.hpp>

namespace core
{
namespace event
{
/**
 * @brief Event that is fired when the bag menu should be opened
 *
 * @ingroup Events
 *
 */
struct OpenBagMenu {
    /// Represents how the menu should be opened
    enum struct Context { BattleUse, PauseMenu };

    /**
     * @brief Construct a new Open Bag Menu event
     *
     * @param ctx The context to open the menu with
     * @param result Where to store the chosen item
     * @param outNow Which peoplemon is out now
     * @param chosenPeoplemon Where to store the chosen peoplemon
     */
    OpenBagMenu(Context ctx, item::Id* result = nullptr, int outNow = -1,
                int* chosenPeoplemon = nullptr)
    : context(ctx)
    , result(result)
    , outNow(outNow)
    , chosenPeoplemon(chosenPeoplemon) {}

    const Context context;
    item::Id* const result;
    const int outNow;
    int* const chosenPeoplemon;
};

} // namespace event
} // namespace core

#endif
