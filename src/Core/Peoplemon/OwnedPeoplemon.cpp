#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace pplmn
{
OwnedPeoplemon::OwnedPeoplemon()
: id(Id::Unknown)
, level(1)
, xp(0)
, currentHp(0)
, ailment(Ailment::None)
, item(item::Id::None) {}

OwnedPeoplemon::OwnedPeoplemon(Id ppl, unsigned int l)
: OwnedPeoplemon() {
    id        = ppl;
    level     = l;
    currentHp = currentStats().hp;
}

const std::string& OwnedPeoplemon::name() const {
    return customName.empty() ? Peoplemon::name(id) : customName;
}

} // namespace pplmn
} // namespace core
