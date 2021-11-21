#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/OwnedMove.hpp>

namespace core
{
namespace pplmn
{
OwnedMove::OwnedMove()
: id(MoveId::Unknown)
, curPP(0)
, maxPP(0) {}

OwnedMove::OwnedMove(MoveId id)
: id(id)
, curPP(Move::pp(id))
, maxPP(Move::pp(id)) {}

} // namespace pplmn
} // namespace core
