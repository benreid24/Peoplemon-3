#include <Core/Maps/Event.hpp>

namespace core
{
namespace map
{
Event::Event()
: trigger(Trigger::OnEnter) {}

Event::Event(const std::string& s, const sf::Vector2i& pos, const sf::Vector2i& sz, Trigger t)
: script(s)
, position(pos)
, areaSize(sz)
, trigger(t) {}

} // namespace map
} // namespace core
