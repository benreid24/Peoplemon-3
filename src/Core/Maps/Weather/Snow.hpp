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
/**
 * @brief Snow weather system
 *
 * @ingroup Weather
 *
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
     *
     */
    virtual ~Snow() = default;

    /**
     * @brief One of LightSnow, LightSnowThunder, HardSnow, HardSnowThunder
     *
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Start the snow
     *
     * @param engine The game engine instance
     * @param map The map the weather is in
     */
    virtual void start(bl::engine::Engine& engine, Map& map) override;

    /**
     * @brief Stops the snow
     *
     */
    virtual void stop() override;

    /**
     * @brief Returns true when no snow is left after a call to stop()
     *
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates all the flakes and spawns more
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) override;

private:
    struct Flake {
        sf::Vector3f position;
        sf::Vector3f velocity;

        Flake() = default;
        Flake(float x, float y, float z, float fallSpeed);
    };

    const Weather::Type _type;
    const unsigned int targetParticleCount;
    const float fallSpeed;
    bl::resource::Ref<sf::Texture> snowTxtr;
    mutable sf::Sprite snowFlake;
    mutable sf::FloatRect area;
    float stopFactor;

    bl::particle::System<Flake> snow;
    Thunder thunder;

    void createFlake(Flake* flake);
};

} // namespace weather
} // namespace map
} // namespace core

#endif
