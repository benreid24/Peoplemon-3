#include "Rain.hpp"

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Util/Random.hpp>

namespace core
{
namespace map
{
namespace weather
{
// TODO - get stuff from properties
Rain::Rain(bool hard, bool canThunder)
: rain(std::bind(&Rain::createDrop, this, std::placeholders::_1), hard ? 1200 : 750, 200.f)
, fallVelocity(hard ? sf::Vector3f(-0.030303f, 0.030303f, -0.757575f) :
                      sf::Vector3f(0.f, 0.f, -0.515151f))
, thunder(canThunder) {
    dropTxtr = bl::engine::Resources::textures().load("Resources/Images/Weather/rainDrop.png").data;
    drop.setTexture(*dropTxtr, true);
    splash1Txtr =
        bl::engine::Resources::textures().load("Resources/Images/Weather/rainSplash1.png").data;
    splash1.setTexture(*splash1Txtr, true);
    splash2Txtr =
        bl::engine::Resources::textures().load("Resources/Images/Weather/rainSplash2.png").data;
    splash2.setTexture(*splash2Txtr, true);

    rainSound =
        bl::engine::Resources::sounds().load("Resources/Audio/Sounds/Weather/lightRain.wav").data;
    rainSoundHandle = bl::audio::AudioSystem::InvalidHandle;
}

Rain::~Rain() { stop(); }

void Rain::start(const sf::FloatRect& a) {
    area = a;
    if (rainSoundHandle != bl::audio::AudioSystem::InvalidHandle) {
        bl::audio::AudioSystem::stopSound(rainSoundHandle);
    }
    rainSoundHandle = bl::audio::AudioSystem::playSound(rainSound, true);
}

void Rain::stop() {
    rain.setTargetCount(0);
    bl::audio::AudioSystem::stopSound(rainSoundHandle, 2.5f);
}

bool Rain::stopped() const { return rain.particleCount() == 0; }

void Rain::update(float dt) {
    const auto updateDrop = [this, dt](sf::Vector3f& drop) {
        if (drop.z > 0.f) { drop += this->fallVelocity * dt; }
        else {
            drop.z -= dt;
        }

        return drop.z > -1.5f;
    };

    rain.update(updateDrop, dt);
    thunder.update(dt);
}

void Rain::render(sf::RenderTarget& target, float lag) const {
    const auto renderDrop = [this, &target, lag](const sf::Vector3f& drop) {
        if (drop.z > 0.f) {
            const sf::Vector3f pos = drop + this->fallVelocity * lag;
            const sf::Vector2f tpos(pos.x - pos.z / 4.f, pos.y - pos.z / 2.f);
            this->drop.setPosition(tpos);
            target.draw(this->drop);
        }
        else {
            sf::Sprite& spr = drop.z >= -0.75f ? this->splash1 : this->splash2;
            spr.setPosition(drop.x, drop.y);
            target.draw(spr);
        }
    };

    area = {target.getView().getCenter() - target.getView().getSize() / 2.f,
            target.getView().getSize()};
    rain.render(renderDrop);
    thunder.render(target, lag);
}

void Rain::createDrop(sf::Vector3f* drop) {
    drop = new (drop)
        sf::Vector3f(bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300),
                     bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f),
                     bl::util::Random::get<float>(50.f, 90.f));
}

} // namespace weather
} // namespace map
} // namespace core
