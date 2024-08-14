#ifndef CORE_MAPS_WEATHER_SANDSTORM_HPP
#define CORE_MAPS_WEATHER_SANDSTORM_HPP

#include "Base.hpp"

#include <BLIB/Particles.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace sandstorm
{
struct Sand;
struct Swirl;
} // namespace sandstorm

/**
 * @brief Creates sandstorms
 *
 * @ingroup Weather
 */
class Sandstorm : public Base {
public:
    /**
     * @brief Creates the sandstorm
     */
    Sandstorm();

    /**
     * @brief Destroy the Sandstorm object
     */
    virtual ~Sandstorm();

    /**
     * @brief Returns Sandstorm
     */
    virtual Weather::Type type() const override;

    /**
     * @brief Starts the sandstorm
     *
     * @param engine The game engine instance
     * @param renderTarget The render target the weather will be rendered to
     * @param map The map the weather is in
     */
    virtual void start(bl::engine::Engine& engine, bl::rc::RenderTarget& renderTarget,
                       Map& map) override;

    /**
     * @brief Stops the sandstorm
     */
    virtual void stop() override;

    /**
     * @brief Returns true if the sandstorm is fully stopped, false if stopping
     */
    virtual bool stopped() const override;

    /**
     * @brief Updates the sandstorm
     *
     * @param dt Time elapsed in seconds since the last call to update
     */
    virtual void update(float dt) override;

private:
    bl::engine::Engine* engine;
    bl::rc::res::TextureRef sandTxtr;
    bl::rc::res::TextureRef swirlTxtr;
    bl::pcl::ParticleManager<sandstorm::Sand>* sand;
    bl::pcl::ParticleManager<sandstorm::Swirl>* swirls;
    float targetAlpha;
    float alpha;

    void setAlpha();
};

} // namespace weather
} // namespace map
} // namespace core

#endif
