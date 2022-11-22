#ifndef CORE_PLAYER_PEOPLEDEX_HPP
#define CORE_PLAYER_PEOPLEDEX_HPP

#include <Core/Peoplemon/Id.hpp>
#include <string>
#include <unordered_map>

namespace core
{
namespace system
{
class Player;
}
namespace player
{
struct State;

/**
 * @brief Maintains the information required to populate the peopledex
 *
 * @ingroup Player
 *
 */
class Peopledex {
public:
    /// @brief Represents an intel level on a given peoplemon
    enum IntelLevel { NoIntel, LimitedIntel, FullIntel };

    /**
     * @brief Construct a new Peopledex object
     *
     * @param playerState The player state
     */
    Peopledex(State& playerState);

    /**
     * @brief Returns how many of the given peoplemon have been seen
     *
     * @param peoplemon The peoplemon to search for
     * @return unsigned int The amount seen
     */
    unsigned int getSeen(pplmn::Id peoplemon) const;

    /**
     * @brief Returns how many of the given peoplemon have been caught
     *
     * @param peoplemon The peoplemon to search for
     * @return unsigned int The amount caught
     */
    unsigned int getCaught(pplmn::Id peoplemon) const;

    /**
     * @brief Returns the name of the location the given peoplemon was first spotted at
     *
     * @param peoplemon The peoplemon to search
     * @return const std::string& The location first seen at. May be empty
     */
    const std::string& getFirstSeenLocation(pplmn::Id peoplemon) const;

    /**
     * @brief Returns the level of intel on the given peoplemon
     *
     * @param peoplemon The peoplemon to get the intel level on
     * @return IntelLevel The intel level on the given peoplemon
     */
    IntelLevel getIntelLevel(pplmn::Id peoplemon) const;

    /**
     * @brief Registers a sighting of the given peoplemon at the given location
     *
     * @param peoplemon The peoplemon that was seen
     * @param location Where the peoplemon was seen
     */
    void registerSighting(pplmn::Id peoplemon, const std::string& location);

private:
    State& playerState;
    std::unordered_map<pplmn::Id, std::uint32_t> seenCounts;
    std::unordered_map<pplmn::Id, std::string> firstSawLocations;

    friend class system::Player;
};
} // namespace player
} // namespace core

#endif
