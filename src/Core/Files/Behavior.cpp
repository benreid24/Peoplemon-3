#include <Core/Files/Behavior.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace file
{
Behavior::Behavior()
: _type(Type::StandStill)
, data(Standing(component::Direction::Down)) {}

bool Behavior::legacyLoad(bl::file::binary::File& input) {
    // TODO
    return false;
}

Behavior::Type Behavior::type() const { return _type; }

void Behavior::setType(Type t) {
    _type = t;
    switch (t) {
    case StandStill:
        data.emplace<Standing>(component::Direction::Down);
        break;

    case SpinInPlace:
        data.emplace<Spinning>(Spinning::Clockwise);
        break;

    case FollowingPath:
        data.emplace<Path>();
        break;

    case Wandering:
        data.emplace<Wander>(10);
        break;

    default:
        BL_LOG_ERROR << "Unknown behavior type: " << type;
        break;
    }
}

Behavior::Standing& Behavior::standing() { return std::get<Standing>(data); }

const Behavior::Standing& Behavior::standing() const { return std::get<Standing>(data); }

Behavior::Spinning& Behavior::spinning() { return std::get<Spinning>(data); }

const Behavior::Spinning& Behavior::spinning() const { return std::get<Spinning>(data); }

Behavior::Path& Behavior::path() { return std::get<Path>(data); }

const Behavior::Path& Behavior::path() const { return std::get<Path>(data); }

Behavior::Wander& Behavior::wander() { return std::get<Wander>(data); }

const Behavior::Wander& Behavior::wander() const { return std::get<Wander>(data); }

Behavior::Standing::Standing(component::Direction d)
: facedir(d) {}

Behavior::Spinning::Spinning(Direction d)
: spinDir(d) {}

Behavior::Path::Path()
: reverse(true) {}

Behavior::Wander::Wander(unsigned int r)
: radius(r) {}

Behavior::Path::Pace::Pace(component::Direction dir, unsigned int s)
: direction(dir)
, steps(s) {}

} // namespace file
} // namespace core
