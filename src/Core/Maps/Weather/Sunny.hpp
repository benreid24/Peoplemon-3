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
class Sunny : public Base {
public:
    Sunny();

    virtual ~Sunny() = default;

    virtual void start(const sf::FloatRect&) override;

    virtual void stop() override;

    virtual bool stopped() const override;

    virtual void update(float dt) override;

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
