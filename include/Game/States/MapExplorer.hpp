#ifndef GAME_STATES_MAP_EXPLORER_HPP
#define GAME_STATES_MAP_EXPLORER_HPP

#include <BLIB/Render/Cameras.hpp>
#include <Core/Maps/Map.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Debug only state that allows free roaming the map with a special camera
 *
 * @ingroup States
 *
 */
class MapExplorer
: public State
, public bl::event::Listener<sf::Event> {
public:
    /**
     * @brief Creates a new MapExplorer state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Map Explorer state
     *
     */
    virtual ~MapExplorer() = default;

    /**
     * @brief Returns "MapExplorer"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Activates the state and camera. Discnnects the player from their entity
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Reconnects the player to their entity and pops the camera
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the game world logic and the hint fadeout
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the game and hint box
     *
     * @param engine The game engine
     * @param lag Time elapsed not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::render::camera::Camera::Ptr mapExplorer;
    sf::RectangleShape hintBox;
    sf::Text hintText;
    float hintTime;

    MapExplorer(core::system::Systems& systems);

    virtual void observe(const sf::Event& event) override;
};

} // namespace state
} // namespace game

#endif
