#ifndef CORE_SYSTEMS_CLOCK_HPP
#define CORE_SYSTEMS_CLOCK_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Events/GameSave.hpp>
#include <iomanip>
#include <ostream>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Simple time keeping systems. Tracks in game time and date based on real elapsed play time
 *
 * @ingroup Systems
 */
class Clock
: private bl::util::NonCopyable
, public bl::engine::System
, public bl::event::Listener<event::GameSaveInitializing, event::GameSaveLoaded> {
public:
    /**
     * @brief Simple struct representing a point in time
     *
     * @ingroup Systems
     */
    struct Time {
        /// Number of days elapsed in game since beginning. Starts at 1
        unsigned int day;

        /// Current hour of the day, in range [0, 23]
        unsigned int hour;

        /// Current minute of the hour. In range [0, 59]
        unsigned int minute;

        /**
         * @brief Construct a new Time at noon of day 0
         *
         */
        Time();

        /**
         * @brief Creates a new time
         *
         * @param hour The hour of the time
         * @param minute The minute of the time
         * @param day The day of the time. Set to 0 to ignore when comparing times
         */
        Time(unsigned int hour, unsigned int minute, unsigned int day = 0);

        /**
         * @brief Adds the amount of minutes to this time. Wraps hours and days
         *
         * @param minutes The number of minutes to add
         */
        void addMinutes(unsigned int minutes);

        /**
         * @brief Adds the number of hours to this time. Wraps days
         *
         * @param hours The number of hours to add
         */
        void addHours(unsigned int hours);

        /**
         * @brief Checks if two times are the same. Compares days as well unless either day is 0
         *
         * @param right The time to check against
         * @return True if the times are the same, false if different
         */
        bool operator==(const Time& right) const;

        /**
         * @brief Checks to see if a given time appears later than the current time. Counts days
         *        unless either day is 0
         *
         * @param right The time to check against
         * @return True if right is later than this time
         */
        bool operator<(const Time& right) const;

        /**
         * @brief Checks to see if a given time appears earlier than the current time. Counts days
         *        unless either day is 0
         *
         * @param right The time to check against
         * @return True if right is earlier than this time
         */
        bool operator>(const Time& right) const;

        friend std::ostream& operator<<(std::ostream& stream, const Time& time) {
            stream << std::setfill('0') << std::setw(2) << time.hour << ":" << std::setfill('0')
                   << std::setw(2) << time.minute;
            return stream;
        }
    };

    /**
     * @brief Initializes the clock system
     *
     * @param owner The owner of this system
     */
    Clock(Systems& owner);

    /**
     * @brief Destroys the system
     */
    virtual ~Clock() = default;

    /**
     * @brief Returns the current in game time
     *
     * @return const Time& The current in game time
     */
    const Time& now() const;

    /**
     * @brief Sets the current time
     *
     * @param time The new current time
     */
    void set(const Time& time);

    /**
     * @brief Adds saved clock data to the save file
     */
    virtual void observe(const event::GameSaveInitializing& save) override;

    /**
     * @brief Initializes clock state from the loading game save
     */
    virtual void observe(const event::GameSaveLoaded& load) override;

private:
    Systems& owner;
    Time currentTime;
    float residual;

    virtual void init(bl::engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace system
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::system::Clock::Time> : public SerializableObjectBase {
    using T = core::system::Clock::Time;

    SerializableField<1, T, unsigned int> day;
    SerializableField<2, T, unsigned int> hour;
    SerializableField<3, T, unsigned int> minute;

    SerializableObject()
    : SerializableObjectBase("ClockTime")
    , day("day", *this, &T::day, SerializableFieldBase::Required{})
    , hour("hour", *this, &T::hour, SerializableFieldBase::Required{})
    , minute("minute", *this, &T::minute, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
