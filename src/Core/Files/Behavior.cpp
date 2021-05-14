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
    std::uint8_t behaviorType;
    if (!input.read(behaviorType)) return false;
    switch (behaviorType) {
    case 0:
        setType(StandStill);
        break;
    case 1:
        setType(SpinInPlace);
        break;
    case 2:
        setType(FollowingPath);
        break;
    case 3:
        setType(Wandering);
        break;
    default:
        BL_LOG_ERROR << "Unrecognized behavior: " << behaviorType;
        return false;
    }

    switch (_type) {
    case StandStill:
        return true;

    case SpinInPlace: {
        std::uint8_t spinDir;
        if (!input.read<std::uint8_t>(spinDir)) return false;
        spinning().spinDir = static_cast<Spinning::Direction>(spinDir);
    }
        return true;

    case FollowingPath: {
        std::uint8_t paceCount;
        std::uint8_t backwards;
        if (!input.read<std::uint8_t>(paceCount)) return false;
        if (!input.read<std::uint8_t>(backwards)) return false;
        path().reverse = backwards == 1;

        path().paces.reserve(paceCount);
        for (std::uint8_t i = 0; i < paceCount; ++i) {
            std::uint8_t dir;
            std::uint8_t steps;
            if (!input.read<std::uint8_t>(dir)) return false;
            if (!input.read<std::uint8_t>(steps)) return false;
            path().paces.emplace_back(static_cast<component::Direction>(dir), steps);
        }
    }
        return true;

    case Wandering: {
        std::uint8_t radius;
        if (!input.read<std::uint8_t>(radius)) return false;
        wander().radius = radius;
    }
        return true;

    default:
        return false;
    }
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
        BL_LOG_ERROR << "Unknown behavior type: " << t;
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
