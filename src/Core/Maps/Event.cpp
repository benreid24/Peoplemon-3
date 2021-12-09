#include <Core/Maps/Event.hpp>

namespace core
{
namespace map
{
Event::Event()
: trigger(Trigger::OnEnter) {}

Event::Event(const std::string& s, const sf::Vector2i& pos, const sf::Vector2i& sz, Trigger t)
: trigger(t)
, position(pos)
, areaSize(sz)
, script(s) {}

} // namespace map
} // namespace core
