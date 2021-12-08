#ifndef CORE_FILES_BEHAVIOR_HPP
#define CORE_FILES_BEHAVIOR_HPP

#include <BLIB/Serialization/Binary.hpp>
#include <Core/Components/Direction.hpp>
#include <cstdint>
#include <variant>

namespace core
{
namespace file
{
/**
 * @brief Set of behaviors for NPCs and trainers
 *
 * @ingroup Files
 *
 */
class Behavior {
public:
    /// The type of behavior
    enum Type : std::uint8_t {
        /// The character will stand and face a given direction
        StandStill = 0,

        /// The character will spin in place
        SpinInPlace = 1,

        /// The character will follow a preset path
        FollowingPath = 2,

        /// The character is allowed to wander freely
        Wandering = 3
    };

    /// Contains data if the behavior type is StandStill
    struct Standing {
        /// The direction to face
        component::Direction facedir;

        /// Creates the data
        Standing(component::Direction d);
    };

    /// Contains data for when the behavior type is SpinInPlace
    struct Spinning {
        /// The direction to spin
        enum Direction : std::uint8_t {
            /// The character spins clockwise
            Clockwise = 0,

            /// The character spins counterclockwise
            Counterclockwise = 1,

            /// The character spins randomly
            Random = 2
        } spinDir;

        /// Creates the data
        Spinning(Direction dir);
    };

    /// Contains data for when the behavior type is following a path
    struct Path {
        /// Represents a straight section of path
        struct Pace {
            /// The direction to walk in
            component::Direction direction;

            /// The number of steps to take. No need to account for direction change
            std::uint16_t steps;

            /**
             * @brief Construct a new Pace
             *
             * @param dir The direction to walk in
             * @param steps The number of steps to take
             */
            Pace(component::Direction dir, std::uint16_t steps);

            /**
             * @brief Makes a single step up
             *
             */
            Pace();
        };

        /// The sections of the path
        std::vector<Pace> paces;

        /// True if the path should be done in reverse when completed, false to loop
        bool reverse;

        /// Makes an empty path
        Path();
    };

    /// Contains data for when the behavior type is Wandering
    struct Wander {
        /// The radius to stay within, in tiles
        unsigned int radius;

        /// Creates the data
        Wander(unsigned int r);
    };

    /**
     * @brief Empty behavior. Standing still facing down
     *
     */
    Behavior();

    /**
     * @brief Loads the behavior data from the legacy formatted file. Use serialize and deserialize
     *        for reading and writing the new format
     *
     * @param input The input file
     * @return True if data was read, false on error
     */
    bool legacyLoad(bl::serial::binary::InputStream& input);

    /**
     * @brief The type of behavior this is
     *
     */
    Type type() const;

    /**
     * @brief Set the Type of behavior. Existing behavior data is cleared
     *
     * @param type The type of behavior to set to
     */
    void setType(Type type);

    /**
     * @brief The data for standing still
     *
     */
    Standing& standing();

    /**
     * @brief The data for spinning
     *
     */
    Spinning& spinning();

    /**
     * @brief The data for path following
     *
     */
    Path& path();

    /**
     * @brief The data for wandering
     *
     */
    Wander& wander();

    /**
     * @brief The data for standing still
     *
     */
    const Standing& standing() const;

    /**
     * @brief The data for spinning
     *
     */
    const Spinning& spinning() const;

    /**
     * @brief The data for path following
     *
     */
    const Path& path() const;

    /**
     * @brief The data for wandering
     *
     */
    const Wander& wander() const;

private:
    Type _type;
    std::variant<Standing, Spinning, Path, Wander> data;

    friend class bl::serial::binary::Serializer<Behavior>;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct SerializableObject<core::file::Behavior::Path::Pace> : public SerializableObjectBase {
    using P = core::file::Behavior::Path::Pace;

    SerializableField<1, core::component::Direction, offsetof(P, direction)> direction;
    SerializableField<2, std::uint16_t, offsetof(P, steps)> steps;

    SerializableObject()
    : direction(*this)
    , steps(*this) {}
};

template<>
struct Serializer<core::file::Behavior> {
    using B = core::file::Behavior;

    static bool deserialize(InputStream& in, B& b) {
        if (!Serializer<B::Type>::deserialize(in, b._type)) return false;

        switch (b._type) {
        case B::Type::StandStill: {
            core::component::Direction dir;
            if (!Serializer<core::component::Direction>::deserialize(in, dir)) return false;
            b.data.emplace<B::Standing>(dir);
            return true;
        }
        case B::Type::SpinInPlace: {
            B::Spinning::Direction dir;
            if (!Serializer<B::Spinning::Direction>::deserialize(in, dir)) return false;
            b.data.emplace<B::Spinning>(dir);
            return true;
        }
        case B::Type::Wandering: {
            std::uint16_t rad;
            if (!in.read<std::uint16_t>(rad)) return false;
            b.data.emplace<B::Wander>(rad);
            return true;
        }
        case B::Type::FollowingPath: {
            b.data.emplace<B::Path>();
            auto& path = std::get<B::Path>(b.data);

            std::uint8_t rev;
            if (!in.read<std::uint8_t>(rev)) return false;
            path.reverse = rev != 0;

            return Serializer<std::vector<B::Path::Pace>>::deserialize(in, path.paces);
        }
        default:
            return false;
        }
    }

    static bool serialize(OutputStream& out, const B& b) {
        if (!Serializer<B::Type>::serialize(out, b._type)) return false;

        switch (b._type) {
        case B::Type::StandStill: {
            const auto& s = std::get<B::Standing>(b.data);
            return Serializer<core::component::Direction>::serialize(out, s.facedir);
        }
        case B::Type::SpinInPlace: {
            const auto& s = std::get<B::Spinning>(b.data);
            return Serializer<B::Spinning::Direction>::serialize(out, s.spinDir);
        }
        case B::Type::Wandering: {
            const auto& w = std::get<B::Wander>(b.data);
            return out.write<std::uint16_t>(w.radius);
        }
        case B::Type::FollowingPath: {
            const auto& path = std::get<B::Path>(b.data);
            if (!out.write<std::uint8_t>(path.reverse ? 1 : 0)) return false;
            return Serializer<std::vector<B::Path::Pace>>::serialize(out, path.paces);
        }
        default:
            return false;
        }
    }

    static std::size_t size(const B& b) {
        std::size_t sz = Serializer<B::Type>::size(b._type);

        switch (b._type) {
        case B::Type::StandStill: {
            const auto& s = std::get<B::Standing>(b.data);
            sz += Serializer<core::component::Direction>::size(s.facedir);
            break;
        }
        case B::Type::SpinInPlace: {
            const auto& s = std::get<B::Spinning>(b.data);
            sz += Serializer<B::Spinning::Direction>::size(s.spinDir);
            break;
        }
        case B::Type::Wandering: {
            sz += sizeof(std::uint16_t);
            break;
        }
        case B::Type::FollowingPath: {
            const auto& path = std::get<B::Path>(b.data);
            sz += sizeof(std::uint8_t);
            sz += Serializer<std::vector<B::Path::Pace>>::size(path.paces);
            break;
        }
        default:
            return 0;
        }
        return sz;
    }
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
