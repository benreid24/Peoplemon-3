#include "Thunder.hpp"

#include <BLIB/Audio/AudioSystem.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Events/Weather.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <cmath>

namespace core
{
namespace map
{
namespace weather
{
namespace
{
float computeAlpha(float time) {
    if (time <= 0.5f) {
        const float x = time - 0.2f;
        return -1000.f * x * x + 225;
    }
    const float x = (time - 0.5f) - 0.2f;
    return -550.f * x * x + 200;
}
} // namespace

Thunder::Thunder(bool e, bool f)
: enabled(e)
, minInterval(f ? Properties::FrequentThunderMinInterval() :
                  Properties::InfrequentThunderMinInterval())
, maxInterval(f ? Properties::FrequentThunderMaxInterval() :
                  Properties::InfrequentThunderMaxInterval())
, timeSinceLastThunder(0.f)
, stopping(false) {
    if (e) { sound = bl::audio::AudioSystem::getOrLoadSound(Properties::ThunderSoundFile()); }
}

Thunder::~Thunder() { bl::audio::AudioSystem::stopSound(sound, 0.5f); }

void Thunder::stop() { stopping = true; }

void Thunder::update(float dt) {
    if (enabled) {
        timeSinceLastThunder += dt;
        if (!stopping) {
            if (bl::util::Random::get<float>(minInterval, maxInterval) <= timeSinceLastThunder) {
                timeSinceLastThunder = 0.f;
                bl::audio::AudioSystem::playSound(sound);
                bl::event::Dispatcher::dispatch<event::Thundered>({});
            }
        }
    }
}

} // namespace weather
} // namespace map
} // namespace core
