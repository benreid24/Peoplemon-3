#ifndef CORE_PLAYER_STORAGESYSTEM_HPP
#define CORE_PLAYER_STORAGESYSTEM_HPP

#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/StoredPeoplemon.hpp>

namespace core
{
namespace system
{
class Player;
}

namespace player
{
/**
 * @brief Peoplemon storage system for extra peoplemon the player has
 *
 * @ingroup Player
 *
 */
class StorageSystem {
public:
    static constexpr int BoxCount  = 14;
    static constexpr int BoxWidth  = 10;
    static constexpr int BoxHeight = 10;

    /**
     * @brief Construct a new Storage System
     *
     */
    StorageSystem();

    /**
     * @brief Adds the given peoplemon to storage in a free slot
     *
     * @param ppl The peoplemon to add
     * @return True if the peoplemon could be stored, false if full
     */
    bool add(const pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Adds the peoplemon to storage at the given position. Does not check if the position is
     *        available, the caller must ensure that it is free.
     *
     * @param box The box to add to [0, 13]
     * @param pos The position in the box
     * @param ppl The peoplemon to add
     */
    void add(unsigned int box, const sf::Vector2i& pos, const pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Clears the peoplemon from the given position, if one is there
     *
     * @param box The box to remove from
     * @param position The position to remove from
     */
    void remove(unsigned int box, const sf::Vector2i& position);

    /**
     * @brief Returns whether or not the given storage space is free
     *
     * @param box The box number to check
     * @param x The x position
     * @param y The y position
     * @return True if that slot is open, false otherwise
     */
    bool spaceFree(int box, int x, int y) const;

    // TODO - methods required for UI and management

private:
    std::array<std::vector<pplmn::StoredPeoplemon>, BoxCount> boxes;

    friend class system::Player;
};

} // namespace player
} // namespace core

#endif
