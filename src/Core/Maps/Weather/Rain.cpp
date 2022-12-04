#include "Rain.hpp"

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace
{
constexpr float SplashTime = -0.15f;
constexpr float TransTime  = -0.3f;
constexpr float DeadTime   = -0.4f;
constexpr float StopTime   = 4.f;
constexpr float StopSpeed  = 1.f / StopTime;
} // namespace

Rain::Rain(bool hard, bool canThunder)
: _type(hard ? (canThunder ? Weather::HardRainThunder : Weather::HardRain) :
               (canThunder ? Weather::LightRainThunder : Weather::LightRain))
, targetParticleCount(hard ? Properties::HardRainParticleCount() :
                             Properties::LightRainParticleCount())
, rain(std::bind(&Rain::createDrop, this, std::placeholders::_1), targetParticleCount, 200.f)
, fallVelocity(hard ? sf::Vector3f(-90.f, 30.f, -740.f) : sf::Vector3f(0.f, 0.f, -500.f))
, stopFactor(-1.f)
, thunder(canThunder, hard) {
    dropTxtr = TextureManager::load(Properties::RainDropFile()).data;
    drop.setTexture(*dropTxtr, true);
    drop.setRotation(hard ? 45.f : 15.f);
    drop.setOrigin(dropTxtr->getSize().x / 2, dropTxtr->getSize().y);
    splash1Txtr = TextureManager::load(Properties::RainSplash1File()).data;
    splash1.setTexture(*splash1Txtr, true);
    splash1.setRotation(hard ? 45.f : 15.f);
    splash1.setOrigin(splash1Txtr->getSize().x / 2, splash1Txtr->getSize().y);
    splash2Txtr = TextureManager::load(Properties::RainSplash2File()).data;
    splash2.setTexture(*splash2Txtr, true);
    splash2.setOrigin(splash2Txtr->getSize().x / 2, splash2Txtr->getSize().y);

    rainSoundHandle = bl::audio::AudioSystem::getOrLoadSound(
        hard ? Properties::HardRainSoundFile() : Properties::LightRainSoundFile());

    rain.setReplaceDestroyed(true);
}

Rain::~Rain() { stop(); }

Weather::Type Rain::type() const { return _type; }

void Rain::start(const sf::FloatRect& a) {
    area = a;
    bl::audio::AudioSystem::playSound(rainSoundHandle, 1.5f, true);
}

void Rain::stop() {
    stopFactor = 0.f;
    bl::audio::AudioSystem::stopSound(rainSoundHandle, StopTime + 0.5f);
    thunder.stop();
}

bool Rain::stopped() const { return rain.particleCount() == 0; }

void Rain::update(float dt) {
    const auto updateDrop = [this, dt](sf::Vector3f& drop) -> bool {
        if (drop.z >= 0.f) {
            drop += this->fallVelocity * dt;
            if (drop.z < 0.f) drop.z = -0.001f;
            return true;
        }
        else {
            drop.z -= dt;
            return drop.z >= DeadTime;
        }
    };

    rain.update(updateDrop, dt);
    thunder.update(dt);

    if (stopFactor >= 0.f) {
        stopFactor = std::min(stopFactor + StopSpeed * dt, 1.f);
        rain.setTargetCount(
            targetParticleCount -
            static_cast<unsigned int>(static_cast<float>(targetParticleCount) * stopFactor));
        if (stopFactor >= 1.f) {
            stopFactor = -1.f;
            rain.setTargetCount(0);
        }
    }
}

void Rain::render(sf::RenderTarget& target, float lag) const {
    const auto renderDrop = [this, &target, lag](const sf::Vector3f& drop) {
        if (drop.z >= 0.f) {
            const sf::Vector3f pos = drop + this->fallVelocity * lag;
            const sf::Vector2f tpos(pos.x + pos.z * 0.25f, pos.y - pos.z * 0.5f);
            this->drop.setPosition(tpos);
            target.draw(this->drop);
        }
        else {
            sf::Sprite& spr = drop.z >= SplashTime ? this->splash1 : this->splash2;
            spr.setPosition(drop.x, drop.y);
            static const float TransLen = std::abs(DeadTime - TransTime);
            const float s               = std::min((drop.z - TransTime) / TransLen, 0.f);
            const float minusA          = 255.f * s;
            spr.setColor(sf::Color(255, 255, 255, 255 + minusA));
            target.draw(spr);
        }
    };

    area = {target.getView().getCenter() - target.getView().getSize() * 0.5f,
            target.getView().getSize()};
    rain.render(renderDrop);
    thunder.render(target, lag);
}

void Rain::createDrop(sf::Vector3f* drop) {
    drop = new (drop) sf::Vector3f(
        bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300.f),
        bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f),
        bl::util::Random::get<float>(120.f, 180.f));
}

} // namespace weather
} // namespace map
} // namespace core
