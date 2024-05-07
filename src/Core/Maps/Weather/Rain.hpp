#ifndef CORE_MAPS_WEATHER_RAIN_HPP
#define CORE_MAPS_WEATHER_RAIN_HPP

#include "Base.hpp"
#include "Thunder.hpp"

#include <BLIB/Audio.hpp>
#include <BLIB/Particles.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/Random.hpp>
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
    struct Raindrop {
        glm::vec2 pos;
        float height;

        Raindrop() = default;

        Raindrop(const sf::FloatRect& area) {
            pos.x = bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300.f);
            pos.y = bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f);
            height = bl::util::Random::get<float>(120.f, 180.f);
        }
    };

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
     * @param engine The game engine instance
     */
    virtual void start(bl::engine::Engine& engine) override;

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

private:
    bl::engine::Engine* engine;
    bl::pcl::ParticleManager<Raindrop>* particles;
    const Weather::Type _type;
    const unsigned int targetParticleCount;
    const sf::Vector3f fallVelocity;
    float stopFactor;

    bl::resource::Ref<sf::Texture> dropTxtr;
    mutable sf::Sprite drop;
    bl::resource::Ref<sf::Texture> splash1Txtr;
    mutable sf::Sprite splash1;
    bl::resource::Ref<sf::Texture> splash2Txtr;
    mutable sf::Sprite splash2;

    bl::audio::AudioSystem::Handle rainSoundHandle;

    Thunder thunder;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
