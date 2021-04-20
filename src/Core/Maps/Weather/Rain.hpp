#ifndef CORE_MAPS_WEATHER_RAIN_HPP
#define CORE_MAPS_WEATHER_RAIN_HPP

#include "Base.hpp"
#include "Thunder.hpp"

#include <BLIB/Media/Audio.hpp>
#include <BLIB/Particles.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector3.hpp>

namespace core
{
namespace map
{
namespace weather
{
class Rain : public Base {
public:
    Rain(bool hard, bool thunder);

    virtual ~Rain();

    virtual void start(const sf::FloatRect& area) override;

    virtual void stop() override;

    virtual bool stopped() const override;

    virtual void update(float dt) override;

    virtual void render(sf::RenderTarget& target, float residual) const override;

private:
    bl::particle::System<sf::Vector3f> rain;
    const sf::Vector3f fallVelocity;
    mutable sf::FloatRect area;

    bl::resource::Resource<sf::Texture>::Ref dropTxtr;
    mutable sf::Sprite drop;
    bl::resource::Resource<sf::Texture>::Ref splash1Txtr;
    mutable sf::Sprite splash1;
    bl::resource::Resource<sf::Texture>::Ref splash2Txtr;
    mutable sf::Sprite splash2;

    bl::resource::Resource<sf::SoundBuffer>::Ref rainSound;
    bl::audio::AudioSystem::Handle rainSoundHandle;

    Thunder thunder;

    void createDrop(sf::Vector3f* drop);
};

} // namespace weather
} // namespace map
} // namespace core

#endif
