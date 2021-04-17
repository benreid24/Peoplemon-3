#include <Core/Maps/CatchZone.hpp>

namespace core
{
namespace map
{
CatchZone::CatchZone()
: area(*this)
, peoplemon(*this) {}

CatchZone::CatchZone(const CatchZone& copy)
: CatchZone() {
    area      = copy.area.getValue();
    peoplemon = copy.peoplemon.getValue();
}

CatchZone& CatchZone::operator=(const CatchZone& copy) {
    area      = copy.area.getValue();
    peoplemon = copy.peoplemon.getValue();
    return *this;
}

void CatchZone::activate() {
    // TODO - load wild peoplemon
}

} // namespace map
} // namespace core
