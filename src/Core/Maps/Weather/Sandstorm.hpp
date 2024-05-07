#ifndef CORE_MAPS_WEATHER_SANDSTORM_HPP
#define CORE_MAPS_WEATHER_SANDSTORM_HPP

#include "Base.hpp"

#include <Core/Resources.hpp>

namespace core
{
namespace map
{
namespace weather
{
/**
 * @brief Creates sandstorms
 *
 * @ingroup Weather
 *
 */
class Sandstorm : public Base {
public:
    /**
     * @brief Creates the sandstorm
     *
     */
    Sandstorm();

    /**
     * @brief Destroy the Sandstorm object
     *
     */
    virtual ~Sandstorm() = default;

    /**
     * @brief Returns Sandstorm
     *
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Starts the sandstorm
     *
     * @param engine The game engine instance
     */
    virtual void start(bl::engine::Engine& engine) override;

    /**
     * @brief Stops the sandstorm
     *
     */
    virtual void stop() override;

    /**
     * @brief Returns true if the sandstorm is fully stopped, false if stopping
     *
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates the sandstorm
     *
     * @param dt Time elapsed in seconds since the last call to update
     */
    virtual void update(float dt) override;

private:
    struct Swirl {
        sf::Vector2f position;
        float angle;
        float scale;
        float angularVel;

        void set(float x, float y);
    };

    bl::resource::Ref<sf::Texture> sandTxtr;
    bl::resource::Ref<sf::Texture> swirlTxtr;
    mutable sf::Sprite sand;
    mutable sf::Sprite swirl;
    mutable sf::FloatRect area;
    std::vector<sf::Vector2f> sandPatches;
    std::vector<Swirl> swirlParticles;
    std::uint8_t targetAlpha;
    float alpha;
};

} // namespace weather
} // namespace map
} // namespace core

#endif
