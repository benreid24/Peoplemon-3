#ifndef GAME_STATES_FLYMAP_HPP
#define GAME_STATES_FLYMAP_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Maps/Town.hpp>
#include <Game/States/State.hpp>
#include <SFML/Graphics.hpp>

namespace game
{
namespace state
{
class FlyMap
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Creates the fly map state
     *
     * @param systems The primary systems object
     * @param unpause Boolean param to set if the pause menu should close after this closes
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, bool& unpause);

    /**
     * @brief Destroy the New Game state
     *
     */
    virtual ~FlyMap() = default;

    /**
     * @brief Returns "FlyMap"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Activates the state
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Deactivates the state
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the state and menus and whatnot
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the new game features
     *
     * @param engine The game engine
     * @param lag Time elapsed not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    std::vector<sf::Vector2f> townPositions;
    bool hudActive;
    bool& unpause;

    bl::resource::Ref<sf::Texture> mapTxtr;
    bl::resource::Ref<sf::Texture> townTxtr;
    bl::resource::Ref<sf::Texture> cursorTxtr;
    bl::resource::Ref<sf::Texture> playerTxtr;
    sf::Sprite map;
    sf::Sprite town;
    sf::Sprite player;
    sf::Sprite cursor;
    bl::gfx::Flashing cursorFlasher;

    bl::resource::Ref<sf::Texture> panelTxtr;
    sf::Sprite panel;
    sf::Text townName;
    sf::Text townDesc;

    bl::menu::Menu townMenu;
    core::input::MenuDriver inputDriver;

    FlyMap(core::system::Systems& systems, bool& unpause);
    void clearHover();
    void hoverTown(unsigned int i);
    void selectTown(unsigned int i);
    void onFlyChoice(const std::string& choice, const core::map::Town& town);
    void messageDone();
    void close();
    void wrap();

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
