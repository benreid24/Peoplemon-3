#include <Core/Maps/Event.hpp>

namespace core
{
namespace map
{
Event::Event()
: trigger(*this)
, position(*this)
, size(*this)
, script(*this) {}

Event::Event(const std::string& s, const sf::Vector2i& pos, const sf::Vector2i& sz, Trigger t)
: Event() {
    script   = s;
    position = pos;
    size     = sz;
    trigger  = t;
}

Event::Event(const Event& copy)
: Event() {
    *this = copy;
}

Event& Event::operator=(const Event& copy) {
    script   = copy.script;
    position = copy.position;
    size     = copy.size;
    trigger  = copy.trigger;
    return *this;
}

} // namespace map
} // namespace core
