#ifndef CORE_FILES_BEHAVIOR_HPP
#define CORE_FILES_BEHAVIOR_HPP

#include <BLIB/Files/Binary.hpp>
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
class Behavior : public bl::file::binary::SerializableObject {
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
            unsigned int steps;

            /**
             * @brief Construct a new Pace
             *
             * @param dir The direction to walk in
             * @param steps The number of steps to take
             */
            Pace(component::Direction dir, unsigned int steps);
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
    bool legacyLoad(bl::file::binary::File& input);

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
};

} // namespace file
} // namespace core

#endif
