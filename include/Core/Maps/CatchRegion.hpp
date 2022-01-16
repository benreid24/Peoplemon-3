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
namespace binary
{
template<>
struct SerializableObject<core::map::CatchRegion> : public SerializableObjectBase {
    using Region = core::map::CatchRegion;
    using Wild   = core::pplmn::WildPeoplemon;

    SerializableField<1, Region, std::string> name;
    SerializableField<2, Region, std::vector<Wild>> wilds;

    SerializableObject()
    : name(*this, &Region::name)
    , wilds(*this, &Region::wilds) {}
};
} // namespace binary
} // namespace serial
} // namespace bl

#endif
