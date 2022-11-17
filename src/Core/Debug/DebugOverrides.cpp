#include <Core/Debug/DebugOverrides.hpp>

#include <Core/Debug/DebugBanner.hpp>

#ifdef PEOPLEMON_DEBUG

namespace core
{
namespace debug
{
namespace
{
std::string bToStr(bool b) { return b ? "ON" : "OFF"; }
} // namespace

DebugOverrides::DebugOverrides()
: skipBattles(false)
, alwaysCatch(false)
, alwaysRun(false) {}

const DebugOverrides& DebugOverrides::get() { return instance(); }

DebugOverrides& DebugOverrides::instance() {
    static DebugOverrides overrides;
    return overrides;
}

void DebugOverrides::subscribe() { bl::event::Dispatcher::subscribe(&instance()); }

void DebugOverrides::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::F2:
            skipBattles = !skipBattles;
            DebugBanner::display("SkipBattles: " + bToStr(skipBattles));
            break;

        case sf::Keyboard::F3:
            alwaysCatch = !alwaysCatch;
            DebugBanner::display("AlwaysCatch: " + bToStr(alwaysCatch));
            break;

        case sf::Keyboard::F4:
            alwaysRun = !alwaysRun;
            DebugBanner::display("AlwaysRun: " + bToStr(alwaysRun));
            break;

        default:
            break;
        }
    }
}

} // namespace debug
} // namespace core

#endif
