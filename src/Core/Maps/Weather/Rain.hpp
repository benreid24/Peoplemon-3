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
namespace rain
{
struct Raindrop;
class TimeEmitter;
} // namespace rain

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
     */
    virtual ~Rain();

    /**
     * @brief One of LightRain, LightRainThunder, HardRain, or HardRainThunder
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Start the rain
     *
     * @param engine The game engine instance
     * @param renderTarget The render target the weather will be rendered to
     * @param map The map the weather is in
     */
    virtual void start(bl::engine::Engine& engine, bl::rc::RenderTarget& renderTarget,
                       Map& map) override;

    /**
     * @brief Stop the rain
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
    bl::pcl::ParticleManager<rain::Raindrop>* particles;
    rain::TimeEmitter* emitter;
    const Weather::Type _type;
    const unsigned int targetParticleCount;
    const glm::vec2 velocity;
    const float fallSpeed;
    const float rotation;
    float stopFactor;

    bl::rc::res::TextureRef dropTxtr;
    bl::rc::res::TextureRef splash1Txtr;
    bl::rc::res::TextureRef splash2Txtr;

    bl::audio::AudioSystem::Handle rainSoundHandle;

    Thunder thunder;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
