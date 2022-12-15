#ifndef CORE_FILES_BEHAVIOR_HPP
#define CORE_FILES_BEHAVIOR_HPP

#include <BLIB/Serialization.hpp>
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

        /// Stand facing down
        Standing();
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

        /// Spin randomly
        Spinning();
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
        std::uint32_t radius;

        /// Creates the data
        Wander(std::uint32_t r);

        /// Wander in a 10 tile radius
        Wander();
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

    friend struct bl::serial::binary::Serializer<Behavior>;
    friend struct bl::serial::SerializableObject<Behavior>;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::file::Behavior::Path::Pace> : public SerializableObjectBase {
    using P = core::file::Behavior::Path::Pace;

    SerializableField<1, P, core::component::Direction> direction;
    SerializableField<2, P, std::uint16_t> steps;

    SerializableObject()
    : SerializableObjectBase("BehaviorPathPace")
    , direction("direction", *this, &P::direction, SerializableFieldBase::Required{})
    , steps("steps", *this, &P::steps, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::Behavior::Path> : public SerializableObjectBase {
    using P = core::file::Behavior::Path;

    SerializableField<1, P, std::vector<P::Pace>> paces;
    SerializableField<2, P, bool> reverse;

    SerializableObject()
    : SerializableObjectBase("BehaviorPath")
    , paces("paces", *this, &P::paces, SerializableFieldBase::Required{})
    , reverse("reverse", *this, &P::reverse, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::Behavior::Standing> : public SerializableObjectBase {
    using S = core::file::Behavior::Standing;

    SerializableField<1, S, core::component::Direction> direction;

    SerializableObject()
    : SerializableObjectBase("BehaviorStanding")
    , direction("direction", *this, &S::facedir, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::Behavior::Spinning> : public SerializableObjectBase {
    using S = core::file::Behavior::Spinning;

    SerializableField<1, S, S::Direction> direction;

    SerializableObject()
    : SerializableObjectBase("BehaviorSpinning")
    , direction("direction", *this, &S::spinDir, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::Behavior::Wander> : public SerializableObjectBase {
    using W = core::file::Behavior::Wander;

    SerializableField<1, W, std::uint32_t> radius;

    SerializableObject()
    : SerializableObjectBase("BehaviorWander")
    , radius("radius", *this, &W::radius, SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<core::file::Behavior> : public SerializableObjectBase {
    using B = core::file::Behavior;

    SerializableField<1, B, B::Type> type;
    SerializableField<2, B, decltype(B::data)> data;

    SerializableObject()
    : SerializableObjectBase("Behavior")
    , type("type", *this, &B::_type, SerializableFieldBase::Required{})
    , data("data", *this, &B::data, SerializableFieldBase::Required{}) {}
};
} // namespace serial
} // namespace bl

#endif
