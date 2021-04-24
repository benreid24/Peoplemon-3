#ifndef CORE_MAPS_WEATHER_FOG_HPP
#define CORE_MAPS_WEATHER_FOG_HPP

#include "Base.hpp"

#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Particles.hpp>

namespace core
{
namespace map
{
namespace weather
{
/**
 * @brief Weather type for thin or thick fog
 *
 * @ingroup Weather
 *
 */
class Fog : public Base {
public:
    /**
     * @brief Creates foggy weather
     *
     * @param thick True for thick fog, false for thin fog
     */
    Fog(bool thick);

    /**
     * @brief No more fog
     *
     */
    virtual ~Fog() = default;

    /**
     * @brief Starts the fog
     *
     * @param area The area to surround with fog
     */
    virtual void start(const sf::FloatRect& area) override;

    /**
     * @brief Stops the fog
     *
     */
    virtual void stop() override;

    /**
     * @brief Returns true when the fog is fully dissipated, false if not
     *
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates the fog
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) override;

    /**
     * @brief Renders the fog
     *
     * @param target Target to render to
     * @param residual Time in seconds not yet accounted for in update
     */
    virtual void render(sf::RenderTarget& target, float residual) const override;

private:
    struct Particle {
        sf::Vector2f position;
        float rotation;
        float angularVelocity;
        float scale;

        Particle() = default;
        void set(const sf::Vector2f& pos);
    };

    const std::uint8_t maxOpacity;
    std::uint8_t targetOpacity;
    float alpha;
    bl::resource::Resource<sf::Texture>::Ref fogTxtr;
    mutable sf::Sprite fog;
    mutable sf::FloatRect area;
    std::vector<Particle> particles;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
