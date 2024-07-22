#ifndef CORE_MAPS_WEATHER_SNOW_HPP
#define CORE_MAPS_WEATHER_SNOW_HPP

#include "Base.hpp"

#include "Thunder.hpp"
#include <BLIB/Particles.hpp>
#include <Core/Resources.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace snow
{
struct Snowflake;
struct TimeEmitter;
} // namespace snow

/**
 * @brief Snow weather system
 *
 * @ingroup Weather
 */
class Snow : public Base {
public:
    /**
     * @brief Create snowy weather
     *
     * @param hard True for hard snow, false for light
     * @param thunder True to enable thunder, false for no thunder
     */
    Snow(bool hard, bool thunder);

    /**
     * @brief Destroy the Snow object
     */
    virtual ~Snow();

    /**
     * @brief One of LightSnow, LightSnowThunder, HardSnow, HardSnowThunder
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Start the snow
     *
     * @param engine The game engine instance
     * @param renderTarget The render target the weather will be rendered to
     * @param map The map the weather is in
     */
    virtual void start(bl::engine::Engine& engine, bl::rc::RenderTarget& renderTarget,
                       Map& map) override;

    /**
     * @brief Stops the snow
     */
    virtual void stop() override;

    /**
     * @brief Returns true when no snow is left after a call to stop()
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates all the flakes and spawns more
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) override;

private:
    bl::engine::Engine* engine;
    const Weather::Type _type;
    const unsigned int targetParticleCount;
    const float fallSpeed;
    float stopFactor;

    bl::pcl::ParticleManager<snow::Snowflake>* particles;
    snow::TimeEmitter* emitter;
    Thunder thunder;
    bl::rc::res::TextureRef snowTxtr;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
