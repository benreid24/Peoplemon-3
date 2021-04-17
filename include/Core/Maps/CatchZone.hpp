#ifndef CORE_MAPS_CATCHZONE_HPP
#define CORE_MAPS_CATCHZONE_HPP

#include <BLIB/Files/Binary.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief Represents a zone containing information about wild peoplemon that can appear in a Map
 *
 * @ingroup Maps
 *
 */
struct CatchZone : public bl::file::binary::SerializableObject {
    bl::file::binary::SerializableField<1, sf::IntRect> area;
    bl::file::binary::SerializableField<2, std::vector<std::string>> peoplemon;

    /**
     * @brief Initializes the serializer data and creates an empty zone
     *
     */
    CatchZone();

    /**
     * @brief Copy constructs the catch zone
     *
     * @param copy The CatchZone to copy
     */
    CatchZone(const CatchZone& copy);

    /**
     * @brief Copies from the given catch zone
     *
     * @param copy The CatchZone to copy
     * @return CatchZone& A reference to this zone
     */
    CatchZone& operator=(const CatchZone& copy);

    /**
     * @brief Must be called once the map is loaded. This loads the wild peoplemon information
     *
     */
    void activate();

    // TODO - method to generate the people that jumps out
};

} // namespace map
} // namespace core

#endif
