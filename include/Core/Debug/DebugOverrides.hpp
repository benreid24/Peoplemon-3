#ifndef GAME_BATTLES_BATTLESKIPPER_HPP
#define GAME_BATTLES_BATTLESKIPPER_HPP

#include <BLIB/Events.hpp>
#include <Core/Debug/DebugBanner.hpp>
#include <SFML/Window/Event.hpp>

#ifdef PEOPLEMON_DEBUG

/**
 * @addtogroup Debug
 * @ingroup Core
 * @brief Collection of debug settings and utilities
 *
 */

namespace core
{
/// @brief  Collection of debug utilities
namespace debug
{
/**
 * @brief A debug-only utility for storing various override flags and settings
 *
 * @ingroup Debug
 *
 */
struct DebugOverrides : private bl::event::Listener<sf::Event> {
public:
    /// @brief Whether or not to skip through all battles
    bool skipBattles;

    /// @brief Whether or not wild peoplemon should always be caught
    bool alwaysCatch;

    /// @brief Whether or not Run should always succeed
    bool alwaysRun;

    /**
     * @brief Returns the global debug override settings
     *
     */
    static const DebugOverrides& get();

    /**
     * @brief Subscribes this class to the event bus to toggle settings
     *
     * @param bus The event bus to subscribe to
     */
    static void subscribe(bl::event::Dispatcher& bus);

private:
    DebugOverrides();
    static DebugOverrides& instance();
    virtual void observe(const sf::Event& event) override;
};

} // namespace debug
} // namespace core

#endif
#endif
