#ifndef CORE_MAPS_WEATHER_SUNNY_HPP
#define CORE_MAPS_WEATHER_SUNNY_HPP

#include "Base.hpp"

#include <BLIB/Engine/Resources.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace map
{
namespace weather
{
/**
 * @brief Makes sunny weather
 *
 * @ingroup Weather
 *
 */
class Sunny : public Base {
public:
    /**
     * @brief Construct a new Sunny object
     *
     */
    Sunny();

    /**
     * @brief Destroy the Sunny object
     *
     */
    virtual ~Sunny() = default;

    /**
     * @brief Returns Sunny
     *
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Starts the sunny weather
     *
     */
    virtual void start(const sf::FloatRect&) override;

    /**
     * @brief Stops the sunny weather
     *
     */
    virtual void stop() override;

    /**
     * @brief Returns true when the sun is finished fading
     *
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates the sunny weather
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    virtual void update(float dt) override;

    /**
     * @brief Renders the sunny weather
     *
     * @param target Target to render to
     * @param lag Time not yet accounted for in update
     */
    virtual void render(sf::RenderTarget& target, float lag) const override;

private:
    mutable sf::RectangleShape sun;
    float t;
    bool stopping;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
