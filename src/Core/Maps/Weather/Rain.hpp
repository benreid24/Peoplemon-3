#ifndef CORE_MAPS_WEATHER_RAIN_HPP
#define CORE_MAPS_WEATHER_RAIN_HPP

#include "Base.hpp"
#include "Thunder.hpp"

#include <BLIB/Audio.hpp>
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
/**
 * @brief Weather type for rainy days. Handles light and hard rain and owns thunder if need be
 *
 * @ingroup Weather
 *
 */
class Rain : public Base {
public:
    /**
     * @brief Construct a new Rain system
     *
     * @param hard True to make hard rain, false for light
     * @param thunder True for periodic thunder, false for no thunder
     */
    Rain(bool hard, bool thunder);

    /**
     * @brief Destroy the Rain
     *
     */
    virtual ~Rain();

    /**
     * @brief One of LightRain, LightRainThunder, HardRain, or HardRainThunder
     *
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Start the rain
     *
     * @param area The initial area to spawn drops in
     */
    virtual void start(const sf::FloatRect& area) override;

    /**
     * @brief Stop the rain
     *
     */
    virtual void stop() override;

    /**
     * @brief Returns true when all rain is fallen after stop() is called
     *
     * @return True if rain done terminating, false if rain still falling
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates the rain
     *
     * @param dt Time elapsed since last call to update, in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Render the rain and thunder if any
     *
     * @param target The target to render to
     * @param residual Residual time not yet accounted for in update
     */
    virtual void render(sf::RenderTarget& target, float residual) const override;

private:
    const Weather::Type _type;
    const unsigned int targetParticleCount;
    bl::particle::System<sf::Vector3f> rain;
    const sf::Vector3f fallVelocity;
    mutable sf::FloatRect area;
    float stopFactor;

    bl::resource::Ref<sf::Texture> dropTxtr;
    mutable sf::Sprite drop;
    bl::resource::Ref<sf::Texture> splash1Txtr;
    mutable sf::Sprite splash1;
    bl::resource::Ref<sf::Texture> splash2Txtr;
    mutable sf::Sprite splash2;

    bl::audio::AudioSystem::Handle rainSoundHandle;

    Thunder thunder;

    void createDrop(sf::Vector3f* drop);
};

} // namespace weather
} // namespace map
} // namespace core

#endif
