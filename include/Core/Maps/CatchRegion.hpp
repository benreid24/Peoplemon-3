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
 */
struct CatchRegion {
    std::string name;
    std::vector<pplmn::WildPeoplemon> wilds;
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
    : name("name", *this, &Region::name, SerializableFieldBase::Required{})
    , wilds("wilds", *this, &Region::wilds, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
