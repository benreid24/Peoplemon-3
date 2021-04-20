#include <Core/Systems/Clock.hpp>

#include <Core/Events/TimeChange.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>

namespace core
{
namespace system
{
Clock::Clock(Systems& owner)
: owner(owner)
, currentTime(12, 0, 1)
, residual(0.f) {}

const Clock::Time& Clock::now() const { return currentTime; }

void Clock::update(float dt) {
    residual += dt;
    const float minutes = std::floor(residual);
    if (minutes > 0) {
        residual -= minutes; // 1 real second = 1 game minute
        currentTime.addMinutes(static_cast<unsigned int>(minutes + 0.1f));
        owner.engine().eventBus().dispatch<event::TimeChange>({currentTime});
    }
}

Clock::Time::Time(unsigned int h, unsigned int m, unsigned int d)
: hour(h)
, minute(m)
, day(d) {}

void Clock::Time::addMinutes(unsigned int m) {
    const unsigned int nm = minute + m;
    const unsigned int h  = nm / 60;
    minute                = nm % 60;
    if (h > 0) addHours(h);
}

void Clock::Time::addHours(unsigned int h) {
    const unsigned int nh = hour + h;
    const unsigned int d  = nh / 24;
    hour                  = nh % 24;
    day += d;
}

bool Clock::Time::operator==(const Time& right) const {
    return minute == right.minute && hour == right.hour &&
           (right.day == 0 || day == 0 || day == right.day);
}

bool Clock::Time::operator<(const Time& right) const {
    if (day != 0 && right.day != 0) {
        if (day < right.day) return true;
        if (day > right.day) return false;
    }
    if (hour < right.hour)
        return true;
    else if (hour == right.hour)
        return minute < right.minute;
    return false;
}

bool Clock::Time::operator>(const Time& right) const {
    if (day != 0 && right.day != 0) {
        if (day < right.day) return false;
        if (day > right.day) return true;
    }
    if (hour > right.hour)
        return true;
    else if (hour == right.hour)
        return minute > right.minute;
    return false;
}

} // namespace system
} // namespace core
