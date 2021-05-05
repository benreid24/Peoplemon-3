#include <Core/Player/Input/Listener.hpp>

#include <Core/Player/Input.hpp>

namespace core
{
namespace player
{
namespace input
{
Listener::Listener()
: owner(nullptr) {}

Listener::Listener(const Listener& c)
: owner(c.owner) {
    if (owner) owner->replaceListener(c, *this);
}

Listener::Listener(Listener&& c)
: owner(c.owner) {
    if (owner) owner->replaceListener(c, *this);
}

Listener::~Listener() {
    if (owner) owner->removeListener(*this);
}

} // namespace input
} // namespace player
} // namespace core
