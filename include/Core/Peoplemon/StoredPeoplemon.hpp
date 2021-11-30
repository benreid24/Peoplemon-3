#ifndef CORE_PEOPLEMON_STOREDPEOPLEMON_HPP
#define CORE_PEOPLEMON_STOREDPEOPLEMON_HPP

#include <SFML/System/Vector2.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a Peoplemon in storage
 * 
 * @ingroup Peoplemon
 * 
 */
struct StoredPeoplemon {
    /// The peoplemon itself
    OwnedPeoplemon peoplemon;

    /// Which box the peoplemon is in
    unsigned int boxNumber;

    /// The position of the peoplemon in the box
    sf::Vector2i position;
};

}
}

#endif
