#ifndef CORE_MAPS_CATCHREGION_HPP
#define CORE_MAPS_CATCHREGION_HPP

#include <Core/Peoplemon/WildPeoplemon.hpp>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief Represents a class of catchable peoplemon
 *
 * @ingroup Maps
 * @ingroup Peoplemon
 *
 */
struct CatchRegion {
    std::string name;
    std::vector<pplmn::WildPeoplemon> wilds;

    /**
     * @brief Selects a wild peoplemon from the set based on frequency, or returns a level 100 Ben
     *        if the set is empty
     *
     * @return const pplmn::WildPeoplemon&
     */
    const pplmn::WildPeoplemon& selectWild() const;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::CatchRegion> : public SerializableObjectBase {
    using Region = core::map::CatchRegion;
    using Wild   = core::pplmn::WildPeoplemon;

    SerializableField<1, Region, std::string> name;
    SerializableField<2, Region, std::vector<Wild>> wilds;

    SerializableObject()
    : SerializableObjectBase("CatchRegion")
    , name("name", *this, &Region::name, SerializableFieldBase::Required{})
    , wilds("wilds", *this, &Region::wilds, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
