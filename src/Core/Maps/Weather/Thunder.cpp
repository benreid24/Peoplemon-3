#include "Thunder.hpp"

#include <BLIB/Audio/AudioSystem.hpp>
#include <BLIB/Util/Random.hpp>
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
    lightning.setFillColor(sf::Color::Transparent);
    if (e) { sound = bl::audio::AudioSystem::getOrLoadSound(Properties::ThunderSoundFile()); }
}

Thunder::~Thunder() { bl::audio::AudioSystem::stopSound(sound, 0.5f); }

void Thunder::stop() { stopping = true; }

void Thunder::update(float dt) {
    if (enabled) {
        timeSinceLastThunder += dt;
        if (lightning.getFillColor().a != 0) {
            const float a = computeAlpha(timeSinceLastThunder);
            if (a <= 0.f) { lightning.setFillColor(sf::Color::Transparent); }
            else { lightning.setFillColor(sf::Color(255, 255, 255, a)); }
        }
        else if (!stopping) {
            if (bl::util::Random::get<float>(minInterval, maxInterval) <= timeSinceLastThunder) {
                timeSinceLastThunder = 0.f;
                bl::audio::AudioSystem::playSound(sound);
                lightning.setFillColor(sf::Color(255, 255, 255, computeAlpha(0.f)));
            }
        }
    }
}

// void Thunder::render(sf::RenderTarget& target, float) const {
//     if (enabled && lightning.getFillColor().a > 0) {
//         lightning.setOrigin(target.getView().getSize() / 2.f);
//         lightning.setPosition(target.getView().getCenter());
//         lightning.setSize(target.getView().getSize());
//         target.draw(lightning);
//     }
// }

} // namespace weather
} // namespace map
} // namespace core
