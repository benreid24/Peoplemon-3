#ifndef CORE_MAPS_WEATHER_SNOW_HPP
#define CORE_MAPS_WEATHER_SNOW_HPP

#include "Base.hpp"

#include "Thunder.hpp"
#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Particles.hpp>
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
     * @brief Start the snow
     *
     * @param area Initial area to spawn flakes in
     */
    virtual void start(const sf::FloatRect& area) override;

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

    /**
     * @brief Renders the snow
     *
     * @param target Target to render to
     * @param residual Time elapsed in seconds not accounted for in update
     */
    virtual void render(sf::RenderTarget& target, float residual) const override;

private:
    struct Flake {
        sf::Vector3f position;
        sf::Vector3f velocity;

        Flake(float x, float y, float z, float fallSpeed);
    };

    const float fallSpeed;
    bl::resource::Resource<sf::Texture>::Ref snowTxtr;
    mutable sf::Sprite snowFlake;
    mutable sf::FloatRect area;

    bl::particle::System<Flake> snow;
    Thunder thunder;

    void createFlake(Flake* flake);
};

} // namespace weather
} // namespace map
} // namespace core

#endif
